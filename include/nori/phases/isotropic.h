#pragma once

#include <nori/phases/phaseFunction.h>

NORI_NAMESPACE_BEGIN

class IsotropicPhase : public PhaseFunction {
public:
	float eval(const Vector3f& wo, const Vector3f& wi) const override;
	
	float sample(SampleQueryRecord &sRec, Sampler* sampler) const override;
	
	float pdf() const override;

	/// Return a human-readable summary
	std::string toString() const;

	IsotropicPhase(const PropertyList &list);

	int m_testVar; 
};

NORI_NAMESPACE_END