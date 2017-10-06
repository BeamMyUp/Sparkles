#pragma once
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

class Phong : public BSDF{
		/**
		* \brief Sample the BSDF and return the importance weight (i.e. the
		* value of the BSDF * cos(theta_o) divided by the probability density
		* of the sample with respect to solid angles).
		*
		* \param bRec    A BSDF query record
		* \param sample  A uniformly distributed sample on \f$[0,1]^2\f$
		*
		* \return The BSDF value divided by the probability density of the sample
		*         sample. The returned value also includes the cosine
		*         foreshortening factor associated with the outgoing direction,
		*         when this is appropriate. A zero value means that sampling
		*         failed.
		*/
		virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override;

		/**
		* \brief Evaluate the BSDF for a pair of directions and measure
		* specified in \code bRec
		*
		* \param bRec
		*     A record with detailed information on the BSDF query
		* \return
		*     The BSDF value, evaluated for each color channel
		*/
		virtual Color3f eval(const BSDFQueryRecord &bRec) const override;

		/**
		* \brief Compute the probability of sampling \c bRec.wo
		* (conditioned on \c bRec.wi).
		*
		* This method provides access to the probability density that
		* is realized by the \ref sample() method.
		*
		* \param bRec
		*     A record with detailed information on the BSDF query
		*
		* \return
		*     A probability/density value expressed with respect
		*     to the specified measure
		*/

		virtual float pdf(const BSDFQueryRecord &bRec) const override;

		Phong(const PropertyList& propList);

private:
	Color3f m_kd; //>> Diffuse reflection constants for each colors
	Color3f m_ks; //>> Specular reflection constants for each colors
	uint32_t m_shininess; //>> Phong's shininess exponent 
};

NORI_NAMESPACE_END