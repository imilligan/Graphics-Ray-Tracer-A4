/*
 * CS 488 Assignment 4
 * Ian Milligan 
*/
#include "a4.hpp"
#include "image.hpp"
#include <cmath>
#include <algorithm>
#include "polyroots.hpp"

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define SHADOW_THRESHOLD 0.0001 // The distance at which the hit interection can be away from our object
#define SUPERSAMPLING_FACTOR 3.0 // How much we are supersampling by (3x3 grid)

// Simple structure for defining a ray
struct Ray {
  Point3D point;
  Vector3D line;
};

// An abtraction of the ray tracing function so we can perform supersampling
Colour* perform_ray_trace(Ray ray, double percentage_x, double percentage_y, SceneNode* root, const Colour& ambient, const std::list<Light*>& lights) {
  
  // The colour to return
  Colour* ret_colour;

  // Find the points that hit
  std::list<Hit*> hits = root->ray_trace(ray.point, ray.line, Matrix4x4());
  if (hits.size() > 0) {
    Hit* min_hit = NULL;

    // If there are hits, find the minimum t value (the closest)
    for (std::list<Hit*>::iterator I = hits.begin(); I != hits.end(); ) {
      if (min_hit == NULL || (*I)->t < min_hit->t) {
        min_hit = *I;
        I++;
      } else {
        std::list<Hit*>::iterator old = I++;
        delete (*old)->normal;
        delete *old;
      }
    }
    
    // The intersection point in world coordinates
    Point3D intersection_point = ray.point + (min_hit->t * ray.line);
    Vector3D* normal = min_hit->normal;
    Material* hit_material = min_hit->material;
    normal->normalize();

    // First setup the ambient colour
    Colour* ambient_colour = hit_material->ambient_light(ambient);
    // The segment colour.  We will add to this for each light source
    Colour segment_colour = Colour(*ambient_colour);
    delete ambient_colour;

    for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {

      // For each light, create a ray from the intersection point to the light
      Ray intersection_to_light = {
        Point3D(intersection_point),
        (**I).position - intersection_point
      };

      // Ray trace to see if any lights are blocking
      std::list<Hit*> blocking = root->ray_trace(intersection_to_light.point, intersection_to_light.line, Matrix4x4());
      if (blocking.size() > 0) {
        bool is_blocking = false;
        for (std::list<Hit*>::iterator I = blocking.begin(); I != blocking.end(); ) {
          Point3D blocking_point = intersection_to_light.point + ((*I)->t * intersection_to_light.line);
          // Make sure we aren't just hitting our own surface
          if ((blocking_point - intersection_point).length() > SHADOW_THRESHOLD) {
            is_blocking = true;
          }
          std::list<Hit*>::iterator old = I++;
          delete (*old)->normal;
          delete *old;
        }

        if (is_blocking) {          
          continue;
        }
      }

      // Define l,v,r as outlined in the lighting model section of the course notes
      Vector3D l = (**I).position - intersection_point;
      Vector3D v = -1.0 * Vector3D(ray.line);
      double distance = l.length();
      l.normalize();
      v.normalize();
      Vector3D r = ((2 * l.dot(*normal)) * (*normal)) - l;
      r.normalize();

      // If the light is behind us, we can skip out early.
      double theta = acos(l.dot(v));
      if (theta > 0 && theta < M_PI) {
        Colour * model_colour = hit_material->lighting_model(l, v, r, *normal, *I, distance);
        segment_colour = segment_colour + *model_colour;
        delete model_colour;
      }
    }

    // Return the segment colour, clamped between 0 and 1
    ret_colour = new Colour(
      CLAMP(segment_colour.R(), 0.0, 1.0),
      CLAMP(segment_colour.G(), 0.0, 1.0),
      CLAMP(segment_colour.B(), 0.0, 1.0)
    );
    delete min_hit->normal;
    delete min_hit;
  } else {
    // Else return the background colour pattern
    ret_colour = new Colour(
      percentage_x > 0.5 ? 1 - percentage_x : percentage_x,
      percentage_y,
      1.0 - percentage_y
    );
  }
  return ret_colour;
}

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  // Use the view, eye, up and fov variables to establish the view frustum
  double view_magnatude = view.length();
  double viewport_size = view_magnatude * tan((M_PI * fov / 180) / 2);
  Vector3D right_normal = Vector3D(view.cross(up));
  right_normal.normalize();
  Vector3D up_normal = Vector3D(up);
  up_normal.normalize();

  // The image we will write into
  Image img(width, height, 3);

  int percentage_complete = 0;
  for (int y = 0; y < height; y++) {
    if ((int)(y * 100 / height) > percentage_complete) {
      percentage_complete = y * 100 / height;
      std::cout << "Percentage Complete: " << percentage_complete << "%" << std::endl;
    }
    for (int x = 0; x < height; x++) {
      // Get the ndc coordinates for this pixel and the next (for supersampling)
      double y_ndc = (((double)y * 2.0) / height) - 1;
      double y_next_ndc = (((double)(y + 1) * 2.0) / height) - 1;
      double x_ndc = (((double)x * 2.0) / width) - 1;
      double x_next_ndc = (((double)(x + 1) * 2.0) / width) - 1;

      // Supersampling code: create a SUPERSAMPLING_FACTOR^2 grid and then average the colour values returned 
      Colour pixel_colour = Colour(0, 0, 0);
      for (int j = 0; j < SUPERSAMPLING_FACTOR; j++) {
        for (int k = 0; k < SUPERSAMPLING_FACTOR; k++) {
          int index = (j * 3) + k;
          double x_segment_ndc = x_ndc + ((x_next_ndc - x_ndc) * ((double)j / SUPERSAMPLING_FACTOR));
          double y_segment_ndc = y_ndc + ((y_next_ndc - y_ndc) * ((double)k / SUPERSAMPLING_FACTOR));

          // The ray we will actually trace
          Ray segment_ray = {
            Point3D(eye),
            view + (x_segment_ndc * viewport_size * right_normal) - (y_segment_ndc * viewport_size * up_normal)
          };
          Colour* segment_colour = perform_ray_trace(segment_ray, (double)x / width, (double)y / height, root, ambient, lights);

          // Add to the rolling average
          pixel_colour = Colour(
            ((pixel_colour.R() * index) + segment_colour->R()) / (index + 1),
            ((pixel_colour.G() * index) + segment_colour->G()) / (index + 1),
            ((pixel_colour.B() * index) + segment_colour->B()) / (index + 1)
          );
          delete segment_colour;
        }
      }     

      // Set the colour of the pixels
      img(x, y, 0) = pixel_colour.R();
      img(x, y, 1) = pixel_colour.G();
      img(x, y, 2) = pixel_colour.B();
    }
  }
  
  img.savePng(filename);
  
}
