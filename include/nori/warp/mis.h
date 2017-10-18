#pragma once

#include <nori/core/common.h>
#include <nori/warp/warp.h>
#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

class MIS {
public:
	enum class EHeuristic {
		ENone = 0x00,
		EBalance,
		EPower,
	};

protected:
	float balanceHeuristic(float pdf1, float pdf2) const;
	float powerHeuristic(float pdf1, float pdf2, uint32_t beta = 2) const;

public:
	float eval(float pdf1, float pdf2, float param = 0) const; 
	float getPdf(Vector3f sample, Intersection its, Vector3f wo, Emitter* emitter, bool isFirst) const;
	static EHeuristic getHeuristic(const std::string& heuristic);

	int getN1() const { return m_n1; }
	int getN2() const { return m_n2; }
	EMeasure getMeasure1() const { return m_measure1; }
	EMeasure getMeasure2() const { return m_measure2; }

	MIS()
		: m_n1(1), m_n2(1), m_measure1(EMeasure::EUnknownMeasure), m_measure2(EMeasure::EUnknownMeasure)
		, m_warpType1(Warp::EWarpType::ENone), m_warpType2(Warp::EWarpType::ENone), m_heuristic(EHeuristic::ENone) {};

	MIS(uint32_t n1, uint32_t n2, EMeasure m1, EMeasure m2, Warp::EWarpType w1, Warp::EWarpType w2, EHeuristic heuristic)
		: m_n1(n1), m_n2(n2), m_measure1(m1), m_measure2(m2), m_warpType1(w1), m_warpType2(w2), m_heuristic(heuristic) {};

	MIS(uint32_t n1, uint32_t n2, EMeasure m1, EMeasure m2, Warp::EWarpType w1, Warp::EWarpType w2, const std::string& heuristic)
		: MIS(n1, n2, m1, m2, w1, w2, getHeuristic(heuristic)) {};

protected : 
	static const std::string s_balance;
	static const std::string s_power;

	uint32_t m_n1;
	uint32_t m_n2; 
	EMeasure m_measure1;
	EMeasure m_measure2; 
	Warp::EWarpType m_warpType1;
	Warp::EWarpType m_warpType2; 
	EHeuristic m_heuristic; 
};

NORI_NAMESPACE_END