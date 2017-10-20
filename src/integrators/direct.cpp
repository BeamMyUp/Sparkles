#include <nori/integrators/direct.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>
#include <nori/warp/warp.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/warp/mis.h>

NORI_NAMESPACE_BEGIN

DirectIntegrator::DirectIntegrator(const PropertyList &props)
	: m_nSamples(props.getInteger("nSamples", 1))
	, m_mis(nullptr){
	std::string measure = props.getString("measure", "");

	m_measure = getMeasure(measure); 

	std::string warpType = "";
	if (m_measure == EMeasure::EHemisphere)
		warpType = props.getString("warp-type", "");

	m_warpType = Warp::getWarpType(m_measure, warpType);
}

DirectIntegrator::DirectIntegrator(EMeasure measure, Warp::EWarpType warpType, const MIS* mis, bool isFirst)
	: m_measure(measure)
	, m_warpType(warpType)
	, m_mis(mis)
	, m_isFirst(isFirst)
{
	if (isFirst)
		m_nSamples = m_mis->getN1();
	else
		m_nSamples = m_mis->getN2(); 

}

Color3f DirectIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	Color3f Li(0.f);

	// Find if the surface is visible from camera
	Intersection its;
	if (!scene->rayIntersect(ray, its))
		return Color3f(0.f);

	// If the intersection hits the light, return the radiance
	if (its.shape->isEmitter())
		return its.shape->getEmitter()->getRadiance();

	// Get the extents of the scene
	float maxt = scene->getBoundingBox().getExtents().norm();

	for (int i = 0; i < m_nSamples; ++i) {
		for (auto emitter : scene->getEmitters()) {		
			Color3f f(0.f);

			// Sample 
			Point2f sample = sampler->next2D();
			SampleQueryRecord sqr; 

			// If EArea or ESolidAngle, sample the emitter
			if (m_measure == EMeasure::EArea || m_measure == EMeasure::ESolidAngle)
				emitter->sample(sqr, m_measure, sample, &its.p);

			// If EHemisphere, sample on an hemisphere around the point according to warpType
			else if (m_measure == EMeasure::EHemisphere) {
				Warp::WarpQueryRecord wqr;
				Warp::warp(wqr, m_warpType, sample);
				sqr.sample.v = wqr.warpedPoint;
				sqr.pdf = wqr.pdf;
			}

			else if (m_measure == EMeasure::EBSDF) {
				BSDFQueryRecord bRec(Vector3f(0.f), its.toLocal(-ray.d), EMeasure::ESolidAngle);
				f = its.shape->getBSDF()->sample(bRec, sqr, sample);
				sqr.sample.v = its.toWorld(bRec.wi);
			}
			
			// Check Pdf validity
			if (sqr.pdf == 0 /* && !deltaPDF*/) // TODO: code Delta pdf concept (have isDelta in bsdf)
				return Color3f(0.0f); 
	
			// Query Emitter's radiance and wi. If solid-angle, wi is the sample directly
			EmitterQueryRecord eqr;
			emitter->eval(eqr, its.p, &sqr.sample.p); 

			Vector3f wi = sqr.sample.v;
			if(m_measure == EMeasure::EArea) // Update sampled direction if Area Sampling
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
				switch (m_measure)
				{
				case EMeasure::EArea: {
					// Weight by geometry term
					float d2 = (sqr.sample.p - its.p).squaredNorm();
					float cosThetaO = zeroClamp(wi.dot(sqr.n));
					float cosThetaI = zeroClamp(wi.dot(its.shFrame.n));
					weight = (cosThetaI * cosThetaO) / d2;
					break;
				}
				case EMeasure::EHemisphere:
				case EMeasure::ESolidAngle:
				case EMeasure::EBSDF:
					weight = zeroClamp(wi.dot(its.shFrame.n)); 
					break;
				}
				
				// Evaluate rendering equation
				if (m_measure != EMeasure::EBSDF) {
					BSDFQueryRecord bsr(wiLocal, woLocal, EMeasure::ESolidAngle);
					f = its.shape->getBSDF()->eval(bsr);
				}

				// If used for MIS, calculate heuristic
				if (m_mis) {

					float pdf2 = m_mis->getPdf(sqr.sample.v, its, woLocal, emitter, !m_isFirst);
					f *= m_mis->eval(sqr.pdf, pdf2); 
				}

				Li += weight * eqr.Le * f / sqr.pdf;
			}
		}
	}

	return Li / m_nSamples;
}

std::string DirectIntegrator::toString() const {
	return tfm::format(
		"DirectIntegrator[]"
	);
}

NORI_REGISTER_CLASS(DirectIntegrator, "direct");
NORI_NAMESPACE_END
