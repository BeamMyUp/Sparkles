#include <nori/warp/mis.h>

NORI_NAMESPACE_BEGIN

/*------------------------------*/
/* Static variables definitions */
/*------------------------------*/

const std::string MIS::s_balance = "balance";
const std::string MIS::s_power = "power";


/*---------------*/
/* MIS functions */
/*---------------*/

float MIS::balanceHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2) {
	// ECSE689: Implement balance Heuristic
	throw NoriException("Not implemented yet");
	return 0;
}

float MIS::powerHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2, uint32_t beta /*= 2*/) {
	// ECSE689: You don't have to implement this heuristic, but you can if you'd like
	throw NoriException("Not implemented yet");
	return 0;
}

/*-----------------*/
/* Query functions */
/*-----------------*/

float MIS::mis(EHeuristic type, float pdf1, float pdf2, uint32_t n1, uint32_t n2, float param /*= 0*/) {
	switch (type)
	{
	case nori::MIS::EHeuristic::EBalance:
		return balanceHeuristic(pdf1, pdf2, n1, n2);
	case nori::MIS::EHeuristic::EPower:
		return powerHeuristic(pdf1, pdf2, n1, n2, param);
	default:
		throw NoriException("MIS::heuristic: The requested heuristic is not handled");
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