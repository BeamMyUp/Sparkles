#include <nori/emitters/directional.h>

NORI_NAMESPACE_BEGIN

DirectionalLight::DirectionalLight(const PropertyList& propList)
	: Emitter(propList)
	, m_direction(propList.getVector("direction", Vector3f())) {
	m_direction.normalize(); 
}

Color3f DirectionalLight::eval(EmitterQueryRecord& eqr, const Point3f& p) const {
	// ECSE689: Implement Directional Light's eval function and fill the EmitterQueryRecord

	return Color3f();
}

std::string DirectionalLight::toString() const {
	return tfm::format(
		"DirectionalLight[\n"
		"  direction = (%f, %f, %f) \n"
		"  radiance = (%f, %f, %f) \n"
		"]",
		m_direction.x(), m_direction.y(), m_direction.z(),
		m_radiance.x(), m_radiance.y(), m_radiance.z()
	);
}

NORI_REGISTER_CLASS(DirectionalLight, "directional");
NORI_NAMESPACE_END