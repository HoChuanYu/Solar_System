#include <iostream>
#include "context.h"
#include "program.h"
#include "opengl_context.h"

FilterProgram::FilterProgram(Context *ctx) : Program(ctx) {
  vertProgramFile = "../assets/shaders/filter.vert";
  fragProgramFIle = "../assets/shaders/filter.frag";

  // TODO#3-1: Generate Framebuffer and VAO/VBO for filter
  // Note:     You need to design proper position/texcoord data for filter program (NDC)
  float positions[] = {
    -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f,  -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
  };
  float texcoords[] = {
      0.0f, 1.0f,
      0.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 0.0f,
      1.0f, 1.0f
  };
  glGenVertexArrays(1, &quadVAO);
  glBindVertexArray(quadVAO);
  glGenBuffers(2, quadVBO);

  glBindBuffer(GL_ARRAY_BUFFER, quadVBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, quadVBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), &texcoords, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindVertexArray(0);

  glGenFramebuffers(1, &filterFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, filterFBO);  

  //----------------------------------------------------------------------
  updateFrameBuffer(OpenGLContext::getWidth(), OpenGLContext::getHeight());
}

void FilterProgram::updateFrameBuffer(int SCR_WIDTH, int SCR_HEIGHT) {
  /* TODO#3-1: generate color/depth buffer for frame buffer
   *           (this function will also be trigger when windown resize)
   *           1. Generate color buffer texture and store in colorBuffer
   *           2. Set texture size to SCR_WIDTH*SCR_HEIGHT
   *           3. Set MIN/MAG filter to linear
   *           4. Generate/Bind a render buffer inad store in rboDepth
   *           5. Set Render buffer size to SCR_WIDTH*SCR_HEIGHT
   *           6. Attach colorBuffer and rboDepth to filterFBO
   * Hint:
   *           - glGenRenderbuffers
   *           - glBindRenderbuffer
   *           - glRenderbufferStorage
   *           - glBindFramebuffer
   *           - glFramebufferTexture2D
   *           - glFramebufferRenderbuffer
   */
  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FilterProgram::bindFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, filterFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FilterProgram::doMainLoop() {
  glUseProgram(programId);

  /* TODO#3-1: pass VAO, enableEdgeDetection, eanbleGrayscale, colorBuffer to shader and render
   */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glBindVertexArray(quadVAO);

  GLint GrayEnLoc = glGetUniformLocation(programId, "eanbleGrayscale");
  glUniform1i(GrayEnLoc, ctx->eanbleGrayscale);  ////////////////////////

  GLint EdgeEnLoc = glGetUniformLocation(programId, "enableEdgeDetection");
  glUniform1i(EdgeEnLoc, ctx->enableEdgeDetection);  ////////////////////////

  glUniform1i(glGetUniformLocation(programId, "colorBuffer"), 0);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  //--------------
  glUseProgram(0);
}