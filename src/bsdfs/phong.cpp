#include <nori/bsdfs/phong.h>
#include <nori/core/frame.h>

NORI_NAMESPACE_BEGIN

Phong::Phong(const PropertyList& propList)
	: m_kd(propList.getColor("diffuse-coefficients", Color3f()))
	, m_ks(propList.getColor("specular-coefficients", Color3f(1.f)))
	, m_shininess(propList.getInteger("shininess", 1))
{

}

Color3f Phong::sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
	// ECSE689: Implement Phong sampling (sample both specular and diffuse)

	return Color3f(0.f);
}

Color3f Phong::eval(const BSDFQueryRecord &bRec) const {
	// ECSE689: Implement Phong evaluation

	return Color3f(0.f);
}

float Phong::pdf(const BSDFQueryRecord &bRec) const {
	// ECSE689: Implement Phong pdf

	return 0;
}

NORI_NAMESPACE_END