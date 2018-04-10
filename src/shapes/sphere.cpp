#pragma once

#include <nori/bsdfs/bsdf.h>
#include <nori/emitters/emitter.h>
#include <nori/shapes/sphere.h>
#include <nori/core/common.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN

Sphere::Sphere(const PropertyList &propList)
	: Shape(propList)
	, m_center(propList.getPoint("center", Point3f()))
	, m_radius(propList.getFloat("radius", 1.f)){

	m_invSurfaceArea = 1.f / (4.f * M_PI * m_radius * m_radius); 
}

void Sphere::calculateBoundingBox(){
	Vector3f rad = Vector3f(m_radius, m_radius, m_radius);

	Vector3f min = m_center - rad;
	Vector3f max = m_center + rad;

	m_bbox = BoundingBox3f(min, max);
}

bool Sphere::rayIntersect(const Ray3f &ray_, float &outT, IntersectionQueryRecord* IQR /*= nullptr*/) const{
	Ray3f ray = ray_;

	// Calculate quadratic coefficients
	double a = ray.d.dot(ray.d);
	double b = 2.f * (ray.d.dot(ray.o - m_center));
	double c = (ray.o - m_center).dot(ray.o - m_center) - m_radius*m_radius;

	double t0, t1;

	// Check for a quadratic solution. If none, no intersection
	if (!quadratic(a, b, c, t0, t1))
		return false;

	// If the intersection is outside the ray's min and max, no intersection
	if (t0 > ray.maxt || t1 < ray.mint)
		return false;

	// try to get a solution inside the ray's min and max 
	float tHit = (float)t0;
	if (t0 < ray.mint)
	{
		tHit = (float)t1;
		if (tHit > ray.maxt)
			return false;
	}

	// Intersection found
	outT = tHit;
	
	return true;
}

void Sphere::updateIntersection(const Ray3f &ray, Intersection &its, const IntersectionQueryRecord* IQR /*= nullptr*/) const {
	its.t = ray.maxt; 
	Vector3f itsp = ray(its.t);

	its.p = itsp;
	its.shape = this;

	// Calculate normal at the intersection point
	Vector3f normal = (itsp - m_center).normalized();

	its.shFrame = Frame(normal);
	its.geoFrame = Frame(normal);

	// Calculate UVs at the intersection point
	double theta = acos(itsp.z() / m_radius);
	double zRad = std::sqrt(itsp.x()*itsp.x() + itsp.y()*itsp.y());
	double cosPhi = itsp.x() / zRad;
	double phi = acos(cosPhi);

	its.uv = Point2f((float)phi * INV_TWOPI, (float)theta * INV_PI);
}

void Sphere::sampleArea(SampleQueryRecord& outSQR, const Point2f &sample) const {
	// Sample uniform sphere
	Warp::WarpQueryRecord wqr; 
	Warp::warp(wqr, Warp::EWarpType::EUniformSphere, sample);

	// Fill Sample Query Record
	outSQR.sample.p = Point3f(wqr.warpedPoint * m_radius) + m_center; // scale sample to sphere
	outSQR.n = Normal3f(wqr.warpedPoint); 
	outSQR.pdf = pdfArea(outSQR.sample.p);
}

void Sphere::sampleSolidAngle(SampleQueryRecord& outSQR, const Point2f &sample, const Point3f& x) const {
	Vector3f centerToX(m_center - x);

	// Calculate subtended solid angle 
	float sinThetaMax2 = m_radius * m_radius / centerToX.squaredNorm();
	float cosThetaMax = safeSqrt(1.f - sinThetaMax2); 

	// Warping to Uniform cone. Not using Warp to keep cosTheta, sinTheta and Phi and avoid recalculating
	Warp::WarpQueryRecord wqr;
	Warp::warp(wqr, Warp::EWarpType::EUniformCone, sample, cosThetaMax);
	Vector3f d = Frame(centerToX.normalized()).toWorld(wqr.warpedPoint); 

	// Find the point on the sphere
	outSQR.sample.v = d; 
	outSQR.pdf = wqr.pdf; 
}

float Sphere::pdfArea(const Point3f &sample) const {
	return m_invSurfaceArea;
}

float Sphere::pdfSolidAngle(const Point3f &sample, const Point3f& x) const {
	Vector3f centerToX(m_center - x);
	
	// Calculate subtended solid angle 
	float sinThetaMax2 = m_radius * m_radius / centerToX.squaredNorm();
	float cosThetaMax = safeSqrt(1.f - sinThetaMax2);

	return Warp::pdf(Warp::EWarpType::EUniformCone, sample, cosThetaMax);
}

std::string Sphere::toString() const {
	return tfm::format(
		"Sphere[\n"
		"  center = (%f, %f, %f) \n"
		"  radius = %f \n"
		"  bsdf = %s,\n"
		"  emitter = %s\n"
		"]",
		m_center.x(), m_center.y(), m_center.z(),
		m_radius,
		m_bsdf ? indent(m_bsdf->toString()) : std::string("null"),
		m_emitter ? indent(m_emitter->toString()) : std::string("null")
	);
}

NORI_REGISTER_CLASS(Sphere, "sphere");
NORI_NAMESPACE_END