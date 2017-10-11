#include <nori/bsdfs/phong.h>
#include <nori/core/frame.h>

NORI_NAMESPACE_BEGIN

Phong::Phong(const PropertyList& propList)
	: m_kd(propList.getColor("diffuse-coefficients", Color3f()))
	, m_ks(propList.getColor("specular-coefficients", Color3f(1.f)))
	, m_exponent(propList.getInteger("exponent", 1))
	, m_samplingRatio(0.f)
{
	// Ensure Energy conservation
	Color3f sumK = m_kd + m_ks; 
	float maxVal = std::max({sumK.x(), sumK.y(), sumK.z()});

	if (maxVal != 0) {
		m_kd /= maxVal;
		m_ks /= maxVal;
	}

	// Compute weights that steer samples towards the specular of diffuse components
	float dAvg = m_kd.getLuminance(); 
	float sAvg = m_ks.getLuminance();
	m_samplingRatio = sAvg / (dAvg + sAvg);
}

Color3f Phong::sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
	Color3f f(0.f);

	bool hasSpec = m_ks.x() > 0 || m_ks.y() > 0 || m_ks.z() > 0;
	bool hasDiff = m_kd.x() > 0 || m_kd.y() > 0 || m_kd.z() > 0;

	// Verifiy that m_ks + m_kd != 0
	if (!hasSpec && !hasDiff)
		return f; 

	/* Choose to sample either specular or diffuse based on sample.x()value 
	*  relative to samplingWeigth. Rescale the value so it ends up still covering 
	*  the full 0-1 range no matter which is sampled. 
	*/
	Point2f s(sample);
	bool sampleSpec = hasSpec;

	if (hasDiff && hasSpec) {
		if (s.x() <= m_samplingRatio) {
			s.x() /= m_samplingRatio;
		}else {
			s.x() = (s.x() - m_samplingRatio) / (1.f - m_samplingRatio);
			sampleSpec = false; 
		}

	}

	return f;
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