#include <nori/integrators/directMIS.h>

NORI_NAMESPACE_BEGIN

DirectMISIntegrator::DirectMISIntegrator(const PropertyList &props)
	: m_nSamples1(props.getInteger("nSamples1", 1))
	, m_nSamples2(props.getInteger("nSamples2", 1)){
	std::string measure1 = props.getString("measure1", "");
	std::string measure2 = props.getString("measure2", "");

	m_measure1 = getMeasure(measure1);
	m_measure2 = getMeasure(measure2);

	assert(m_measure1 != measure2 && "DirectMISIntegrator::DirectMISIntegrator : The given measures are the same. Use direct integrator instead");

	std::string warpType1 = "", warpType2 = "";
	if (m_measure1 == EHemisphere)
		warpType1 = props.getString("warp-type1", "");

	if (m_measure2 == EHemisphere)
		warpType2 = props.getString("warp-type2", "");

	m_warpType1 = Warp::getWarpType(m_measure1, warpType1);
	m_warpType1 = Warp::getWarpType(m_measure2, warpType2);
}

Color3f DirectMISIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	Color3f Li(0.f);

	// ECSE689: Implement MIS integrator (the class is made generic but you can assume only Emitter IS + BSDF IS for the assignment)

	return Li;
}

std::string DirectMISIntegrator::toString() const {
	return tfm::format(
		"DirectMISIntegrator[]"
	);
}

NORI_REGISTER_CLASS(DirectMISIntegrator, "directMIS");
NORI_NAMESPACE_END
