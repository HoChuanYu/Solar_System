#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "model.h"
#include "camera.h"
#include "program.h"

// Global varaibles share between main.cpp and shader programs
class Context {
 public:
  std::vector<Program* > programs;
  std::vector<Model* > models;
  std::vector<Object* > objects;
  Object* skybox;

  GLuint shadowMapTexture;
  GLuint enableShadow = 1;
  GLuint enableEdgeDetection = 0;
  GLuint eanbleGrayscale = 0;

 public:
  int showMoon=1;
  int smallMoon = 0;
  glm::vec3 lightPos = glm::vec3(0);
  float lightDegree = 30.0f;
  double t_gap = 0.01;
  float pointLightConstant = 0.9f;
  float pointLightLinear = 0.027f;
  float pointLightQuardratic = 0.0128f;
  glm::vec3 lightDirection = glm::vec3(-0.3f, -0.5f, -0.2f);
  glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
  glm::vec3 lightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
  glm::vec3 lightSpecular = glm::vec3(0.3f, 0.3f, 0.3f);

 public:
  Camera *camera = 0;
  GLFWwindow *window = 0;
};