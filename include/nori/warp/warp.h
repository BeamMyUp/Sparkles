/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <nori/core/common.h>
#include <nori/samplers/sampler.h>

NORI_NAMESPACE_BEGIN

/// A collection of useful warping functions for importance sampling
class Warp {
public:

	enum class EWarpType {
		ENone = 0x00,
		EUniformSquare,
		ETent,
		EUniformDisk,
		EUniformSphere,
		EUniformHemisphere,
		ECosineHemisphere, 
		EBeckmann,
		EUniformCone,	
		EMicrofacetBRDF,
	};

	struct WarpQueryRecord {
		Vector3f warpedPoint;
		float	 pdf; 
	};

protected:
    /// Dummy warping function: takes uniformly distributed points in a square and just returns them
    static Point2f squareToUniformSquare(const Point2f &sample);

    /// Probability density of \ref squareToUniformSquare()
    static float squareToUniformSquarePdf(const Point2f &p);

    /// Sample a 2D tent distribution
    static Point2f squareToTent(const Point2f &sample);

    /// Probability density of \ref squareToTent()
    static float squareToTentPdf(const Point2f &p);

    /// Uniformly sample a vector on a 2D disk with radius 1, centered around the origin
    static Point2f squareToUniformDisk(const Point2f &sample);

    /// Probability density of \ref squareToUniformDisk()
    static float squareToUniformDiskPdf(const Point2f &p);

    /// Uniformly sample a vector on the unit sphere with respect to solid angles
    static Vector3f squareToUniformSphere(const Point2f &sample);

    /// Probability density of \ref squareToUniformSphere()
    static float squareToUniformSpherePdf(const Vector3f &v);

    /// Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to solid angles
    static Vector3f squareToUniformHemisphere(const Point2f &sample);

    /// Probability density of \ref squareToUniformHemisphere()
    static float squareToUniformHemispherePdf(const Vector3f &v);

    /// Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to projected solid angles
    static Vector3f squareToCosineHemisphere(const Point2f &sample);

    /// Probability density of \ref squareToCosineHemisphere()
    static float squareToCosineHemispherePdf(const Vector3f &v);

    /// Warp a uniformly distributed square sample to a Beckmann distribution * cosine for the given 'alpha' parameter
    static Vector3f squareToBeckmann(const Point2f &sample, float alpha);

    /// Probability density of \ref squareToBeckmann()
    static float squareToBeckmannPdf(const Vector3f &m, float alpha);

	/// Warp a uniformly distributed square sample to a solid angle's cone
	static Vector3f squareToUniformCone(const Point2f &sample, float cosThetaMax);

	/// Probability density of \ref squareToUniformCone
	static float squareToUniformConePdf(float cosThetaMax);

public:
	/// Warp a point according to the EWarp given
	static void warp(WarpQueryRecord& outWqr, const EWarpType warpFunction, const Point2f& sample, float param = 0);

	/// Get strictly the pdf for a given point
	static float pdf(const EWarpType warpFunction, const Vector3f& point, float param = 0); 

	/// Returns the EWarp warp type associated with the given EMeasure and optionally a passed string when a measure can handle more than one warptype
	static EWarpType getWarpType(const EMeasure measure, const std::string& warpType = "");

protected:

	/// Returns the EWarp warp type associated with a given string
	static EWarpType getWarpType(const std::string& warpType); 

	static const std::string s_uniformSquare;
	static const std::string s_tent;
	static const std::string s_uniformDisk;
	static const std::string s_uniformSphere;
	static const std::string s_uniformHemisphere; 
	static const std::string s_cosineHemisphere;
	static const std::string s_beckmann;
	static const std::string s_uniformCone;
};

NORI_NAMESPACE_END
