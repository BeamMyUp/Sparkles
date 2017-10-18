#include <nori/warp/mis.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/warp/warp.h>
#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

/*------------------------------*/
/* Static variables definitions */
/*------------------------------*/

const std::string MIS::s_balance = "balance";
const std::string MIS::s_power = "power";


/*---------------*/
/* MIS functions */
/*---------------*/

float MIS::balanceHeuristic(float pdf1, float pdf2) const {
	return (m_n1 * pdf1) / (m_n1 * pdf1 + m_n2 * pdf2); 
}

float MIS::powerHeuristic(float pdf1, float pdf2, uint32_t beta /*= 2*/) const {
	float f = m_n1 * pdf1;
	float g = m_n2 * pdf2; 
	float fpow = std::pow(f, beta);
	float gpow = std::pow(g, beta);
	
	return fpow / (fpow + gpow); 
}

/*-----------------*/
/* Query functions */
/*-----------------*/

float MIS::eval(float pdf1, float pdf2, float param /*= 0*/) const{
	switch (m_heuristic)
	{
	case nori::MIS::EHeuristic::EBalance:
		return balanceHeuristic(pdf1, pdf2);
	case nori::MIS::EHeuristic::EPower:
		return powerHeuristic(pdf1, pdf2, param);
	default:
		throw NoriException("MIS::heuristic: The requested heuristic is not handled");
		break;
	}
}

float MIS::getPdf(Vector3f sample, Intersection its, Vector3f wo, Emitter* emitter, bool isFirst) const {
	
	EMeasure mesUsed = m_measure1;
	Warp::EWarpType warpUsed = m_warpType1;

	if (!isFirst) {
		mesUsed = m_measure2;
		warpUsed = m_warpType2;
	}

	/* Needs : 
		- Sampled point or vector (Solid Angle || BSDF || SolidAngle || Area || Hemisphere)
		- Intersection (Solid Angle || BSDF) 
		- wo (BSDF)
		- Emitter (SolidAngle || Area)
		- 
	*/
	float pdf = 0;
	switch (mesUsed){
	case nori::EMeasure::EUnknownMeasure:
		throw NoriException("MIS::getPdf() not implemented for EMeasure::EUnkownMeasure");
		break;
	case nori::EMeasure::EDiscrete:
		throw NoriException("MIS::getPdf() not implemented for EMeasure::EDiscrete");
		break;
	case nori::EMeasure::ESolidAngle:
	case nori::EMeasure::EArea:
		pdf = emitter->pdf(mesUsed, sample, &its.p);
		break;
	case nori::EMeasure::EHemisphere:
		pdf = Warp::pdf(warpUsed, sample); 
		break;
	case nori::EMeasure::EBSDF: {
		BSDFQueryRecord bRec(sample, wo, EMeasure::ESolidAngle);
		pdf = its.shape->getBSDF()->pdf(bRec);
		break;
	}
	default:
		throw NoriException("MIS::getPdf():: The measure given is invalid");
		break;
	}
}

MIS::EHeuristic MIS::getHeuristic(const std::string& heuristic) {

	if (heuristic == s_balance)
		return EHeuristic::EBalance;
	else if (heuristic == s_power)
		return EHeuristic::EPower;
	else {
		return EHeuristic::ENone;
		throw NoriException("Heuristic requested unknown");
	}
}

NORI_NAMESPACE_END