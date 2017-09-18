#include <nori/emitters/point.h>

NORI_NAMESPACE_BEGIN

PointLight::PointLight(const PropertyList& propList)
	: Emitter(propList)
	, m_position(propList.getPoint("position", Point3f())) {

}

void PointLight::eval(EmitterQueryRecord& outERec, const Point3f& p, const Point3f* const samplePoint /*= nullptr*/) const {
	// ECSE689: Implement Point Light's eval function and fill the EmitterQueryRecord
}

void PointLight::sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample, const Point3f* const x /*= nullptr*/) const {
	sqr.sample.p = m_position; 
}

float PointLight::pdf(EMeasure measure, const Point3f& sample, const Point3f* const x /*= nullptr*/) const {
	throw NoriException("PointLight::pdf is not yet implemented");
	return 0.0f;
}

std::string PointLight::toString() const {
	return tfm::format(
		"PointLight[\n"
		"  position = (%f, %f, %f) \n"
		"  radiance = (%f, %f, %f) \n"
		"]",
		m_position.x(), m_position.y(), m_position.z(),
		m_radiance.x(), m_radiance.y(), m_radiance.z()
	);
}

NORI_REGISTER_CLASS(PointLight, "point");
NORI_NAMESPACE_END