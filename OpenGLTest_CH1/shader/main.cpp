#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../include/glm/gtx/string_cast.hpp"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
//#include <learnopengl/filesystem.h>
#include "shader_m.h"
#include "../camera.h"
#include "Objeto.h"
#include "glut_ply.h"
#include <iostream>
#include <random>
using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float tiempoInicial = 0.0f, tiempoTranscurrido = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 30.0f, 2.0f);

Esfera esfera(vec3(0),2., 20, 20);
Esfera *pEsfera = new Esfera(vec3(0),2, 50, 50);
Model_PLY modelo;
vector<Objeto*> objetos;
bool boton_presionado = false;

int main() {

    Esfera *esferaEstatica = new Esfera(glm::vec3(15, 15, 15), 3, 25, 25);
    esferaEstatica->x0  = 15;
    esferaEstatica->y0  = 15;
    esferaEstatica->z0  = 15;
    esferaEstatica->vao = esfera.vao;
    esferaEstatica->radius = 2;
    esferaEstatica->color = vec3(1,0,0);
    esferaEstatica->indices_size = esfera.indices_size;
    esferaEstatica->actualizarBS();
    char *archivo = "../models/bunny.ply";
    modelo.Load(archivo);

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
    if (window == NULL)     {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))     {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader lightingShader("../shader/2.2.basic_lighting.vs", "../shader/2.2.basic_lighting.fs");
    //Shader lightCubeShader("../2.2.light_cube.vs", "../2.2.light_cube.fs");

    esfera.setup();
    modelo.setup();
    //pEsfera->setup();
    pEsfera->vao = esfera.vao;
    modelo.obtenerBS();
    pEsfera->centro = esfera.bs->centro;
    pEsfera->radius = esfera.bs->radio;
    pEsfera->escala = esfera.escala;
    pEsfera->mueve = false;
    objetos.emplace_back(pEsfera);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //tiempoTranscurrido = currentFrame - tiempoInicial; //static_cast<float>(glfwGetTime());
        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        //lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);


        esferaEstatica->display(lightingShader);

        //esfera.display(lightingShader);
        //pEsfera->display(lightingShader);
        //cout << endl << currentFrame;
        for (auto &obj : objetos) {
            lightingShader.setVec3("objectColor", obj->color.x, obj->color.y, obj->color.z);
            obj->actualizarPosicion(currentFrame);
            if(obj->bs->intersecta(*esferaEstatica->bs)){
                cout << "INTERSECTA" << endl;
            }
            //cout << "("<< obj->xt << "-" << obj->yt<<")";
            obj->display(lightingShader);
        }
        modelo.display(lightingShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    //   glDeleteVertexArrays(1, &cubeVAO);
    //   glDeleteVertexArrays(1, &lightCubeVAO);
    //   glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        boton_presionado = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE){
        if (boton_presionado) {
            std::random_device rd;  // Obtener una semilla aleatoria del dispositivo
            std::mt19937 gen(rd()); // Generador Mersenne Twister con la semilla aleatoria
            std::uniform_real_distribution<double> dis(0.0, 1.0);
            float x = camera.Front.x, y = camera.Front.y, z = camera.Front.z;
            Objeto *pE = new Esfera(glm::vec3(x,y,z));
            Esfera* esferaT = dynamic_cast<Esfera*> (pE);
            esferaT->radius = esfera.radius;
            pE->color.x = dis(gen);
            pE->color.y = dis(gen);
            pE->color.z = dis(gen);
            pE->centro = vec3(x,y,z);
            pE->v0 = 20;
            pE->velocidad.x = 50 * x;
            pE->velocidad.y = 50 * y;
            pE->velocidad.z = 150 * z;
            pE->a0 = 50 + rand() % 20;
            pE->x0 = x;
            pE->y0 = y;
            pE->z0 = z;
            pE->vao = esfera.vao;
            pE->indices_size = esfera.indices_size;
            pE->tiempo_inicial = static_cast<float>(glfwGetTime());
            objetos.emplace_back(pE);
            boton_presionado = false;
            //cout << endl << x << " " << y << " " << z << " " << pE->a0;

        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}