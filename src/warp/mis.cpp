#include <nori/warp/mis.h>

NORI_NAMESPACE_BEGIN

float MIS::balanceHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2) {
	return (n1 * pdf1) / (n1 * pdf1 + n2 * pdf2); 
}

float MIS::powerHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2, uint32_t beta /*= 2*/) {
	float f = n1 * pdf1;
	float g = n2 * pdf2; 
	float fpow = std::pow(f, beta);
	float gpow = std::pow(g, beta);
	
	return fpow / (fpow + gpow); 
}

float MIS::heurisitic(EMISType type, float pdf1, float pdf2, uint32_t n1, uint32_t n2, float param /*= 0*/) {
	switch (type)
	{
	case nori::MIS::EMISType::EBalance:
		return balanceHeuristic(pdf1, pdf2, n1, n2);
	case nori::MIS::EMISType::EPower:
		return powerHeuristic(pdf1, pdf2, n1, n2, param);
	default:
		throw NoriException("MIS::heuristic: The requested heuristic is not handled");
		break;
	}
}

NORI_NAMESPACE_END