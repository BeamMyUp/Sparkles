#pragma once

#include <nori/integrators/integrator.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN


class AmbientOcclusion : public Integrator {
public:
	AmbientOcclusion(const PropertyList &props);

	/**
	* \brief Ambient Occlusion Integrator 
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

protected : 
	uint32_t m_nSamples; 
	Warp::EWarpType m_warpType; 
};

NORI_NAMESPACE_END
