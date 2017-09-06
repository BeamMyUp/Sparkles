#include <nori/emitters/area.h>

NORI_NAMESPACE_BEGIN

AreaLight::AreaLight(const PropertyList& propList)
	: Emitter(propList){

}

Color3f AreaLight::eval(EmitterQueryRecord& eqr, const Point3f& p) const {
	// ECSE689: Implement Area Light's eval function and fill the EmitterQueryRecord

	return Color3f(0.f);
}

void AreaLight::sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample) const {
	m_shape->sample(sqr, measure, sample); 
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