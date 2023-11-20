#pragma once

#include "color.h"
#include <OpenGL/gl.h>


struct Material {
  Color diffuse;
  float albedo;
  float specularAlbedo;
  float specularCoefficient;
  float reflectivity;
  float transparency;
  float refractionIndex;
  GLuint cubemapID;  // Identificación del cubemap (skybox)
};

