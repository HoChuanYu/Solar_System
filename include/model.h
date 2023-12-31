#pragma once

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

class Model;

void attachGeneralObjectVAO(Model* model);

void attachSkyboxVAO(Model* model);

class Model {
 public:

 public:
  // VBO data for the position of the vertex
  std::vector<float> positions;
  // VBO data for the normal of the vertex
  std::vector<float> normals; 
   // Or uv coordinates, VBO data for the 2D texture mapping of the vertex
  std::vector<float> texcoords;

  // Total number of vertex 
  int numVertex = 0; 
  // Mode parameter for glDrawArrays
  GLenum drawMode = GL_TRIANGLES; 

  // VAO
  GLuint vao;
  
  // Matrix transfer from model local space to world space.
  // Used to scale model to acceptable base size
  std::vector<glm::mat4> modelMatrixs;
  // Ids for texture of this model
  std::vector<GLuint> textures; 

  static Model* fromObjectFile(const char* obj_file);
};

// Represent an object in the scene
struct Object {
 public:
  // Index of model for this object in model vector
  int modelIndex = 0;
  // Index of texture id for this object in the model::textures vector in model for this object
  int textureIndex = 0;
  // Matrix for translate, rotate and scaling in world space
  glm::mat4 transformMatrix;

  glm::mat4 rotationMatrix = glm::identity<glm::mat4>();

  Object(int modelIndex, glm::mat4 transformMatrix) : modelIndex(modelIndex), transformMatrix(transformMatrix) {}
};
