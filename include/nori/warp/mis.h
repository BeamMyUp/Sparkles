#pragma once

#include <nori/core/common.h>

NORI_NAMESPACE_BEGIN

class MIS {
public:
	enum class EHeuristic {
		ENone = 0x00,
		EBalance,
		EPower,
	};

protected:
	static float balanceHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2);
	static float powerHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2, uint32_t beta = 2);

public:
	static float mis(EHeuristic type, float pdf1, float pdf2, uint32_t n1, uint32_t n2, float param = 0); 
	static EHeuristic getHeuristic(const std::string& heuristic); 

protected : 
	static const std::string s_balance;
	static const std::string s_power;
};

NORI_NAMESPACE_END