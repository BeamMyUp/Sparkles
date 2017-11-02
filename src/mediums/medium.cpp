#include <nori/mediums/medium.h>
#include <nori/phases/phaseFunction.h>

NORI_NAMESPACE_BEGIN

Medium::Medium()
	: m_phaseFunction(nullptr){}

void Medium::addChild(NoriObject *child) {
	switch (child->getClassType()) {
	case EClassType::EPhaseFunction:
		if (m_phaseFunction)
			throw NoriException(
				"Medium: tried to register multiple phase instances!");
		m_phaseFunction = static_cast<PhaseFunction *>(child);
		break;

	default:
		throw NoriException("Shape::addChild(<%s>) is not supported!",
			classTypeName(child->getClassType()));
	}
};

NORI_NAMESPACE_END