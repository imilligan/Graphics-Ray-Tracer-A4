/*
 * CS 488 Assignment 4
 * Ian Milligan 
*/
#include "material.hpp"
#include <algorithm>

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
  // Perform OpenGL calls necessary to set up this material.
}

Colour* PhongMaterial::ambient_light(const Colour& ambient) {
  // Get the ambient light for a given object
	return new Colour(
		ambient.R() * m_kd.R(),
		ambient.G() * m_kd.G(),
		ambient.B() * m_kd.B()
	);
}
Colour* PhongMaterial::lighting_model(Vector3D l, Vector3D v, Vector3D r, Vector3D normal, Light* light, double distance) {
  // Get the diffuse and specular light based on the phong lighting model code in the notes
	double red_addition = (m_kd.R() + m_ks.R() * pow(r.dot(v), 10) / normal.dot(l)) * light->colour.R() * (l.dot(normal) / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance));
  double green_addition = (m_kd.G() + m_ks.G() * pow(r.dot(v), 10) / normal.dot(l)) * light->colour.G() * (l.dot(normal) / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance));
  double blue_addition = (m_kd.B() + m_ks.B() * pow(r.dot(v), 10) / normal.dot(l)) * light->colour.B() * (l.dot(normal) / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance));
  return new Colour(
  	std::max(0.0, red_addition),
  	std::max(0.0, green_addition),
  	std::max(0.0, blue_addition)
  );
}
