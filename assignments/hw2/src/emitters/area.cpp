#include <nori/emitters/area.h>

NORI_NAMESPACE_BEGIN

AreaLight::AreaLight(const PropertyList& propList)
	: Emitter(propList){

}

void AreaLight::eval(EmitterQueryRecord& outERec, const Point3f& p, const Point3f* const ref /*= nullptr*/) const {
	if(!ref)
		throw NoriException("Can't evaluate areaLight without reference");

	// ECSE689: Implement Area Light's eval function and fill the EmitterQueryRecord
}

void AreaLight::sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample, const Point3f* x /*= nullptr*/) const {
	// ECSE689: Implement Area Light's sample function, filling the SampleQueryRecord
}

float AreaLight::pdf(EMeasure measure, const Point3f& sample, const Point3f* const x /* = nullptr*/) const {
	// ECSE689: Implement Area Light's pdf function
	return 0.f; 
}

std::string AreaLight::toString() const {
	return tfm::format(
		"AreaLight[\n"
		"  radiance = (%f, %f, %f) \n"
		"]",
		m_radiance.x(), m_radiance.y(), m_radiance.z()
	);
}

NORI_REGISTER_CLASS(AreaLight, "area");
NORI_NAMESPACE_END