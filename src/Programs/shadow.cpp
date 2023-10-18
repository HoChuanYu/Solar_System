#include <iostream>
#include "context.h"
#include "opengl_context.h"
#include "program.h"

GLfloat borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

ShadowProgram::ShadowProgram(Context* ctx) : Program(ctx) {
  vertProgramFile = "../assets/shaders/shadow.vert";
  fragProgramFIle = "../assets/shaders/shadow.frag";

  // TODO#2-0: comment this line if your computer is poor
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &SHADOW_MAP_SIZE);
  std::cout << "Current depth map size is " << SHADOW_MAP_SIZE << std::endl;

  /* TODO#2-1 Generate frame buffer and depth map for shadow program
   *          1. Generate frame buffer and store to depthMapFBO
   *          2. Generate depthmap texture and store to  ctx->shadowMapTexture
   *             - texure size is SHADOW_MAP_SIZE * SHADOW_MAP_SIZE
   *          3. properly setup depthmap's texture paremters
   *             - Set texture wrap to "clamp to border" and use border color provider above
   *          4. bind texture to framebuffer's depth buffer and disable color buffer read and write
   * Hint:
   *          - glGenFramebuffers
   *          - glGenTextures
   *          - glBindTexture
   *          - glTexImage2D
   *          - glTexParameteri
   *          - GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T
   *          - glTexParameterfv
   *          - GL_TEXTURE_BORDER_COLOR
   *          - glBindFramebuffer
   *          - glFramebufferTexture2D
   *          - glDrawBuffer
   *          - glReadBuffer
   */
  glGenFramebuffers(1, &depthMapFBO);
  // create depth texture
  glGenTextures(1, &ctx->shadowMapTexture);
  glBindTexture(GL_TEXTURE_2D, ctx->shadowMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ctx->shadowMapTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);//---------------------------------------------------------------------------------------
}

double Ttime=0;
void ShadowProgram::doMainLoop() {
  //std::cout << ctx->enableShadow << std::endl;
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();

  glm::vec3 fakelightPos = glm::vec3(0);  // ctx->lightPos;ctx->lightDirection * -10.0f
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightMatrix;
  float near_plane = 1.0f, far_plane = 7.5f;
  lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  glm::vec3 light_front = glm::vec3(ctx->objects[3]->transformMatrix[3].x, ctx->objects[3]->transformMatrix[3].y,
                               ctx->objects[3]->transformMatrix[3].z);
  lightView = glm::lookAt(fakelightPos, light_front, glm::vec3(0.0, 1.0, 0.0));
  lightMatrix = lightProjection * lightView;

  glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);

  GLint vmatLoc = glGetUniformLocation(programId, "LightViewMatrix");
  glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, glm::value_ptr(lightMatrix));

  Ttime += ctx->t_gap; 
  for (int i =0; i < obj_num; i++) {
     if (i != 0)
    ctx->objects[i]->transformMatrix =
        glm::rotate(ctx->objects[i]->transformMatrix, glm::radians(-1.0f), glm::vec3(0, 10, 0));
    if (i != 0 && i != 9) {
        ctx->objects[i]->transformMatrix =
          glm::translate(glm::identity<glm::mat4>(),
                         //glm::vec3(2 * i * cos(Ttime / i / i ), 0, 2 * i * sin(Ttime / i / i)));
                           glm::vec3(2 * i * cos(Ttime / i / i + 9 * i), 0, 2 * i * sin(Ttime / i / i + 9 * i)));
      }
  }
  

    ctx->objects[9]->transformMatrix =
        glm::translate(ctx->objects[3]->transformMatrix, glm::vec3(cos(Ttime * 2), 0, sin(Ttime * 2)));
   glm::mat4 temp = glm::translate(ctx->objects[3]->transformMatrix, glm::vec3(0, -5, 0));
    if (ctx->camera->getEarthView()) ctx->camera->changeEarthPos(ctx->objects[3]->transformMatrix[3]);
    ctx->camera->updateViewMatrix();

     /**/


  for (int i = 1; i < obj_num; i++) {
    if (i != 3 && i != 9) continue;
    int modelIndex = ctx->objects[i]->modelIndex;
    Model* model = ctx->models[modelIndex];
    glBindVertexArray(model->vao);
  
    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrixs[i]);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);
    glDrawArrays(model->drawMode, 0, model->numVertex);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(0, 0, OpenGLContext::getWidth(), OpenGLContext::getHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(0);
}