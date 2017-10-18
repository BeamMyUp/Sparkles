#pragma once

#include <nori/integrators/integrator.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN

class MIS; 

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
	DirectIntegrator(EMeasure measure, Warp::EWarpType warpType, const MIS* mis, bool isFirst);

protected:
	// Direct Integrator members
	uint32_t m_nSamples;
	EMeasure m_measure;
	Warp::EWarpType m_warpType;

	// MIS members - used only when directIntegrator is initialized by directMIS.
	const MIS* m_mis; 
	bool m_isFirst; 
};

NORI_NAMESPACE_END