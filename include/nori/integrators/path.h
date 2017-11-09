#pragma once

#include <nori/integrators/integrator.h>
#include <nori/warp/warp.h>
#include <functional>

NORI_NAMESPACE_BEGIN

class PathIntegrator : public Integrator {
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

	// Implicit Path tracing
	virtual Color3f implicitLi(const Scene* scene, Sampler* sampler, const Ray3f &ray) const;

	// Explicit Path Tracing
	virtual Color3f explicitLi(const Scene* scene, Sampler* sampler, const Ray3f &ray) const;

	// Implements stopping condition (Russian Roulette or max Depth)
	bool stopPath(uint32_t currentDepth) const;

	/// Return a brief string summary of the instance (for debugging purpose)
	std::string toString() const override;

	PathIntegrator(const PropertyList &props);

protected:
	enum class Termination {
		EMaxDepth,
		ERussianRoulette,
	};

	float m_terminationParam;	//> maximum number of bounces in the scene if using maximum depth termination or Russian Roulette probability
	Termination m_termination;	//> how the path tracing will terminate
	EMeasure m_directMeasure;	//> measure used for Direct Illumination (explicit)
	EMeasure m_indirectMeasure; //> measure used of Indirect Illumination
	Warp::EWarpType m_directWarpType;	//> warp type used for Direct Illumination (explicit)
	Warp::EWarpType m_indirectWarpType;	//> warp type used for indirect illumination
	uint32_t m_nSamples;		//> number of samples 
	std::function<Color3f(const PathIntegrator* const, const Scene*, Sampler*, const Ray3f&)> m_Li; //> explicit or implicit Lis
};

NORI_NAMESPACE_END
