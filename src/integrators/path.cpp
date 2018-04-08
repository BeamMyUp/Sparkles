#include <nori/integrators/path.h>
#include <nori/shapes/shape.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/core/scene.h>
#include <nori/warp/warp.h>
#include <nori/samplers/sampler.h>
#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

PathIntegrator::PathIntegrator(const PropertyList &props)
	: m_terminationParam(props.getFloat("termination-param", 2))
	, m_nSamples(props.getInteger("nSamples", 1)) {

	std::string termination = props.getString("termination");
	if (termination == "russian-roulette") {
		m_termination = Termination::ERussianRoulette;
	}
	else {
		m_termination = Termination::EMaxDepth;
	}

	m_directMeasure = getMeasure(props.getString("direct-measure", "none"));
	m_indirectMeasure = getMeasure(props.getString("indirect-measure"));
	m_directWarpType = Warp::getWarpType(m_directMeasure, props.getString("direct-warp", "none"));
	m_indirectWarpType = Warp::getWarpType(m_indirectMeasure, props.getString("indirect-warp"));

	if (props.getBoolean("isExplicit", false))
		m_Li = &PathIntegrator::explicitLi;
	else
		m_Li = &PathIntegrator::implicitLi;
}

// Implicit Path tracing
Color3f PathIntegrator::implicitLi(const Scene* scene, Sampler* sampler, const Ray3f &ray) const {
	Color3f Lacc(1.f); 
	Ray3f _ray(ray);
	
	// Loop until the path escapes, hits a light or is forced to terminate
	for (uint32_t nDepth = 0; ;) {

		Intersection its;
		bool hit = scene->rayIntersect(_ray, its);
		
		if (hit) {
			// If the ray hit the light, get Le's contribution and terminate path
			if (its.shape->isEmitter()) {
				Lacc *= its.shape->getEmitter()->getRadiance();
				break; 
			}

			// If it hits a surface, bounce
			else{
				++nDepth;

				// Check termination condition
				if (stopPath(nDepth)) {
					return Color3f(0.0f);
				}

				// Calculate next Ray
				Warp::WarpQueryRecord wRec;
				Warp::warp(wRec, Warp::EWarpType::ECosineHemisphere, sampler->next2D());
				Vector3f wi = its.toWorld(wRec.warpedPoint);

				// Calculate Local Coordinates
				Vector3f woLocal(its.toLocal(-_ray.d));
				Vector3f wiLocal(its.toLocal(wi));

				// Calculate BSDF + cosine factor
				BSDFQueryRecord bRec(wiLocal, woLocal, EMeasure::ESolidAngle);
				Color3f fr = its.shape->getBSDF()->eval(bRec);
				float cosTheta = zeroClamp(wi.dot(its.shFrame.n));

				// Accumulate L (L = Le + \int fr*L*cosTheta)
				Lacc *= (fr * cosTheta) / wRec.pdf;

				// Build next ray
				_ray = Ray3f(its.p, wi); 
			}
		}

		// The path escapes the scene 
		else {
			return Color3f(0.f);
		}
	}
	return Lacc; 
}

// Explicit Path Tracing
Color3f PathIntegrator::explicitLi(const Scene* scene, Sampler* sampler, const Ray3f &ray) const {
	Color3f Lacc(1.f);
	Ray3f _ray(ray);

	Intersection its;
	bool hit = scene->rayIntersect(_ray, its);

	if (hit) {
		// If the ray hit the light, get Le's contribution and terminate path
		if (its.shape->isEmitter()) {
			return its.shape->getEmitter()->getRadiance();
		}

		// Loop until the path escapes or is forced to terminate
		for (uint32_t nDepth = 0; ;) {
			// TODO;
		}
	}
}

Color3f PathIntegrator::simplifiedDirect(const Scene* scene, Sampler* sampler, const Ray3f &ray, const Intersection& its) const {
	Color3f Li(0.f);

	// Get the extents of the scene
	float maxt = scene->getBoundingBox().getExtents().norm();

	// Assumes only one direct ray is cast
	for (auto emitter : scene->getEmitters()) {
		Color3f f(0.f);

		// Sample
		Point2f sample = sampler->next2D();
		SampleQueryRecord sqr;

		// If EArea or ESolidAngle, sample the emitter
		if (m_directMeasure == EMeasure::EArea || m_directMeasure == EMeasure::ESolidAngle)
			emitter->sample(sqr, m_directMeasure, sample, &its.p);

		// Check Pdf validity
		if (sqr.pdf == 0 /* && !deltaPDF*/) // TODO: code Delta pdf concept (have isDelta in bsdf)
			return Color3f(0.0f);

		// Query Emitter's radiance and wi. If solid-angle, wi is the sample directly
		EmitterQueryRecord eqr;
		emitter->eval(eqr, its.p, &sqr.sample.p);

		Vector3f wi = sqr.sample.v;
		if (m_directMeasure == EMeasure::EArea) // Update sampled direction if Area Sampling
			wi = eqr.wi;

		// Visibility test
		Ray3f lightRay = Ray3f(its.p, wi, Epsilon, maxt);
		Intersection itsLight;
		bool intersects = scene->rayIntersect(lightRay, itsLight);

		// If point is visible, calculate final color
		if (intersects && emitter == itsLight.shape->getEmitter()) {

			// Calculate Local Coordinates
			Vector3f woLocal(its.toLocal(-ray.d));
			Vector3f wiLocal(its.toLocal(wi));

			// Calculate the weight of the light
			float weight = 0;
			switch (m_directMeasure)
			{
			case EMeasure::EArea: {
				// Weight by geometry term
				float d2 = (sqr.sample.p - its.p).squaredNorm();
				float cosThetaO = zeroClamp(wi.dot(sqr.n));
				float cosThetaI = zeroClamp(wi.dot(its.shFrame.n));
				weight = (cosThetaI * cosThetaO) / d2;
				break;
			}
			case EMeasure::ESolidAngle:
				weight = zeroClamp(wi.dot(its.shFrame.n));
				break;
			}

			// Evaluate rendering equation
			BSDFQueryRecord bsr(wiLocal, woLocal, EMeasure::ESolidAngle);
			f = its.shape->getBSDF()->eval(bsr);

			Li += weight * eqr.Le * f / sqr.pdf;
		}
	}

	return Li;
}

Color3f PathIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	return m_Li(this, scene, sampler, ray);
}

bool PathIntegrator::stopPath(uint32_t currentDepth) const {
	bool stop = false;
	
	if (currentDepth > static_cast<uint32_t>(m_terminationParam))
		stop = true;

	// TODO: Implement Russian Roulette here

	return stop; 
}

std::string PathIntegrator::toString() const {
	return tfm::format(
		"PathIntegrator[]"
	);
}

NORI_REGISTER_CLASS(PathIntegrator, "path");
NORI_NAMESPACE_END