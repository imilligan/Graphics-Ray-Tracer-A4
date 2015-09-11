#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;

  virtual bool ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal);
  
private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  NonhierBox* m_bounding_box;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
