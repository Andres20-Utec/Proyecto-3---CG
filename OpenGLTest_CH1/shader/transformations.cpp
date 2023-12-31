#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "shader_s.h"
#include <random>
#include <iostream>
using namespace glm;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)  {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    Shader ourShader("../shader/5.1.transform.vs", "../shader/5.1.transform.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
            // positions
            0.5f,  0.5f, 0.5f, // top right
            0.5f, -0.5f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
            -0.5f,  0.5f, 0.0f // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3,  // second triangle
            0, 2, 1,
            2, 0, 3
    };
    float vertices2[] = {
            0.0f,    0.0f, 0.0f,
            0.0f,    0.5f, 0.0f,
            0.5f,    0.0f, 0.0f,
            0.25f,  0.25f, 0.5f
    };
    unsigned int indices2[] = {
            0, 1, 2,
            0, 1, 3,
            1, 2, 3,
            2, 0, 3
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    ourShader.use();
    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    unsigned int colorLoc = glGetUniformLocation(ourShader.ID, "color");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.9, 0.8);
    int N_FIGURES = 10;
    std::vector<std::pair<float, float>> pos;
    std::uniform_real_distribution<float> dist1(0.3, 0.8);
    std::vector<float> sizes_;
    std::vector<float> colores;
    for(int i = 0; i < N_FIGURES; i++){
        float f1 = dist(gen);
        float f2 = dist(gen);
        sizes_.push_back(dist1(gen));
        colores.push_back(dist1(gen));
        colores.push_back(dist1(gen));
        colores.push_back(dist1(gen));
        pos.push_back(std::make_pair(f1,f2));
    }

    // render loop
    while (!glfwWindowShouldClose(window)){
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(int i = 0; i < N_FIGURES; i++){
            // create transformations
            glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first - matriz identidad
            transform = glm::scale(transform, glm::vec3(sizes_[i]));
            transform = glm::translate(transform, glm::vec3(pos[i].first, pos[i].second, 0.0f));
            transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
            // get matrix's uniform location and set matrix
            ourShader.use();
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));// Usando las funciones de glfw
            glUniform3f(colorLoc,  colores[i * 3], colores[(i * 3) + 1], colores[(i + 3) + 2]);
            // render container
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        }


        // create transformations
        /*glm::mat4 transform2 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        transform2 = glm::translate(transform2, glm::vec3(0.5f, -0.5f, 0.0f));
        transform2 = glm::rotate(transform2, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        // get matrix's uniform location and set matrix
        ourShader.use();
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform2));
        // render container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);*/

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}