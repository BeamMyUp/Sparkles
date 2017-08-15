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

	Intersection aIt;
	bool objHit = false;
	bool hit = false;
	for (int i = 0; i < m_shapes.size(); ++i) {
		objHit = m_shapes[i]->rayIntersect(ray_, aIt, shadowRay);

		if (objHit && shadowRay)
			return true;

		if (objHit && aIt.t < its.t) {
			its = aIt;
			hit = true;
		}
		objHit = false;
	}

	return hit;
}

NORI_NAMESPACE_END

