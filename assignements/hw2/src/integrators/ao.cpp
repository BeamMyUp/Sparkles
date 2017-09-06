#include <nori/integrators/ao.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

AmbientOcclusion::AmbientOcclusion(const PropertyList &props) 
	: m_nSamples(props.getInteger("nSamples", 1)){
	std::string samplingMethod = props.getString("sampling", ""); 

	m_warpType = Warp::getWarpType(EHemisphere, samplingMethod); 
}

Color3f AmbientOcclusion::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	// ECSE689: Implement a Ambient Occlusion Integrator
	
	return Color3f(0.f);
}

std::string AmbientOcclusion::toString() const {
	return tfm::format(
		"SimpleIntegrator[]"
	);
}

NORI_REGISTER_CLASS(AmbientOcclusion, "ao");
NORI_NAMESPACE_END
