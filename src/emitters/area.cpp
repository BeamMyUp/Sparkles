#include <nori/emitters/area.h>

NORI_NAMESPACE_BEGIN

AreaLight::AreaLight(const PropertyList& propList)
	: Emitter(propList){

}

void AreaLight::eval(EmitterQueryRecord& outERec, const Point3f& p, const Point3f* const ref /*= nullptr*/) const {
	if(!ref)
		throw NoriException("Can't evaluate areaLight without reference");

	outERec.wi = (*ref - p).normalized();
	outERec.Le = m_radiance;
}

void AreaLight::sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample, const Point3f* x /*= nullptr*/) const {
	m_shape->sample(sqr, measure, sample, x); 
}

float AreaLight::pdf(EMeasure measure, const Point3f& sample, const Point3f* const x /* = nullptr*/) const {
	return m_shape->pdf(measure, sample, x);
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