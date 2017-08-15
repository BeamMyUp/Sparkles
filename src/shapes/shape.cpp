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

#include <nori/shapes/shape.h>
#include <nori/core/bbox.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/emitters/emitter.h>
#include <nori/warp/warp.h>
#include <Eigen/Geometry>

NORI_NAMESPACE_BEGIN

Shape::Shape() { }

Shape::~Shape() {
	delete m_bsdf;
	delete m_emitter;
}

void Shape::activate() {
	if (!m_bsdf) {
		/* If no material was assigned, instantiate a diffuse BRDF */
		m_bsdf = static_cast<BSDF *>(
			NoriObjectFactory::createInstance("diffuse", PropertyList()));
	}
}

void Shape::addChild(NoriObject *obj) {
	switch (obj->getClassType()) {
	case EBSDF:
		if (m_bsdf)
			throw NoriException(
				"Shape: tried to register multiple BSDF instances!");
		m_bsdf = static_cast<BSDF *>(obj);
		break;

	case EEmitter: {
		Emitter *emitter = static_cast<Emitter *>(obj);
		if (m_emitter)
			throw NoriException(
				"Shape: tried to register multiple Emitter instances!");
		m_emitter = emitter;
	}
				   break;

	default:
		throw NoriException("Shape::addChild(<%s>) is not supported!",
			classTypeName(obj->getClassType()));
	}
}

std::string Shape::toString() const {
	return tfm::format(
		"Shape[\n"
		"  name = \"%s\",\n"
		"  bsdf = %s,\n"
		"  emitter = %s\n"
		"]",
		m_name,
		m_bsdf ? indent(m_bsdf->toString()) : std::string("null"),
		m_emitter ? indent(m_emitter->toString()) : std::string("null")
	);
}

std::string Intersection::toString() const {
	if (!shape)
		return "Intersection[invalid]";

	return tfm::format(
		"Intersection[\n"
		"  p = %s,\n"
		"  t = %f,\n"
		"  uv = %s,\n"
		"  shFrame = %s,\n"
		"  geoFrame = %s,\n"
		"  shape = %s\n"
		"]",
		p.toString(),
		t,
		uv.toString(),
		indent(shFrame.toString()),
		indent(geoFrame.toString()),
		shape ? shape->toString() : std::string("null")
	);
}


NORI_NAMESPACE_END
