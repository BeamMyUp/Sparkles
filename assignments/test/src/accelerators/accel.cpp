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

#include <nori/accelerators/accel.h>
#include <nori/Shapes/Shape.h>
#include <nori/shapes/mesh.h>

#include <Eigen/Geometry>

NORI_NAMESPACE_BEGIN

void Accel::addShape(Shape* shape) {
	m_shapes.push_back(shape);
	m_bbox = BoundingBox3f::merge(shape->getBoundingBox(), m_bbox);
}

void Accel::build() {
	/* Nothing to do here for now */
}

bool Accel::rayIntersect(const Ray3f &ray_, Intersection &its, bool shadowRay) const {
	bool hit = false;

	Ray3f ray(ray_); 
	float t = std::numeric_limits<float>::infinity(); 

	for (int i = 0; i < m_shapes.size(); ++i) {
		uint32_t nPrimitives = 1; 
		if (m_shapes[i]->isMesh())
			nPrimitives = static_cast<Mesh*>(m_shapes[i])->getTriangleCount(); 
		
		MeshIntersectionQueryRecord miqr;

		for (int j = 0; j < nPrimitives; ++j) {
			
			miqr.idx = j;

			if (m_shapes[i]->rayIntersect(ray, t, &miqr)) {
				if (shadowRay)
					return true;

				ray.maxt = t; 
				hit = true; 
				miqr.f = miqr.idx; 
				its.shape = m_shapes[i]; 
			}
		}

		if (hit)
			its.shape->updateIntersection(ray, its, &miqr);
	}

	return hit;
}

NORI_NAMESPACE_END

