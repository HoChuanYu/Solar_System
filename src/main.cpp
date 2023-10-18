#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "context.h"
#include "gl_helper.h"
#include "model.h"
#include "opengl_context.h"
#include "program.h"
#include "utils.h"
#include "constants.h"

void initOpenGL();
void resizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int, int action, int);

Context ctx;

void loadPrograms() {
  /* TODO#1~3 uncoment lines to enable shader programs
   * Notes:
   *   SkyboxProgram for TODO#1
   *   ShadowProgram, ShadowLightProgram for TODO#2
   *     - also comment out default LightProgram
   *   FilterProgramBindFrameAdapter, FilterProgram for TODO#3
   *     - FilterProgramBindFrameAdapter is used to change render buffer for skybox and light program
   */
  ctx.programs.push_back(new ShadowProgram(&ctx));
  ctx.programs.push_back(new SkyboxProgram(&ctx));
  //ctx.programs.push_back(new LightProgram(&ctx));
  ctx.programs.push_back(new ShadowLightProgram(&ctx));

  // TODO#0: You can trace light program before doing hw to know how this template work and difference from hw2
  for (auto iter = ctx.programs.begin(); iter != ctx.programs.end(); iter++) {
    if (!(*iter)->load()) {
      std::cout << "Load program fail, force terminate" << std::endl;
      exit(1);
    }
  }
  glUseProgram(0);
}

void loadModels() {
  // TODO#0: You can trace light program before doing hw to know how this template work and difference from hw2
  /*
  Model* m = Model::fromObjectFile("../assets/models/cube/cube.obj");
  m->textures.push_back(createTexture("../assets/models/cube/texture.bmp"));
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));
  attachGeneralObjectVAO(m);
  ctx.models.push_back(m);
  */
  Model* m = Model::fromObjectFile("../assets/models/Planet/Sun.obj");
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Sun.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.3f, 0.3f, 0.3f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Mercury.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1f, 0.1f, 0.1f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Venus.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1f, 0.1f, 0.1f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Earth.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1f, 0.1f, 0.1f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Mars.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1f, 0.1f, 0.1f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Jupiter.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.2f, 0.2f, 0.2f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Saturn.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.2f, 0.2f, 0.2f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Neptune.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.2f, 0.2f, 0.2f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Uranus.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.2f, 0.2f, 0.2f)));
  m->textures.push_back(createTexture("../assets/models/Planet/Textures/Moon.jpg"));
  m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.06f, 0.06f, 0.06f)));
  //m->textures.push_back(createTexture("../assets/models/Planet/Textures/Saturn_Ring.jpg"));
  //m->modelMatrixs.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1f, 0.1f, 0.1f)));
  attachGeneralObjectVAO(m);
  ctx.models.push_back(m);

  // TODO#1-1: Add skybox mode
  m = new Model();
  for (int i = 0; i < 36 * 3; i++) {
    m->positions.push_back(skyboxVertices[i]);
  }
  m->drawMode = GL_TRIANGLES;
  m->textures.push_back(createCubemap(blueSkyboxfaces));
  m->numVertex = 36;
  attachSkyboxVAO(m);
  ctx.models.push_back(m);
}

void setupObjects() {
  ctx.objects.push_back(new Object(0, glm::identity<glm::mat4>()));
  //ctx.models[ctx.models.size()-1] = 
  (*ctx.objects.rbegin())->textureIndex = 0;    //Sun
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 1;    //Mercury
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(6, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 2;    //Venus
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(8, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 3;    //Earth
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(10, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 4;    //Mars
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(12, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 5;    //Jupiter
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(14, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 6;    //Saturn
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(16, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 7;    //Neptune
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(18, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 8;    //Uranus
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(2, 0, 0))));
  (*ctx.objects.rbegin())->textureIndex = 9;    //Moon

  /* TODO#1-1: Uncomment to create skybox Object
   * Note:     Skybox object is put in Context::skybox rather than Context::objects 
   */
  ctx.skybox = new Object(1, glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, 0)));
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW3 - `your student id`"
   *         Ex. HW3 - 311550000
   */
  glfwSetWindowTitle(window, "HW3");

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);
  ctx.camera = &camera;
  ctx.window = window;

  loadModels();
  loadPrograms();
  setupObjects();

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);

    ctx.lightDegree = glm::clamp(ctx.lightDegree, 30.0f, 160.0f);
    ctx.lightDirection =
        glm::vec3(-0.3, -0.3 * sinf(glm::radians(ctx.lightDegree)), -0.3 * cosf(glm::radians(ctx.lightDegree)));

    // TODO#0: You can trace light program before doing hw to know how this template work and difference from hw2
    size_t sz = ctx.programs.size();
    for (size_t i = 0; i < sz; i++) {
      ctx.programs[i]->doMainLoop();
    }
    

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_K:
        ctx.lightDegree += 1.0f;
        break;
      case GLFW_KEY_L:
        ctx.lightDegree -= 1.0f;
        break;
      case GLFW_KEY_Z:
        ctx.t_gap -= 0.01f;
        break;
      case GLFW_KEY_X:
        ctx.t_gap += 0.01f;
        break;
      default:
        break;
    }
    return;
  }
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F9: {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
          // Show the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
          // Hide the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
      }
      case GLFW_KEY_Y:
        ctx.enableShadow = !ctx.enableShadow;
        break;
      case GLFW_KEY_U:
        ctx.enableEdgeDetection = !ctx.enableEdgeDetection;
        break;
      case GLFW_KEY_I:
        ctx.eanbleGrayscale = !ctx.eanbleGrayscale;
        break;
      case GLFW_KEY_C:
        ctx.t_gap = 0;
        break;
      case GLFW_KEY_V:
        ctx.t_gap = 0.005;
        break;
      case GLFW_KEY_T:
        ctx.showMoon = !ctx.showMoon;
        break;
      default:
        break;
    }
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  // TODO#3 uncomment this to update frame buffer size when window size chnage
  // fp->updateFrameBuffer(width, height);
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, G  LFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}