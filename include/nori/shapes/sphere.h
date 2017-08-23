#pragma once

#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

class Sphere : public Shape {
public:

	// Calculate/Update Axis Aligned Bounding Box
	void calculateBoundingBox() override;

	//// Return Centroid of the Shape
	virtual Point3f getCentroid() const override { return m_center; }

	// Return whether a ray intersects with the sphere or not
	bool rayIntersect(const Ray3f &ray_, Intersection &its, bool shadowRay) const override;

	/// Return a human-readable summary of this instance
	std::string toString() const override;

	/// Create a default unit sphere
	Sphere(const PropertyList &propList);

protected:
	float       m_radius; 
	Vector3f    m_center; 
};

NORI_NAMESPACE_END
