#include <nori/bsdfs/phong.h>
#include <nori/core/frame.h>
#include <nori/warp/warp.h>

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
	bool hasSpec = m_ks.x() > 0 || m_ks.y() > 0 || m_ks.z() > 0;
	bool hasDiff = m_kd.x() > 0 || m_kd.y() > 0 || m_kd.z() > 0;

	// Verifiy that m_ks + m_kd != 0
	if (!hasSpec && !hasDiff)
		return Color3f(0.f); 

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

	if (sampleSpec) {
		float sinAlpha = std::sqrt(1.f - std::pow(1.f - s.x(), 2.f / (m_exponent + 1.f))); 
		float cosAlpha = std::pow(s.x(), 1.f / (m_exponent + 2.f));
		float phi = (2.f * M_PI) * s.y();
		Vector3f localDir = Vector3f(
			sinAlpha * std::cos(phi),
			sinAlpha * std::sin(phi),
			cosAlpha
		);

		// Rotate into correct coordinate system
		Vector3f r = reflect(bRec.wo);
		bRec.wi = Frame(r).toWorld(localDir);
		
		if (Frame::cosTheta(bRec.wi) <= 0)
			return Color3f(0.0f); 
	}
	else {
		Warp::WarpQueryRecord wqr;
		Warp::warp(wqr, Warp::EWarpType::ECosineHemisphere, s); 
		bRec.wi = wqr.warpedPoint; 
	}

	bRec.eta = 1.f;

	float _pdf = pdf(bRec);
	if (_pdf == 0)
		return Color3f(0.0f); 

	return eval(bRec) / _pdf;
}

Vector3f Phong::reflect(const Vector3f &wo) const {
	return Vector3f(-wo.x(), -wo.y(), wo.z());
}

Color3f Phong::eval(const BSDFQueryRecord &bRec) const {
	Color3f f(0.f);

	if (bRec.measure != EMeasure::ESolidAngle
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
	if (bRec.measure != EMeasure::ESolidAngle
		|| Frame::cosTheta(bRec.wi) <= 0
		|| Frame::cosTheta(bRec.wo) <= 0) {
		return 0.f;
	}

	bool hasSpec = m_ks.x() > 0 || m_ks.y() > 0 || m_ks.z() > 0;
	bool hasDiff = m_kd.x() > 0 || m_kd.y() > 0 || m_kd.z() > 0;

	float diffuseProb = 0;
	float specProb = 0;

	if (hasDiff) {
		diffuseProb = Warp::pdf(Warp::EWarpType::ECosineHemisphere, bRec.wi, 0.f);
	}

	if (hasSpec) {
		float alpha = bRec.wi.dot(reflect(bRec.wo));
		
		if (alpha > 0)
			specProb = std::pow(alpha, m_exponent) * (m_exponent + 1.0f) / (2.0f * M_PI); 
	}

	if (hasDiff && hasSpec)
		return m_samplingRatio * specProb + (1 - m_samplingRatio) * diffuseProb;
	else if (hasDiff)
		return diffuseProb;
	else if (hasSpec)
		return specProb;
	else
		return 0.0f;
}


std::string Phong::toString() const {
	return "Phong[]";
}

NORI_REGISTER_CLASS(Phong, "phong");
NORI_NAMESPACE_END