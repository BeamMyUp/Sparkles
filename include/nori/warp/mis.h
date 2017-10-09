#pragma once

#include <nori/core/common.h>

NORI_NAMESPACE_BEGIN

class MIS {
	enum class EMISType {
		EBalance = 0x00,
		EPower,
	};

protected:
	static float balanceHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2);
	static float powerHeuristic(float pdf1, float pdf2, uint32_t n1, uint32_t n2, uint32_t beta = 2);

public:
	static float heurisitic(EMISType type, float pdf1, float pdf2, uint32_t n1, uint32_t n2, float param = 0); 
};

NORI_NAMESPACE_END