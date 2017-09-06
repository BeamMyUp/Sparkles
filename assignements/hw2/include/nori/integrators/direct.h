#pragma once

#include <nori/integrators/integrator.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN


class DirectIntegrator : public Integrator {
public:
	/**
	* \brief Sample the incident radiance along a ray
	*
	* \param scene
	*    A pointer to the underlying scene
	* \param sampler
	*    A pointer to a sample generator
	* \param ray
	*    The ray in question
	* \return
	*    Estimate of the radiance in the direction given
	*/
	virtual Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override;

	/// Return a brief string summary of the instance (for debugging purpose)
	std::string toString() const override;

	DirectIntegrator(const PropertyList &props);

protected:
	uint32_t m_nSamples;
	EMeasure m_measure;
	Warp::EWarpType m_warpType;
};

NORI_NAMESPACE_END