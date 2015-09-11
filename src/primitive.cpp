/*
 * CS 488 Assignment 4
 * Ian Milligan 
*/
#include "primitive.hpp"
#include "polyroots.hpp"
#include <climits>

#define CUBE_EPSILON 0.00000001

// Helper function to reduce copied code
bool trace_sphere(
	const Point3D& point,
	const Vector3D& line,
	Point3D pos,
	double radius,
	const Matrix4x4& model_to_view,
	double* hit_t,
	Vector3D** hit_normal) {

	Point3D transformed_point = model_to_view.invert() * point;
	Vector3D transformed_line = model_to_view.invert() * line;

	// Find the A, B and C of the quadratic equation
	double A = transformed_line.dot(transformed_line);
  double B = ((double)2 * (transformed_point - pos)).dot(transformed_line);
  double C = ((double)-1 * (transformed_point - pos)).dot((double)-1 * (transformed_point - pos)) - (radius * radius);
  double closest_t = -1;
  double roots[2];
  size_t num_roots = quadraticRoots(A, B, C, roots);
  
  // Select the minimum roots
  if (num_roots) {
    if (roots[0] < roots[1] && roots[0] >= 0) {
      closest_t = roots[0];
    } else {
      closest_t = roots[1];
    }
  }

  // Find the intersection point, and the normal from said point
  Point3D intersection_point = transformed_point + (closest_t * transformed_line);
  if (closest_t > 0) {
  	Vector3D *normal = new Vector3D(model_to_view.invert().transpose() * (intersection_point - pos));
	  (*hit_t) = closest_t;
  	(*hit_normal) = normal;
  	return true;
  } else {
  	return false;
  }
}

// Helper function to reduce copied code
bool trace_box(
	const Point3D& point,
	const Vector3D& line,
	Point3D pos,
	Point3D max_pos,
	const Matrix4x4& model_to_view,
	double* hit_t,
	Vector3D** hit_normal) {

	Point3D transformed_point = model_to_view.invert() * point;
	Vector3D transformed_line = model_to_view.invert() * line;
  
  // The ray will intersect any two given x, y, or z planes at a near location and far location.
	double t_near = INT_MIN;
	double t_far = INT_MAX;
	Vector3D* near_normal = NULL;
	Vector3D* far_normal = NULL;
  // For each axis
	for (int axis = 0; axis < 3; axis++) {
		if (transformed_line[axis] > -CUBE_EPSILON && transformed_line[axis] < CUBE_EPSILON) {
			// The ray runs parallel to the planes and isn't inside the bounds
			if (transformed_point[axis] < pos[axis] || transformed_point[axis] > max_pos[axis]) {
				return false;
			}
		} else {
      // See where the ray hits the near and far planes
			double t1 = (pos[axis] - transformed_point[axis]) / transformed_line[axis];
			double t2 = (max_pos[axis] - transformed_point[axis]) / transformed_line[axis];
			bool has_swapped = false;
			if (t1 > t2) {
        // Make sure t1 is the smaller of the two.
        // Set has_swapped if this is the case
				double swap = t1;
				t1 = t2;
				t2 = swap;
				has_swapped = true;
			}
      
      // One of the normal vectors for our planes
			Vector3D normal = Vector3D();
      normal[axis] = 1;
			if (t1 > t_near) {
				t_near = t1;
				delete near_normal;
				near_normal = has_swapped ? new Vector3D(normal) : new Vector3D(-1.0 * normal);
			}

			if (t2 < t_far) {
				t_far = t2;
				delete far_normal;
				far_normal = has_swapped ? new Vector3D(-1.0 * normal) : new Vector3D(normal);
			}

			// The box is missed 
			if (t_near > t_far) {
				return false;
			}

			// The box is behind the ray
			if (t_far < 0) {
				return false;
			}
		}
	}
  
  double closest_t;
  Vector3D* normal;
  if (t_near > 0) {
    closest_t = t_near;
    normal = near_normal;
    delete far_normal;  
  } else {
    closest_t = t_far;
    normal = far_normal;
    delete near_normal; 
  }

  if (closest_t > 0) {
	  (*hit_t) = closest_t;
		Vector3D* transformed_normal = new Vector3D(model_to_view.invert().transpose() * (*normal));
	  delete normal;
		(*hit_normal) = transformed_normal;
		return true;
  } else {
  	delete normal;
  	return false;
  }
}

Primitive::~Primitive()
{
}

bool Primitive::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal) {
	return false;
}

Sphere::~Sphere()
{
}

bool Sphere::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal) {
	return trace_sphere(point, line, Point3D(), 1.0, model_to_view, hit_t, hit_normal);
}

Cube::~Cube()
{
}

bool Cube::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal) {
	return trace_box(point, line, Point3D(), Point3D() + Vector3D(1.0, 1.0, 1.0), model_to_view, hit_t, hit_normal);
}

NonhierSphere::~NonhierSphere()
{

}

bool NonhierSphere::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal) {
	return trace_sphere(point, line, m_pos, m_radius, model_to_view, hit_t, hit_normal);
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal) {
	return trace_box(point, line, m_pos, m_pos + m_dimensions, model_to_view, hit_t, hit_normal);
}
