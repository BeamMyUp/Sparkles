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

#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

/**
* \brief Structure containing data which must be passed to Intersection Query
*
* This structure contains the information that the rayIntersection function
* needs to optimize the evaluation of the intersection query (avoid testing against
* all triangles, using a proper acceleration structure, and avoid  re-calculating
* texture information at the time of the Intersection update, once the closest
* intersection has been found.
*
*/
struct MeshIntersectionQueryRecord : public IntersectionQueryRecord {
	uint32_t idx; ///< current triangle index tested for intersection
	uint32_t f;   ///< closest triangle index for which an intersection was found
	Point2f uv; 
};

/**
* \brief Triangle mesh
*
* This class stores a triangle mesh object and provides numerous functions
* for querying the individual triangles. Subclasses of \c Mesh implement
* the specifics of how to create its contents (e.g. by loading from an
* external file)
*/
class Mesh : public Shape {
public:

	/// Return the surface area of the given triangle
	float surfaceArea(uint32_t index) const;

	/// Return the bounding box of the full mesh
	void calculateBoundingBox() override { /* TODO: */ }

	/// Return whether a ray intersects with the mesh or not
	bool rayIntersect(const Ray3f &ray_, float &outT, IntersectionQueryRecord* IQR = nullptr) const override;

	/** \brief Ray-triangle intersection test
	*
	* Uses the algorithm by Moeller and Trumbore discussed at
	* <tt>http://www.acm.org/jgt/papers/MollerTrumbore97/code.html</tt>.
	*
	* Note that the test only applies to a single triangle in the mesh.
	* An acceleration data structure like \ref BVH is needed to search
	* for intersections against many triangles.
	*
	* \param index
	*    Index of the triangle that should be intersected
	* \param ray
	*    The ray segment to be used for the intersection query
	* \param t
	*    Upon success, \a t contains the distance from the ray origin to the
	*    intersection point,
	* \param u
	*   Upon success, \c u will contain the 'U' component of the intersection
	*   in barycentric coordinates
	* \param v
	*   Upon success, \c v will contain the 'V' component of the intersection
	*   in barycentric coordinates
	* \return
	*   \c true if an intersection has been detected
	*/
	bool rayIntersect(uint32_t index, const Ray3f &ray, float &u, float &v, float &t) const;

	void updateIntersection(const Ray3f &ray, Intersection &its, const IntersectionQueryRecord* IQR = nullptr) const override;

	/// Returns a sample point using surface area sampling
	virtual void sampleArea(SampleQueryRecord &outSQR, const Point2f &sample) const override;

	/// Returns a sample point using subtended solid angle sampling
	virtual void sampleSolidAngle(SampleQueryRecord &outSQR, const Point2f &sample, const Point3f& x) const override;

	/// Returns a pdf of a 3D point on the shape using surface area sampling
	virtual float pdfArea(const Point3f &sample) const override;

	/// Returns a pdf of a 3D point on the shape using subtended solid angle sampling
	virtual float pdfSolidAngle(const Point3f &sample, const Point3f& x) const override;

	/// Return an axis-aligned bounding box of the entire mesh
	const BoundingBox3f &getBoundingBox() const { return m_bbox; }

	/// Return an axis-aligned bounding box containing the given triangle
	BoundingBox3f getBoundingBox(uint32_t index) const;

	/// Return the centroid of the given triangle
	Point3f getCentroid(uint32_t index) const;

	/// Return a pointer to the vertex positions
	const MatrixXf &getVertexPositions() const { return m_V; }

	/// Return a pointer to the vertex normals (or \c nullptr if there are none)
	const MatrixXf &getVertexNormals() const { return m_N; }

	/// Return a pointer to the texture coordinates (or \c nullptr if there are none)
	const MatrixXf &getVertexTexCoords() const { return m_UV; }

	/// Return a pointer to the triangle vertex index list
	const MatrixXu &getIndices() const { return m_F; }

	/// Return the total number of triangles in this shape
	uint32_t getTriangleCount() const { return (uint32_t)m_F.cols(); }

	/// Return the total number of vertices in this shape
	uint32_t getVertexCount() const { return (uint32_t)m_V.cols(); }

	/// Return a human-readable summary of this instance
	std::string toString() const override;

	/// Return whether the shape is a mesh or not
	virtual bool isMesh() const override { return true; }

protected:
	/// Create an empty mesh
	Mesh();
	Mesh(const PropertyList& propList) : Shape(propList){}

protected:
	std::string m_name;                  ///< Identifying name
	MatrixXf      m_V;                   ///< Vertex positions
	MatrixXf      m_N;                   ///< Vertex normals
	MatrixXf      m_UV;                  ///< Vertex texture coordinates
	MatrixXu      m_F;                   ///< Faces
};

NORI_NAMESPACE_END