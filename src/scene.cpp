/*
 * CS 488 Assignment 4
 * Ian Milligan 
*/
#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
  // Rotation code
  double sin_angle = sin(angle * M_PI / 180);
  double cos_angle = cos(angle * M_PI / 180);
  Matrix4x4 rotate_matrix;
  switch (axis) {
    case 'x':
      rotate_matrix = Matrix4x4(
        Vector4D(1, 0, 0, 0),
        Vector4D(0, cos_angle, 0 - sin_angle, 0),
        Vector4D(0, sin_angle, cos_angle, 0),
        Vector4D(0, 0, 0, 1)
      );
    break;
    case 'y':
      rotate_matrix = Matrix4x4(
        Vector4D(cos_angle, 0, sin_angle, 0),
        Vector4D(0, 1, 0, 0),
        Vector4D(-sin_angle, 0, cos_angle, 0),
        Vector4D(0, 0, 0, 1)
      );
    break;
    case 'z':
      rotate_matrix = Matrix4x4(
        Vector4D(cos_angle, 0 - sin_angle, 0, 0),
        Vector4D(sin_angle, cos_angle, 0, 0),
        Vector4D(0, 0, 1, 0),
        Vector4D(0, 0, 0, 1)
      );
    break;
  }
  set_transform(m_trans * rotate_matrix);
}

void SceneNode::scale(const Vector3D& amount)
{
  // Scale code
  Matrix4x4 scale_matrix = Matrix4x4(
    Vector4D(amount[0], 0, 0, 0),
    Vector4D(0, amount[1], 0, 0),
    Vector4D(0, 0, amount[2], 0),
    Vector4D(0, 0, 0, 1)
  );
  set_transform(m_trans * scale_matrix);  
}

void SceneNode::translate(const Vector3D& amount)
{
  // Translate code
    Matrix4x4 translate_matrix = Matrix4x4(
    Vector4D(1, 0, 0, amount[0]),
    Vector4D(0, 1, 0, amount[1]),
    Vector4D(0, 0, 1, amount[2]),
    Vector4D(0, 0, 0, 1)
  );
  set_transform(m_trans * translate_matrix);
}

bool SceneNode::is_joint() const
{
  return false;
}

std::list<Hit*> SceneNode::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view) {
  list<Hit*> ret_list;
  
  // Add on to our transformation matrix
  Matrix4x4 transformed_matrix = model_to_view * m_trans;
  std::list<SceneNode*>::const_iterator iterator;
  for (iterator = m_children.begin(); iterator != m_children.end(); ++iterator) {
    // Trace each of the children, adding to the return list
    list<Hit*> hits = (*iterator)->ray_trace(point, line, transformed_matrix);
    ret_list.splice(ret_list.end(), hits);      
  }

  return ret_list;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

std::list<Hit*> JointNode::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view) {
  list<Hit*> ret_list;

  // Add on to our transformation matrix
  Matrix4x4 transformed_matrix = model_to_view * m_trans;
  std::list<SceneNode*>::const_iterator iterator;
  for (iterator = m_children.begin(); iterator != m_children.end(); ++iterator) {
    // Trace each of the children, adding to the return list
    list<Hit*> hits = (*iterator)->ray_trace(point, line, transformed_matrix);
    ret_list.splice(ret_list.end(), hits);      
  }

  return ret_list;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

std::list<Hit*> GeometryNode::ray_trace(const Point3D& point, const Vector3D& line, const Matrix4x4& model_to_view) {

  list<Hit*> ret_list;
  
  // Add on to our transformation matrix
  Matrix4x4 transformed_matrix = model_to_view * m_trans;
  double new_t;
  Vector3D* new_normal;
  // Check the primative associated with the geo node
  if (m_primitive->ray_trace(point, line, transformed_matrix, &new_t, &new_normal)) {
    // Return a hit
    Hit* geo_hit = new Hit(new_t, new_normal, m_material);
    ret_list.push_back(geo_hit);
  }

  return ret_list;

}
 
