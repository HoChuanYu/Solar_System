#include <iostream>
#include "context.h"
#include "program.h"

void ShadowLightProgram::doMainLoop() {
  glUseProgram(programId);
  bool isSun=false;
  /* TODO#2-3: Render scene with shadow mapping
   *           1. Copy from LightProgram
   *           2. Pass LightViewMatrix, fakeLightPos, shadowMap, enableShadow to shader program
   * Note:     LightViewMatrix and fakeLightPos are the same as what we used is ShadowProgram
   */ 

  glm::vec3 fakelightPos = glm::vec3(0) ;  //  ctx->lightPos;ctx->lightDirection * -10.0f
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightMatrix;
  float near_plane = 1.0f, far_plane = 7.5f;
  lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  glm::vec3 light_front = glm::vec3(ctx->objects[3]->transformMatrix[3].x, ctx->objects[3]->transformMatrix[3].y,
                                    ctx->objects[3]->transformMatrix[3].z);
  lightView = glm::lookAt(fakelightPos, light_front, glm::vec3(0.0, 1.0, 0.0));
  lightMatrix = lightProjection * lightView;

  GLint enShaLoc = glGetUniformLocation(programId, "enableShadow");
  glUniform1i(enShaLoc, ctx->enableShadow);
  
  GLint lvmatLoc = glGetUniformLocation(programId, "LightViewMatrix");
  glUniformMatrix4fv(lvmatLoc, 1, GL_FALSE, glm::value_ptr(lightMatrix));
  
  GLint flpLoc = glGetUniformLocation(programId, "fakeLightPos");
  glUniform3f(flpLoc, fakelightPos[0], fakelightPos[1], fakelightPos[2]);

  int obj_num = (int)ctx->objects.size();
  
  for (int i = 0; i < obj_num; i++) {
    if (i == 9 && !ctx->showMoon) continue;
    int modelIndex = ctx->objects[i]->modelIndex;
    Model* model = ctx->models[modelIndex];
    glBindVertexArray(model->vao);

    if (i == 0)
      isSun = true;
    else
      isSun = false;

    
    GLint enSunLoc = glGetUniformLocation(programId, "isSun");
    glUniform1i(enSunLoc, isSun);

    const float* p = ctx->camera->getProjectionMatrix();
    GLint pmatLoc = glGetUniformLocation(programId, "Projection");
    glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

    const float* v = ctx->camera->getViewMatrix();
    GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
    glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

    const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrixs[i]);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

    glm::mat4 TIMatrix = glm::transpose(glm::inverse(model->modelMatrixs[i]));
    const float* ti = glm::value_ptr(TIMatrix);
    mmatLoc = glGetUniformLocation(programId, "TIModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, ti);

    const float* vp = ctx->camera->getPosition();
    mmatLoc = glGetUniformLocation(programId, "viewPos");
    glUniform3f(mmatLoc, vp[0], vp[1], vp[2]);

    glUniform3fv(glGetUniformLocation(programId, "dl.direction"), 1, glm::value_ptr(ctx->lightDirection));
    glUniform3fv(glGetUniformLocation(programId, "dl.position"), 1, glm::value_ptr(ctx->lightPos));

    glUniform1f(glGetUniformLocation(programId, "pl.constant"), ctx->pointLightConstant);
    glUniform1f(glGetUniformLocation(programId, "pl.linear"), ctx->pointLightLinear);
    glUniform1f(glGetUniformLocation(programId, "pl.quadratic"), ctx->pointLightQuardratic);

    glUniform3fv(glGetUniformLocation(programId, "dl.ambient"), 1, glm::value_ptr(ctx->lightAmbient));
    glUniform3fv(glGetUniformLocation(programId, "dl.diffuse"), 1, glm::value_ptr(ctx->lightDiffuse));
    glUniform3fv(glGetUniformLocation(programId, "dl.specular"), 1, glm::value_ptr(ctx->lightSpecular));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);
    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx->shadowMapTexture);
    glUniform1i(glGetUniformLocation(programId, "shadowMap"), 1);
    glDrawArrays(model->drawMode, 0, model->numVertex);
  }

  glUseProgram(0);
}
