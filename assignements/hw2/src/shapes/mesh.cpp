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

#include <nori/shapes/mesh.h>
#include <nori/emitters/emitter.h>
#include <nori/core/bbox.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/warp/warp.h>
#include <Eigen/Geometry>

NORI_NAMESPACE_BEGIN

Mesh::Mesh() { }

float Mesh::surfaceArea(uint32_t index) const {
	uint32_t i0 = m_F(0, index), i1 = m_F(1, index), i2 = m_F(2, index);

	const Point3f p0 = m_V.col(i0), p1 = m_V.col(i1), p2 = m_V.col(i2);

	return 0.5f * Vector3f((p1 - p0).cross(p2 - p0)).norm();
}

bool Mesh::rayIntersect(const Ray3f &ray, float &outT, IntersectionQueryRecord* IQR /*= nullptr*/) const{
	if (!IQR)
		throw NoriException("No IntersectionQueryRecord found");
	
	MeshIntersectionQueryRecord* miqr = static_cast<MeshIntersectionQueryRecord*>(IQR);

	float u, v, t;
	if (rayIntersect(miqr->idx, ray, u, v, t)) {
		outT = t;
		miqr->uv = Point2f(u, v);
		return true;
	}

	return false;
}

bool Mesh::rayIntersect(uint32_t index, const Ray3f &ray, float &u, float &v, float &t) const {
	uint32_t i0 = m_F(0, index), i1 = m_F(1, index), i2 = m_F(2, index);
	const Point3f p0 = m_V.col(i0), p1 = m_V.col(i1), p2 = m_V.col(i2);

	/* Find vectors for two edges sharing v[0] */
	Vector3f edge1 = p1 - p0, edge2 = p2 - p0;

	/* Begin calculating determinant - also used to calculate U parameter */
	Vector3f pvec = ray.d.cross(edge2);

	/* If determinant is near zero, ray lies in plane of triangle */
	float det = edge1.dot(pvec);

	if (det > -1e-8f && det < 1e-8f)
		return false;
	float inv_det = 1.0f / det;

	/* Calculate distance from v[0] to ray origin */
	Vector3f tvec = ray.o - p0;

	/* Calculate U parameter and test bounds */
	u = tvec.dot(pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return false;

	/* Prepare to test V parameter */
	Vector3f qvec = tvec.cross(edge1);

	/* Calculate V parameter and test bounds */
	v = ray.d.dot(qvec) * inv_det;
	if (v < 0.0 || u + v > 1.0)
		return false;

	/* Ray intersects triangle -> compute t */
	t = edge2.dot(qvec) * inv_det;

	return t >= ray.mint && t <= ray.maxt;
}

void Mesh::updateIntersection(const Ray3f &ray, Intersection &its, const IntersectionQueryRecord* IQR /*= nullptr*/) const {
	const MeshIntersectionQueryRecord* miqr = static_cast<const MeshIntersectionQueryRecord*>(IQR);
	uint32_t f = miqr->f;

	its.t = ray.maxt;
	its.uv = miqr->uv; 
	
	/* At this point, we now know that there is an intersection,
	and we know the triangle index of the closest such intersection.

	The following computes a number of additional properties which
	characterize the intersection (normals, texture coordinates, etc..)
	*/

	/* Find the barycentric coordinates */
	Vector3f bary;
	bary << 1 - its.uv.sum(), its.uv;

	/* Vertex indices of the triangle */
	uint32_t idx0 = m_F(0, f), idx1 = m_F(1, f), idx2 = m_F(2, f);

	Point3f p0 = m_V.col(idx0), p1 = m_V.col(idx1), p2 = m_V.col(idx2);

	/* Compute the intersection positon accurately
	using barycentric coordinates */
	its.p = bary.x() * p0 + bary.y() * p1 + bary.z() * p2;

	/* Compute proper texture coordinates if provided by the mesh */
	if (m_UV.size() > 0)
		its.uv = bary.x() * m_UV.col(idx0) +
		bary.y() * m_UV.col(idx1) +
		bary.z() * m_UV.col(idx2);

	/* Compute the geometry frame */
	its.geoFrame = Frame((p1 - p0).cross(p2 - p0).normalized());

	if (m_N.size() > 0) {
		/* Compute the shading frame. Note that for simplicity,
		the current implementation doesn't attempt to provide
		tangents that are continuous across the surface. That
		means that this code will need to be modified to be able
		use anisotropic BRDFs, which need tangent continuity */

		its.shFrame = Frame(
			(bary.x() * m_N.col(idx0) +
				bary.y() * m_N.col(idx1) +
				bary.z() * m_N.col(idx2)).normalized());
	}
	else {
		its.shFrame = its.geoFrame;
	}
}

void Mesh::sampleArea(SampleQueryRecord& outSQR, Point2f &sample) const {
	throw NoriException("Mesh::sampleArea is not yet implemented");
}

void Mesh::sampleSolidAngle(SampleQueryRecord& outSQR, Point2f &sample) const {
	throw NoriException("Mesh::sampleSolidAngle is not yet implemented");
}

BoundingBox3f Mesh::getBoundingBox(uint32_t index) const {
	BoundingBox3f result(m_V.col(m_F(0, index)));
	result.expandBy(m_V.col(m_F(1, index)));
	result.expandBy(m_V.col(m_F(2, index)));
	return result;
}

Point3f Mesh::getCentroid(uint32_t index) const {
	return (1.0f / 3.0f) *
		(m_V.col(m_F(0, index)) +
			m_V.col(m_F(1, index)) +
			m_V.col(m_F(2, index)));
}

std::string Mesh::toString() const {
	return tfm::format(
		"Mesh[\n"
		"  name = \"%s\",\n"
		"  vertexCount = %i,\n"
		"  triangleCount = %i,\n"
		"  bsdf = %s,\n"
		"  emitter = %s\n"
		"]",
		m_name,
		m_V.cols(),
		m_F.cols(),
		m_bsdf ? indent(m_bsdf->toString()) : std::string("null"),
		m_emitter ? indent(m_emitter->toString()) : std::string("null")
	);
}

NORI_NAMESPACE_END