#pragma warning(disable:4996)
#pragma once
#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include <fstream>
#include <sstream>

float cubeVertices[] = {
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f
};
glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 captureViews[] = {
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

const char* vertexSource = "#version 330 core\n"
"layout (location=0) in vec3 aPos;\n"
"out vec3 localPos;\n"
"uniform mat4 P;\n"
"uniform mat4 V;\n"
"void main(){\n"
"localPos=aPos;\n"
"gl_Position=P*V*vec4(localPos,1.0);}\n";

const char* fragSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 localPos;\n"
"uniform sampler2D equirectangularMap;\n"
"const vec2 invAtan = vec2(0.1591, 0.3183);\n"
"vec2 SampleSphericalMap(vec3 v);\n"
"void main(){\n"
"vec2 uv = SampleSphericalMap(normalize(localPos));\n"
"vec3 color = texture(equirectangularMap, uv).rgb;\n"
"FragColor = vec4(color, 1.0);}\n"
"vec2 SampleSphericalMap(vec3 v){\n"
"vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n"
"uv *= invAtan;\n"
"uv += 0.5;\n"
"return uv;}\n";

void writeHDRCubemap(std::string srcPath, std::string outPath, int size) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(size, size, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW");
    }
    glViewport(0, 0, size, size);

    stbi_set_flip_vertically_on_load(true);
    int texWidthHDR, texHeightHDR, nrChannelsHDR;
    float* dataHDR = stbi_loadf(srcPath.c_str(), &texWidthHDR, &texHeightHDR, &nrChannelsHDR, 0);

    unsigned int textureHDR;
    glGenTextures(1, &textureHDR);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHDR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, texWidthHDR, texHeightHDR, 0, GL_RGB, GL_FLOAT, dataHDR);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(dataHDR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int HDRFramebuffer;
    glGenFramebuffers(1, &HDRFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, HDRFramebuffer);
    unsigned int HDRAttachment;
    glGenTextures(1, &HDRAttachment);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, HDRAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDRAttachment, 0);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);
    glBindVertexArray(cubeVAO);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "P"), 1, GL_FALSE, glm::value_ptr(captureProjection));
    glUniform1i(glGetUniformLocation(shaderProgram, "equirectangularMap"), 0);

    for (int i = 0; i < 6; i++) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "V"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, HDRFramebuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        float* pixels = new float[size * size * 3];
        glReadPixels(0, 0, size, size, GL_RGB, GL_FLOAT, pixels);
        stbi_write_hdr((outPath + "/" + std::to_string(i) + std::string(".hdr")).c_str(), size, size, 3, pixels);
    }
    glfwTerminate();
}