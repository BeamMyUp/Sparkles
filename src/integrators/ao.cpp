#include <nori/integrators/ao.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

AmbientOcclusion::AmbientOcclusion(const PropertyList &props) 
	: m_nSamples(props.getInteger("nSamples", 1)){
	std::string samplingMethod = props.getString("warp-type", ""); 

	m_warpType = Warp::getWarpType(EHemisphere, samplingMethod); 
}

Color3f AmbientOcclusion::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	Color3f Li(0.f);

	// Query if the ray intersects an object
	Intersection its;
	if (!scene->rayIntersect(ray, its))
		return Li;

	// Find the corresponding normal and BSDF where the ray intersects
	Vector3f n = its.shFrame.n;
	const BSDF* bsdf = its.shape->getBSDF();

	for (int i = 0; i < m_nSamples; ++ i) {
		// Get a sample and determine wi
		Warp::WarpQueryRecord wqr; 
		Warp::warp(wqr, m_warpType, sampler->next2D()); 

		Vector3f wi = its.toWorld(wqr.warpedPoint).normalized(); 

		// Check for point's visibility
		float maxt = scene->getBoundingBox().getExtents().norm();
		Ray3f visibilityRay(its.p, wi, Epsilon, maxt);
		float weight = scene->rayIntersect(visibilityRay) ? 0.f: 1.f;

		if(m_warpType == Warp::EWarpType::EUniformHemisphere)
			weight *= zeroClamp(n.dot(wi.normalized()));

		Li += weight;
	}

	// rho/(pi*pdf*N)
	float mult = 1.f / static_cast<float>(m_nSamples);
	if (m_warpType == Warp::EWarpType::EUniformHemisphere)
		mult *= 2.f; 

	return Li * mult;
}

std::string AmbientOcclusion::toString() const {
	return tfm::format(
		"AmbientOcclusionIntegrator[]"
	);
}

NORI_REGISTER_CLASS(AmbientOcclusion, "ao");
NORI_NAMESPACE_END
