#include <nori/integrators/direct.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>
#include <nori/warp/warp.h>
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

DirectIntegrator::DirectIntegrator(const PropertyList &props)
	: m_nSamples(props.getInteger("nSamples", 1)){
	std::string measure = props.getString("measure", "");

	if (measure == "area")
		m_measure = EArea;
	else if (measure == "solid-angle")
		m_measure = ESolidAngle;
	else if (measure == "hemisphere")
		m_measure = EHemisphere;
	else
		throw NoriException("DirectIntegrator::DirectIntegrator : Cannot support given measure");

	std::string warpType = "";
	if (m_measure == EHemisphere)
		warpType = props.getString("warp-type", "");

	m_warpType = Warp::getWarpType(m_measure, warpType);
}

Color3f DirectIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	Color3f Li(0.f);

	// Find if the surface is visible from camera
	Intersection its;
	if (!scene->rayIntersect(ray, its))
		return Li;

	// If the intersection hits the light, return the radiance
	if (its.shape->isEmitter())
		return its.shape->getEmitter()->getRadiance();

	// Get the extents of the scene
	float maxt = scene->getBoundingBox().getExtents().norm();

	for (int i = 0; i < m_nSamples; ++i) {
		for (auto emitter : scene->getEmitters()) {
			
			// Sample
			SampleQueryRecord sqr; 
			Point2f sample = sampler->next2D();

			// If EArea or ESolidAngle, sample the emitter
			if(m_measure == EArea || m_measure == ESolidAngle)
				emitter->sample(sqr, m_measure, sample, &its.p);

			// If EHemisphere, sample on an hemisphere around the point according to warpType
			if (m_measure == EHemisphere) {
				Warp::WarpQueryRecord wqr;
				Warp::warp(wqr, m_warpType, sample);
				sqr.sample.v = wqr.warpedPoint;
				sqr.pdf = wqr.pdf;
			}
			
			// Check Pdf validity
			if (sqr.pdf == 0 /* && !deltaPDF*/) // TODO: code Delta pdf concept (have isDelta in bsdf)
				return Li; 
	
			// Query Emitter's radiance and wi. If solid-angle, wi is the sample directly
			EmitterQueryRecord eqr;
			emitter->eval(eqr, its.p, &sqr.sample.p); 
			if (m_measure == ESolidAngle || m_measure == EHemisphere)
				eqr.wi = sqr.sample.v;

			// Visibility test
			Ray3f lightRay = Ray3f(its.p, eqr.wi, Epsilon, maxt); 
			Intersection itsLight; 
			bool intersects = scene->rayIntersect(lightRay, itsLight);

			// If point is visible, calculate final color
			if (intersects && emitter == itsLight.shape->getEmitter()) {
				float weight = 0;

				// Calculate Local Coordinates
				Vector3f woLocal(its.toLocal(-ray.d));
				Vector3f wiLocal(its.toLocal(eqr.wi));

				// Calculate the weight of the light
				switch (m_measure)
				{
				case EArea: {
					// Weight by geometry term
					float d2 = (sqr.sample.p - its.p).squaredNorm();
					float cosThetaO = zeroClamp(eqr.wi.dot(sqr.n));
					float cosThetaI = zeroClamp(eqr.wi.dot(its.shFrame.n));
					weight = (cosThetaI * cosThetaO) / d2;
					break;
				}
				case EHemisphere:
				case ESolidAngle:
					weight = zeroClamp(eqr.wi.dot(its.shFrame.n)); 
					break;
				}
				
				// Evaluate rendering equation
				BSDFQueryRecord bsr(wiLocal, woLocal, ESolidAngle);
				Li += eqr.Le * its.shape->getBSDF()->eval(bsr) * weight / sqr.pdf; 
			}
		}
	}

	return Li;
}

std::string DirectIntegrator::toString() const {
	return tfm::format(
		"SimpleIntegrator[]"
	);
}

NORI_REGISTER_CLASS(DirectIntegrator, "direct");
NORI_NAMESPACE_END
