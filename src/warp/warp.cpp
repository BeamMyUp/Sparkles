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

#include <nori/warp/warp.h>
#include <nori/core/vector.h>
#include <nori/core/frame.h>

NORI_NAMESPACE_BEGIN

/*------------------------------*/
/* Static variables definitions */
/*------------------------------*/

const std::string Warp::s_uniformSquare = "uniform-square";
const std::string Warp::s_tent = "tent";
const std::string Warp::s_uniformDisk = "uniform-disk";
const std::string Warp::s_concentricDisk = "concentric-disk";
const std::string Warp::s_uniformSphere = "uniform-sphere";
const std::string Warp::s_uniformHemisphere = "uniform-hemisphere";
const std::string Warp::s_cosineHemisphere = "cosine-hemisphere";
const std::string Warp::s_beckmann = "beckmann";
const std::string Warp::s_uniformCone = "uniform-cone";

/*-------------------*/
/* Warping functions */
/*-------------------*/

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::squareToTent(const Point2f &sample) {
    throw NoriException("Warp::squareToTent() is not yet implemented!");
}

float Warp::squareToTentPdf(const Point2f &p) {
    throw NoriException("Warp::squareToTentPdf() is not yet implemented!");
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    throw NoriException("Warp::squareToUniformDisk() is not yet implemented!");
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    throw NoriException("Warp::squareToUniformDiskPdf() is not yet implemented!");
}

Point2f Warp::squareToConcentricDisk(const Point2f &sample) {
	// Map uniform random numbers to [-1,1]^2
	Point2f uOffset = 2.f * sample - Vector2f(1, 1);

	// Handle degeneracy at the origin
	if (uOffset.x() == 0 && uOffset.y() == 0)
		return Point2f(0, 0);

	// Apply concentric mapping to point
	float theta, r;
	if (std::abs(uOffset.x()) > std::abs(uOffset.y())) {
		r = uOffset.x();
		theta = M_PI/4.f * (uOffset.y() / uOffset.x());
	}
	else {
		r = uOffset.y();
		theta = M_PI/2.f - M_PI/4.f * (uOffset.x() / uOffset.y());
	}

	return r * Point2f(std::cos(theta), std::sin(theta));
}

float Warp::squareToConcentricDiskPdf(const Point2f &p) {
	return p.norm() > 1.f ? 0.f : INV_PI;
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
	float z = 1.0f - 2.0f * sample.x();
	float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
	float phi = 2.0f * M_PI * sample.y();
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
	return v.norm() > 1.f ? 0.0f : INV_FOURPI; 
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
	float z = sample.x();
	float r = std::sqrt(std::max(0.f, 1.f - z *z));
	float phi = 2 * M_PI * sample.y();

	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
	return v.z() < 0.f ? 0.f : INV_TWOPI;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
	Point2f d = squareToConcentricDisk(sample);
	float z = std::sqrt(std::max(0.f, 1.f - d.x()*d.x() - d.y()*d.y()));

	return Vector3f(d.x(), d.y(), z);
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
	return v.z() < 0.f ? 0.f : v.z() * INV_PI;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    throw NoriException("Warp::squareToBeckmann() is not yet implemented!");
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    throw NoriException("Warp::squareToBeckmannPdf() is not yet implemented!");
}


Vector3f Warp::squareToUniformCone(const Point2f &sample, float cosThetaMax) {
	throw NoriException("Warp::squareToUniformCone() is not yet implemented!");
}

float Warp::squareToUniformConePdf(float cosThetaMax) {
	throw NoriException("Warp::squareToUniformConePdf() is not yet implemented!");
}

/*-----------------*/
/* Query functions */
/*-----------------*/

