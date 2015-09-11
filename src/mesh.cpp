/*
 * CS 488 Assignment 4
 * Ian Milligan 
*/
#include "mesh.hpp"
#include <iostream>
#include <climits>
#include <algorithm>

#define MESH_EPSILON 0.000001
#define BOUNDING_EPSILON 0.0001

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
  // Get the minimum and maximum coordinates to make our bounding box 
  double min_x = INT_MAX;
  double min_y = INT_MAX;
  double min_z = INT_MAX;
  double max_x = INT_MIN;
  double max_y = INT_MIN;
  double max_z = INT_MIN;

  for (std::vector<Point3D>::const_iterator I = m_verts.begin(); I != m_verts.end(); ++I) {
    Point3D point = *I;
    if (point[0] < min_x) {
      min_x = point[0];
    }
    if (point[0] > max_x) {
      max_x = point[0];
    }

    if (point[1] < min_y) {
      min_y = point[1];
    }
    if (point[1] > max_y) {
      max_y = point[1];
    }

    if (point[2] < min_z) {
      min_z = point[2];
    }
    if (point[2] > max_z) {
      max_z = point[2];
    }
  }
  
  // Create the box, with the anchor as the minimum x, y and z, and the size as the axis differences
  Point3D anchor = Point3D(
    min_x,
    min_y,
    min_z 
  );

  m_bounding_box = new NonhierBox(anchor, Vector3D(max_x - min_x, max_y - min_y, max_z - min_z));
}

bool Mesh::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal) {
  
  double bounding_hit_t;
  Vector3D* bounding_normal;
  if (!m_bounding_box->ray_trace(point, line, model_to_view, &bounding_hit_t, &bounding_normal)) {
    return false;
  }

  Point3D transformed_point = model_to_view.invert() * point;
  Vector3D transformed_line = model_to_view.invert() * line;

  double min_t = INT_MAX;
  Vector3D* min_normal = NULL;
  for (std::vector<Mesh::Face>::const_iterator I = m_faces.begin(); I != m_faces.end(); ++I) {
    Mesh::Face face = *I;

    Vector3D* normal = new Vector3D((m_verts[face[2]] - m_verts[face[1]]).cross(m_verts[face[0]] - m_verts[face[1]]));

    if (transformed_line.dot(*normal) > -MESH_EPSILON && transformed_line.dot(*normal) < MESH_EPSILON) {
      continue;
    }

    double t = (m_verts[face[0]] - transformed_point).dot(*normal) / transformed_line.dot(*normal);

    //std::cerr << "Mesh: " << t << std::endl;

    Point3D plane_point = transformed_point + (t * transformed_line);
    bool inside_face = true;
    for (unsigned int i = 0; i < face.size(); i++) {
      Point3D current_point = m_verts[face[i]];
      Point3D next_point = m_verts[face[(i + 1) % face.size()]];
      Vector3D q = normal->cross(next_point - current_point);
      if ((plane_point - current_point).dot(q) < 0) {
        inside_face = false;
        break;
      }
    }

    if (inside_face && t > 0 && t < min_t) {
      min_t = t;
      delete min_normal;
      min_normal = normal;
    } else {
      delete normal;
    }
  }

  (*hit_t) = min_t;
  Vector3D *normal = NULL;
  if (min_normal != NULL) {
    normal = new Vector3D(model_to_view.invert().transpose() * (*min_normal));
    delete min_normal;
  }
  (*hit_normal) = normal;
  //std::cerr << "May have hit a mesh: " << (min_normal != NULL) << std::endl;
  return min_normal != NULL;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
