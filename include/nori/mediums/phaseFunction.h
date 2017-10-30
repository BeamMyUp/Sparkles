#pragma once

#include<nori/core/object.h>

NORI_NAMESPACE_BEGIN

class PhaseFunction : public NoriObject{
public:
	virtual float eval(const Vector3f& wo, const Vector3f& wi) const; 

	virtual float sample(SampleQueryRecord &sRec, Sampler* sampler) const = 0; 

	virtual float pdf() const = 0;

protected:
	PhaseFunction(const PropertyList &list);

};

NORI_NAMESPACE_END