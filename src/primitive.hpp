/*
 * CS 488 Assignment 4
 * Ian Milligan 
*/
#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual bool ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal);
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  virtual bool ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual bool ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal);

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_dimensions(size, size, size)
  {
  }
  NonhierBox(const Point3D& pos, const Vector3D& max_pos)
    : m_pos(pos), m_dimensions(max_pos)
  {
  }
  
  virtual ~NonhierBox();
  virtual bool ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view, double* hit_t, Vector3D** hit_normal);

private:
  Point3D m_pos;
  Vector3D m_dimensions;
};

#endif
