#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "shader.hpp"
#include "camera.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class CubeRenderer {
public:
  CubeRenderer(const CubeRenderer&) = delete;
  static CubeRenderer& get() { return instance; }

  void init();

  void render();

  glm::vec3 pos;
  glm::vec2 angle;

  uint64_t numCubes;

  ~CubeRenderer();
private:
  CubeRenderer() {};
  static CubeRenderer instance;
  float vertices[180] = {
    // Back - Green
    -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,
     0.5f, -0.5f, -0.5f,  0.5f , 0.5f,
     0.5f,  0.5f, -0.5f,  0.5f , 1.0f,
     0.5f,  0.5f, -0.5f,  0.5f , 1.0f,
    -0.5f,  0.5f, -0.5f,  0.25f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,

    // Front - Blue
    -0.5f, -0.5f,  0.5f,  0.25f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.5f , 0.0f,
     0.5f,  0.5f,  0.5f,  0.5f , 0.5f,
     0.5f,  0.5f,  0.5f,  0.5f , 0.5f,
    -0.5f,  0.5f,  0.5f,  0.25f, 0.5f,
    -0.5f, -0.5f,  0.5f,  0.25f, 0.0f,

    // Left - Red
    -0.5f,  0.5f,  0.5f,  0.5f , 0.5f,
    -0.5f,  0.5f, -0.5f,  0.75f, 0.5f,
    -0.5f, -0.5f, -0.5f,  0.75f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.75f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.5f , 1.0f,
    -0.5f,  0.5f,  0.5f,  0.5f , 0.5f,

    // Right - Orange
     0.5f,  0.5f,  0.5f,  0.5f , 0.0f,
     0.5f,  0.5f, -0.5f,  0.75f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.75f, 0.5f,
     0.5f, -0.5f, -0.5f,  0.75f, 0.5f,
     0.5f, -0.5f,  0.5f,  0.5f , 0.5f,
     0.5f,  0.5f,  0.5f,  0.5f , 0.0f,

    // Bottom - Yellow
    -0.5f, -0.5f, -0.5f,  0.0f , 0.0f,
     0.5f, -0.5f, -0.5f,  0.25f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.25f, 0.5f,
     0.5f, -0.5f,  0.5f,  0.25f, 0.5f,
    -0.5f, -0.5f,  0.5f,  0.0f , 0.5f,
    -0.5f, -0.5f, -0.5f,  0.0f , 0.0f,

    // Top - White
    -0.5f,  0.5f, -0.5f,  0.0f , 0.5f,
     0.5f,  0.5f, -0.5f,  0.25f, 0.5f,
     0.5f,  0.5f,  0.5f,  0.25f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f , 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f , 0.5f
  };
  uint VBO, VAO, modelMatBuf;

  glm::mat4* modelMatrices;
  
  float radius = 150.0;
  float offset = 25.0f;

  uint cubeTexture;
  int width, height, nrChannels;
  uint8_t* data;
};

class App {
public:
  App(const App&) = delete;
  static App& get() { return instance; }
  void init(const uint64_t p_cubeSize = 3);

  void update();

  bool getVsyncState() { return vsync; }
  void disableVSYNC();
  void enableVSYNC();
  bool vsyncKeyDownUsed = false;

  bool open = true;
  bool pollFPS = false;
  uint fps;
  
  uint64_t cubeSize = 3;
  
  uint SCR_WIDTH = 800;
  uint SCR_HEIGHT = 600;
  bool wireframe = false;
  
  // camera
  Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  float lastX = SCR_WIDTH / 2.0f;
  float lastY = SCR_HEIGHT / 2.0f;
  bool firstMouse = true;

  Shader* shaderProgram;

  // timing
  float deltaTime = 0.0f; // Time between current and last frame
  float lastFrame = 0.0f; // Time of last frame

  std::vector<glm::vec3> cubePositions;

  ~App();
private:
  App() {};
  static App instance;

  GLFWwindow* window = nullptr;

  bool vsync = false;
  
  uint8_t frames;
  double sampleStartTime;
};
