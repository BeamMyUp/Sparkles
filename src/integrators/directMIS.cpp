#include <nori/integrators/directMIS.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>
#include <nori/emitters/emitter.h>
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

DirectMISIntegrator::DirectMISIntegrator(const PropertyList &props){
	uint32_t nSamples1 = props.getInteger("nSamples1", 1);
	uint32_t nSamples2 = props.getInteger("nSamples2", 1);
	std::string heuristic = props.getString("heuristic", ""); 
	EMeasure measure1 = getMeasure(props.getString("measure1", ""));
	EMeasure measure2 = getMeasure(props.getString("measure2", ""));

	assert(m_measure1 != m_measure2 && "DirectMISIntegrator::DirectMISIntegrator : The given measures are the same. Use direct integrator instead");

	Warp::EWarpType warpType1, warpType2;
	if (measure1 == EMeasure::EHemisphere)
		warpType1 = Warp::getWarpType(measure1, props.getString("warp-type1", ""));

	if (measure2 == EMeasure::EHemisphere)
		warpType2 = Warp::getWarpType(measure2, props.getString("warp-type2", ""));

	m_mis = MIS(nSamples1, nSamples2, measure1, measure2, warpType1, warpType2, heuristic);

	m_integrator1 = new DirectIntegrator(measure1, warpType1, &m_mis, true); 
	m_integrator2 = new DirectIntegrator(measure2, warpType2, &m_mis, false);
}

Color3f DirectMISIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {

	Color3f weightedColor1 = m_integrator1->Li(scene, sampler, ray);
	Color3f weightedColor2 = m_integrator2->Li(scene, sampler, ray);

	Color3f res = weightedColor1 + weightedColor2; 

	return res; 
}

std::string DirectMISIntegrator::toString() const {
	return tfm::format(
		"DirectMISIntegrator[]"
	);
}

NORI_REGISTER_CLASS(DirectMISIntegrator, "direct-mis");
NORI_NAMESPACE_END
