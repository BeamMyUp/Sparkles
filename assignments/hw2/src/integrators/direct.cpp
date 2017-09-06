#include <nori/integrators/direct.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN

DirectIntegrator::DirectIntegrator(const PropertyList &props)
	: m_nSamples(props.getInteger("nSamples", 1)){
	std::string measure = props.getString("measure", "");

	if (measure == "area")
		m_measure = EArea;
	else if (measure == "solid-angle")
		m_measure = ESolidAngle;
	else
		m_measure = EHemisphere;

	m_warpType = Warp::getWarpType(m_measure);
}

Color3f DirectIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	// ECSE689: Implement a Monte Carlo Direct Integrator
	
	return Color3f(0.f);
}

std::string DirectIntegrator::toString() const {
	return tfm::format(
		"SimpleIntegrator[]"
	);
}

NORI_REGISTER_CLASS(DirectIntegrator, "direct");
NORI_NAMESPACE_END
