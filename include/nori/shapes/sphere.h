#pragma once

#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

class Sphere : public Shape {
public:

	/// Calculate/Update Axis Aligned Bounding Box
	void calculateBoundingBox() override;

	/// Return whether a ray intersects with the sphere or not
	bool rayIntersect(const Ray3f &ray_, float &outT, IntersectionQueryRecord* IQR = nullptr) const override;

	void updateIntersection(const Ray3f &ray, Intersection &its, const IntersectionQueryRecord* IQR = nullptr) const override;

	/// Returns a sample point using surface area sampling
	virtual void sampleArea(SampleQueryRecord& outSQR, const Point2f &sample) const override;

	/// Returns a sample point using subtended solid angle sampling
	virtual void sampleSolidAngle(SampleQueryRecord& outSQR, const Point2f &sample, const Point3f& x) const override;

	/// Returns a pdf of a 3D point on the shape using surface area sampling
	virtual float pdfArea(const Point3f &sample) const override;

	/// Returns a pdf of a 3D point on the shape using subtended solid angle sampling
	virtual float pdfSolidAngle(const Point3f &sample, const Point3f& x) const override;

	/// Return Centroid of the Shape
	virtual Point3f getCentroid() const override { return m_center; }

	virtual void initializeBuffers() override { throw NoriException("Sphere::initializeBuffers not implemented"); }

	/// Return a human-readable summary of this instance
	std::string toString() const override;

	/// Create a default unit sphere
	Sphere(const PropertyList &propList);

protected:
	float       m_radius; 
	Vector3f    m_center; 
	float		m_invSurfaceArea; 
};

NORI_NAMESPACE_END