void Warp::warp(WarpQueryRecord& outWQR, const EWarpType warpFunction, const Point2f& sample, float param /* = 0 */) {

	switch (warpFunction) {
	case EWarpType::EUniformSquare: {
		Point2f p = squareToUniformSquare(sample);
		outWQR.warpedPoint = Vector3f(p.x(), p.y(), 0);
		outWQR.pdf = squareToUniformSquarePdf(sample);
		break;
	}
	case EWarpType::ETent: {
		Point2f p = squareToTent(sample);
		outWQR.warpedPoint = Vector3f(p.x(), p.y(), 0);
		outWQR.pdf = squareToTentPdf(p);
		break;
	}
	case EWarpType::EUniformDisk: {
		Point2f p = squareToUniformDisk(sample);
		outWQR.warpedPoint = Vector3f(p.x(), p.y(), 0);
		outWQR.pdf = squareToUniformDiskPdf(p);
		break;
	}
	case EWarpType::EConcentricDisk: {
		Point2f p = squareToConcentricDisk(sample);
		outWQR.warpedPoint = Vector3f(p.x(), p.y(), 0);
		outWQR.pdf = squareToConcentricDiskPdf(p); 
		break;
	}
	case EWarpType::EUniformSphere:
		outWQR.warpedPoint = squareToUniformSphere(sample);
		outWQR.pdf = squareToUniformSpherePdf(outWQR.warpedPoint);
		break;
	case EWarpType::EUniformHemisphere:
		outWQR.warpedPoint = squareToUniformHemisphere(sample);
		outWQR.pdf = squareToUniformHemispherePdf(outWQR.warpedPoint);
		break;
	case EWarpType::ECosineHemisphere:
		outWQR.warpedPoint = squareToCosineHemisphere(sample);
		outWQR.pdf = squareToCosineHemispherePdf(outWQR.warpedPoint);
		break;
	case EWarpType::EBeckmann:
		outWQR.warpedPoint = squareToBeckmann(sample, param);
		outWQR.pdf = squareToBeckmannPdf(outWQR.warpedPoint, param);
		break;
	case EWarpType::EUniformCone:
		outWQR.warpedPoint = squareToUniformCone(sample, param);
		outWQR.pdf = squareToUniformConePdf(param);
		break;
	}
}

float Warp::pdf(const EWarpType warpFunction, const Vector3f& point, float param/* = 0*/) {

	switch (warpFunction) {
	case EWarpType::EUniformSquare:
		return squareToUniformSquarePdf(Point2f(point.x(), point.y()));
	case EWarpType::ETent:
		return squareToTentPdf(Point2f(point.x(), point.y()));
	case EWarpType::EUniformDisk:
		return squareToUniformDiskPdf(Point2f(point.x(), point.y()));
	case EWarpType::EConcentricDisk:
		return squareToConcentricDiskPdf(Point2f(point.x(), point.y())); 
	case EWarpType::EUniformSphere:
		return squareToUniformSpherePdf(point);
	case EWarpType::EUniformHemisphere:
		return squareToUniformHemispherePdf(point);
	case EWarpType::ECosineHemisphere:
		return squareToCosineHemispherePdf(point);
	case EWarpType::EBeckmann:
		return squareToBeckmannPdf(point, param);
	case EWarpType::EUniformCone:
		return squareToUniformConePdf(param);
	}
}

Warp::EWarpType Warp::getWarpType(const std::string& warpType) {
	if (warpType == "")
		return EWarpType::ENone;
	else if (warpType == s_uniformSquare)
		return EWarpType::EUniformSquare;
	else if (warpType == s_tent)
		return EWarpType::ETent;
	else if (warpType == s_uniformDisk)
		return EWarpType::EUniformDisk;
	else if (warpType == s_concentricDisk)
		return EWarpType::EConcentricDisk;
	else if (warpType == s_uniformSphere)
		return EWarpType::EUniformSphere;
	else if (warpType == s_uniformHemisphere)
		return EWarpType::EUniformHemisphere;
	else if (warpType == s_cosineHemisphere)
		return EWarpType::ECosineHemisphere;
	else if (warpType == s_beckmann)
		return EWarpType::EBeckmann;
	else if (warpType == s_uniformCone)
		return EWarpType::EUniformCone;
	else
		throw NoriException("Warp type requested unknown");
}

Warp::EWarpType Warp::getWarpType(const EMeasure measure, const std::string& warpType/* = ""*/) {
	EWarpType result = EWarpType::ENone;

	switch (measure)
	{
	case EMeasure::EUnknownMeasure:
		result = getWarpType(warpType); 
	case EMeasure::ESolidAngle:
		result = EWarpType::EUniformCone;
	case EMeasure::EArea:
		result = EWarpType::EUniformSphere;
	case EMeasure::EHemisphere:
		if (warpType == s_cosineHemisphere)
			result = EWarpType::ECosineHemisphere;
		else if (warpType == s_uniformHemisphere)
			result = EWarpType::EUniformHemisphere;
	}

	return result; 
}


NORI_NAMESPACE_END
