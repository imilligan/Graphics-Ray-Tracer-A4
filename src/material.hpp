#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "light.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;
  virtual Colour* ambient_light(const Colour& ambient) = 0;
  virtual Colour* lighting_model(Vector3D l, Vector3D v, Vector3D r, Vector3D normal, Light* light, double distance) = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;
  virtual Colour* ambient_light(const Colour& ambient);
  virtual Colour* lighting_model(Vector3D l, Vector3D v, Vector3D r, Vector3D normal, Light* light, double distance);

public:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};


#endif
