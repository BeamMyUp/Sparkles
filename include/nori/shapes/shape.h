#pragma once

#include <nori/core/object.h>
#include <nori/core/frame.h>
#include <nori/core/bbox.h>

NORI_NAMESPACE_BEGIN

class Shape; 

/**
* \brief Intersection data structure
*
* This data structure records local information about a ray-object intersection.
* This includes the position, traveled ray distance, uv coordinates, as well
* as well as two local coordinate frames (one that corresponds to the true
* geometry, and one that is used for shading computations).
*/
struct Intersection {
	/// Position of the surface intersection
	Point3f p;
	/// Unoccluded distance along the ray
	float t;
	/// UV coordinates, if any
	Point2f uv;
	/// Shading frame (based on the shading normal)
	Frame shFrame;
	/// Geometric frame (based on the true geometry)
	Frame geoFrame;
	/// Pointer to the associated mesh
	const Shape *shape;

	/// Create an uninitialized intersection record
	Intersection() 
		: shape(nullptr)
		, t(std::numeric_limits<float>::infinity())
	{}

	/// Transform a direction vector into the local shading frame
	Vector3f toLocal(const Vector3f &d) const {
		return shFrame.toLocal(d);
	}

	/// Transform a direction vector from local to world coordinates
	Vector3f toWorld(const Vector3f &d) const {
		return shFrame.toWorld(d);
	}

	/// Return a human-readable summary of the intersection record
	std::string toString() const;
};

/**
* \brief Structure containing data which must be passed to Intersection Query
*
* This structure contains the information that the rayIntersection function
* needs to optimize the evaluation of the intersection query in the case
* that the shape is made of many primitives (i.e. mesh) and or to avoid
* re-calculating some information at the time of the Intersection update, once
* the closest intersection has been found.
*
*/
struct IntersectionQueryRecord {
	/// Is empty by default
};

/**
* \brief Scene Object
*
* This class defines an interface for all general objects in the scene
*/
class Shape : public NoriObject {
public:

	/*-------------------------------------------*/
	/* Public Initialization/Destruction methods */
	/*-------------------------------------------*/

	/// Release all memory
	virtual ~Shape();

	/// Initialize internal data structures (called once by the XML parser)
	virtual void activate();

	/*---------------------*/
	/* Calculation methods */
	/*---------------------*/

	/// Calculate/Update the axis-aligned bounding box of the object
	virtual void calculateBoundingBox() = 0; 

	/** \brief Intersection test
	*
	* \param ray
	*    A 3-dimensional ray data structure with minimum/maximum extent
	*    information
	*
	* \param its
	*    A detailed intersection record, which will be filled by the
	*    intersection query
	*
	* \param shadowRay
	*    \c true if this is a shadow ray query, i.e. a query that only aims to
	*    find out whether the ray is blocked or not without returning detailed
	*    intersection information.
	*
	* \return
	*   \c true if an intersection has been detected
	*/
	virtual bool rayIntersect(const Ray3f &ray_, float &outT, IntersectionQueryRecord* IQR = nullptr) const = 0;

	virtual void updateIntersection(const Ray3f &ray, Intersection &its, const IntersectionQueryRecord* IQR = nullptr) const = 0;

	/*------------------*/
	/* Sampling methods */
	/*------------------*/

	/// Returns a sample point on the shape according to EMeasure
	virtual void sample(SampleQueryRecord &outSQR, EMeasure measure, const Point2f &sample, const Point3f* const x = nullptr) const;

	/// Returns a sample point using surface area sampling
	virtual void sampleArea(SampleQueryRecord &outSQR, const Point2f &sample) const = 0;

	/// Returns a sample point using subtended solid angle sampling
	virtual void sampleSolidAngle(SampleQueryRecord &outSQR, const Point2f &sample, const Point3f& x) const = 0;

	/// Returns the pdf of a 3D point on the shape according to EMeasure
	virtual float pdf(EMeasure measure, const Point3f &sample, const Point3f* const x = nullptr) const;

	/// Returns a pdf of a 3D point on the shape using surface area sampling
	virtual float pdfArea(const Point3f &sample) const = 0;

	/// Returns a pdf of a 3D point on the shape using subtended solid angle sampling
	virtual float pdfSolidAngle(const Point3f &sample, const Point3f& x) const = 0;
	
	/*-----------------*/
	/* Utility methods */
	/*-----------------*/

	/// Return an axis-aligned bounding box of the entire object
	virtual const BoundingBox3f &getBoundingBox() const { return m_bbox; }

	/// Return Centroid of the Shape
	virtual Point3f getCentroid() const; 

	/// Return whether the Shape is a mesh or not
	virtual bool isMesh() const { return false; }

	/// Is this object an area emitter?
	virtual bool isEmitter() const { return m_emitter != nullptr; }

	/// Return a pointer to an attached area emitter instance
	virtual Emitter *getEmitter() { return m_emitter; }

	/// Return a pointer to an attached area emitter instance (const version)
	virtual const Emitter *getEmitter() const { return m_emitter; }

	/// Return a pointer to the BSDF associated with this object
	virtual const BSDF *getBSDF() const { return m_bsdf; }

	/// Return the name of this object
	virtual const std::string &getName() const { return m_name; }

	/// Register a child object (e.g. a BSDF) with the object
	virtual void addChild(NoriObject *child);

	/// Return a human-readable summary of this instance
	virtual std::string toString() const;

	/**
	* \brief Return the type of object (i.e. Mesh/BSDF/etc.)
	* provided by this instance
	* */
	virtual EClassType getClassType() const { return EClassType::EShape; }

protected:
	/// Create an empty Object
	Shape() {}

protected:
	std::string m_name;                  ///< Identifying name

	BSDF         *m_bsdf = nullptr;      ///< BSDF of the surface
	Emitter      *m_emitter = nullptr;   ///< Associated emitter, if any
	BoundingBox3f m_bbox;                ///< Bounding box of the object
};


NORI_NAMESPACE_END
