#pragma once
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

class Phong : public BSDF{
public:
	Color3f sample(BSDFQueryRecord &bRec, SampleQueryRecord& sRec, const Point2f &sample) const override;
	Color3f eval(const BSDFQueryRecord &bRec) const override;
	float pdf(const BSDFQueryRecord &bRec) const override;
	EBSDFType getBSDFType() const override { return EBSDFType::EPhong; }

	std::string toString() const;
	Color3f getKd() const { return m_kd; }
	Color3f getKs() const { return m_ks; }
	uint32_t exp() const { return m_exponent; }

	Phong(const PropertyList& propList);
private:
	Color3f m_kd; //>> Diffuse reflection constants for each colors
	Color3f m_ks; //>> Specular reflection constants for each colors
	uint32_t m_exponent; //>> Phong's shininess exponent 
	float m_samplingRatio; //>> probability of sampling specular 

	Vector3f reflect(const Vector3f &wo) const;
};

NORI_NAMESPACE_END