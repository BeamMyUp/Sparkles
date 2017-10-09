#include <nori/bsdfs/phong.h>
#include <nori/core/frame.h>

NORI_NAMESPACE_BEGIN

Phong::Phong(const PropertyList& propList)
	: m_kd(propList.getColor("diffuse-coefficients", Color3f()))
	, m_ks(propList.getColor("specular-coefficients", Color3f(1.f)))
	, m_exponent(propList.getInteger("exponent", 1))
{
	// Ensure Energy conservation
	Color3f sumK = m_kd + m_ks; 
	float maxVal = std::max({sumK.x(), sumK.y(), sumK.z()});
	m_kd /= maxVal;
	m_ks /= maxVal;
}

Color3f Phong::sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
	// ECSE689: Implement Phong sampling (sample both specular and diffuse)

	return Color3f(0.f);
}

Vector3f Phong::reflect(const Vector3f &wo) const {
	return Vector3f(-wo.x(), -wo.y(), wo.z());
}

Color3f Phong::eval(const BSDFQueryRecord &bRec) const {
	Color3f f(0.f);

	if (bRec.measure != ESolidAngle
		|| Frame::cosTheta(bRec.wi) <= 0
		|| Frame::cosTheta(bRec.wo) <= 0) {
		return f;
	}

	float cosAlpha = bRec.wi.dot(reflect(bRec.wo));
	if(cosAlpha > 0.0f)
		f += m_ks * (m_exponent + 2.f)  * INV_TWOPI * std::pow(cosAlpha, m_exponent);

	f += m_kd * INV_PI;

	return f;
}

float Phong::pdf(const BSDFQueryRecord &bRec) const {
	// ECSE689: Implement Phong pdf

	return 0;
}


std::string Phong::toString() const {
	return "Phong[]";
}

NORI_REGISTER_CLASS(Phong, "phong");
NORI_NAMESPACE_END