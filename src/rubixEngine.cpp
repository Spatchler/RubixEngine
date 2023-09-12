#include "rubixEngine.hpp"

App App::instance;
CubeRenderer CubeRenderer::instance;

void CubeRenderer::init() {
    modelMatrices = new glm::mat4[numCubes];
    srand(static_cast<uint>(glfwGetTime())); // initialize random seed
    for (uint i = 0; i < numCubes; ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, App::get().cubePositions[i]);
        modelMatrices[i] = model;
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &modelMatBuf);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatBuf);
    glBufferData(GL_ARRAY_BUFFER, numCubes * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // modelMatrices vertex attribute
    glBindVertexArray(VAO);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6); 
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);

    // Load and generate textures
    glGenTextures(1, &cubeTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("res/textures/cubetexture.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture\n";
    }

    stbi_image_free(data);

    pos = glm::vec3(0.0f, 0.0f, -static_cast<float>(App::get().cubeSize) * 2);
    angle = glm::vec2(0.0f, 0.0f);

    App::get().shaderProgram->use();
    App::get().shaderProgram->setInt("cubeTexture", 0);
}

void CubeRenderer::render() {
    glBindVertexArray(VAO);
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0, 1.0, 0.0));
    // if (p_pos.y == 0.5)
    //     model = glm::rotate(model, glm::radians((float)glfwGetTime() * 180), glm::vec3(0.0, 1.0, 0.0));
    // model = glm::translate(model, glm::vec3(p_pos.x+0.5, p_pos.y+0.5, p_pos.z+0.5));
    App::get().shaderProgram->setMat4("model", model);
    App::get().shaderProgram->use();
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numCubes);
    glBindVertexArray(0);
}

CubeRenderer::~CubeRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &modelMatBuf);
}

void App::init(const uint64_t p_cubeSize) {
    cubeSize = p_cubeSize;
    // GLFW Init and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MACOSX
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW Window creation
    window = glfwCreateWindow(App::get().SCR_WIDTH, App::get().SCR_HEIGHT, "Rubix-CubeRenderer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    // VSYNC
    glfwSwapInterval(0);

    // GLFW callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD Load opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
    }

    glEnable(GL_DEPTH_TEST);

    // Shader program
    shaderProgram = new Shader("res/shaders/shader.vs", "res/shaders/shader.fs");

    // Generate Cubie Positions
    CubeRenderer::get().numCubes = 0;
    float min = -static_cast<float>(cubeSize) / 2;
    float max = static_cast<float>(cubeSize) / 2;
    for (float x = min; x < max; ++x) {
        for (float y = min; y < max; ++y) {
            for (float z = min; z < max; ++z) {
                if (x <= min || y <= min || z <= min || x >= max - 1 || y >= max - 1 || z >= max - 1) {
                    cubePositions.push_back(glm::vec3(static_cast<float>(x),
                                                      static_cast<float>(y),
                                                      static_cast<float>(z)));
                    ++CubeRenderer::get().numCubes;
                }
            }
        }
    }

    // Init CubeRenderer
    CubeRenderer::get().init();
}

void App::update() {
    // Update
    // -----------------------------------------------------
    open = !glfwWindowShouldClose(window);

    float currentFrame = glfwGetTime();
    App::get().deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);
    
    // Rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render
    shaderProgram->use();
    glm::mat4 projection = glm::perspective(glm::radians(App::get().camera.Zoom), (float)App::get().SCR_WIDTH / (float)App::get().SCR_HEIGHT, 0.1f, 10000.0f);
    shaderProgram->setMat4("projection", projection);
    glm::mat4 view = App::get().camera.GetViewMatrix();
    shaderProgram->setMat4("view", view);

    // Render cubes
    CubeRenderer::get().render();
     
    glfwSwapBuffers(window);
    glfwPollEvents();

    if (pollFPS) {
        frames++;

        // Most accurate fps display
        //std::cout << "fps: " << 1 / deltaTime << "\n";
        
        if (frames > 50) {
            fps = frames / (glfwGetTime() - sampleStartTime);
            std::cout << "fps: " << fps << "\n";
            sampleStartTime = glfwGetTime();
            frames = 0;
        }
    }
}

void App::enableVSYNC() {
    vsync = true;
    glfwSwapInterval(-1);
    std::cout << "vsync on" << "\n";
}
void App::disableVSYNC() {
    vsync = false;
    glfwSwapInterval(0);
    std::cout << "vsync off" << "\n";
}

// Destructor
App::~App() {
    delete shaderProgram;
    glfwTerminate();
}

// GLFW Window resize callback
// ---------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    App::get().SCR_WIDTH = width;
    App::get().SCR_HEIGHT = height;
}

// GLFW Input process
// ---------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        App::get().wireframe = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        App::get().wireframe = false;
    }
    if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
        if (!App::get().vsyncKeyDownUsed) {
            if (App::get().getVsyncState()) {
                App::get().disableVSYNC();
            }
            else {
                App::get().enableVSYNC();
            }
        }
        App::get().vsyncKeyDownUsed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_RELEASE) {
        App::get().vsyncKeyDownUsed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        App::get().camera.ProcessKeyboard(FORWARD, App::get().deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        App::get().camera.ProcessKeyboard(BACKWARD, App::get().deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        App::get().camera.ProcessKeyboard(LEFT, App::get().deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        App::get().camera.ProcessKeyboard(RIGHT, App::get().deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        CubeRenderer::get().angle.x -= 50 * App::get().deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        CubeRenderer::get().angle.x += 50 * App::get().deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        CubeRenderer::get().angle.y -= 50 * App::get().deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        CubeRenderer::get().angle.y += 50 * App::get().deltaTime;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    
    if (App::get().firstMouse) {
        App::get().lastX = xpos;
        App::get().lastY = ypos;
        App::get().firstMouse = false;
    }
  
    float xoffset = xpos - App::get().lastX;
    float yoffset = App::get().lastY - ypos; 
    App::get().lastX = xpos;
    App::get().lastY = ypos;

    App::get().camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    App::get().camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
