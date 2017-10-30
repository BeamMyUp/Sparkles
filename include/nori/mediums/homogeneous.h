#pragma once

#include <nori/mediums/medium.h>

NORI_NAMESPACE_BEGIN

class HomogeneousMedium : public Medium {
public:

	virtual Color3f Tr(float t) const override;

	/// Sample an Emitter according to the measure given in argument and eval for the resulting point
	virtual void sample(SampleQueryRecord& sRec, const Ray3f &ray, Sampler& sampler, float& outT) const override;

	/// Returns the pdf of a 3D point on the emitter according to EMeasure
	virtual float pdf(EMeasure measure, const Point3f& sample, const Point3f* const x = nullptr) const override;

	HomogeneousMedium(const PropertyList &props);

	virtual std::string toString() const override;

protected:
	Color3f m_sigmaS; //< Scattering coefficient 
	Color3f m_sigmaA; //< Absorption coefficient
	Color3f m_sigmaT; //< Transmission constant
};

NORI_NAMESPACE_END