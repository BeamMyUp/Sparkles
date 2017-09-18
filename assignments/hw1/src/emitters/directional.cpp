#include <nori/emitters/directional.h>

NORI_NAMESPACE_BEGIN

DirectionalLight::DirectionalLight(const PropertyList& propList)
	: Emitter(propList)
	, m_direction(propList.getVector("direction", Vector3f())) {
	m_direction.normalize(); 
}

void DirectionalLight::eval(EmitterQueryRecord& outERec, const Point3f& p, const Point3f* const samplePoint /*= nullptr*/) const {
	// ECSE689: Implement Directional Light's eval function and fill the EmitterQueryRecord
}

void DirectionalLight::sample(SampleQueryRecord &sqr, EMeasure measure, const Point2f &sample, const Point3f* const x /*= nullptr*/) const {
	sqr.sample.v = m_direction; 
}

float DirectionalLight::pdf(EMeasure measure, const Point3f& sample, const Point3f* const x /* = nullptr*/) const {
	throw NoriException("DirectionalLight::pdf is not yet implemented");
	return 0.0f;
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