#include <nori/emitters/point.h>

NORI_NAMESPACE_BEGIN

PointLight::PointLight(const PropertyList& propList)
	: Emitter(propList)
	, m_position(propList.getPoint("position", Point3f())) {

}

Color3f PointLight::eval(EmitterQueryRecord& eqr, const Point3f& p) const {
	eqr.wi = Vector3f(m_position - p);
	eqr.wi.normalize(); 

	Color3f res = m_radiance / squaredDist(m_position, p);

	return res;
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