#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

Emitter::Emitter(const PropertyList& propList)
	: m_radiance(propList.getColor("radiance", Color3f())) {

}

std::string Emitter::toString() const {
	return tfm::format(
		"Emitter[\n"
		"  radiance = (%f, %f, %f) \n"
		"]",
		m_radiance.x(), m_radiance.y(), m_radiance.z()
	);
}

NORI_NAMESPACE_END