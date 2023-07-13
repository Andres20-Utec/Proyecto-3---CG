#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
//#include <learnopengl/filesystem.h>
#include "shader_m.h"
#include "../camera1.h"
#include "Objeto.h"
#include "glut_ply.h"
#include <iostream>
#include <fstream> // Lectura y escritura
using namespace std;

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

struct Triangle{
    vec3 point1;
    vec3 point2;
    vec3 point3;
    vec3 color;
};

// lighting
glm::vec3 lightPos(1.2f, 30.0f, 2.0f);

Esfera esfera(vec3(0),2., 20, 20);
Esfera *pEsfera = new Esfera(vec3(0),2, 50, 50);
Model_PLY modelo;
vector<Objeto*> objetos;
bool boton_presionado = false;

vec3 crossProduct(vec3 diff_ba, vec3 diff_ca){
    vec3 dir = vec3(0, 0, 0);
    dir.x = diff_ba.y * diff_ca.z - diff_ba.z * diff_ca.y;
    dir.y = -(diff_ba.x * diff_ca.z - diff_ba.z * diff_ca.x);
    dir.z = diff_ba.x * diff_ca.y - diff_ba.y * diff_ca.x;
    return dir;
}

void normalizeVec3(vec3 &v){
    float answer = sqrt((v.x * v.x + v.y * v.y + v.z * v.z));
    v.x = v.x / answer;
    v.y = v.y / answer;
    v.z = v.z / answer;
}

vec3 calcularNormal(vec3 a, vec3 b, vec3 c){
    vec3 diff_ba = b - a;
    vec3 diff_ca = c - a;
    vec3 dir = crossProduct(diff_ba, diff_ca);
    normalizeVec3(dir);
    return dir;
}

int countRows(string filename){
    int rows=0;
    ifstream file(filename);
    string line;
    while (getline(file, line))
        rows++;
    file.close();
    return rows - 1;
}

void lecturaMultiplesColumnas(float* array, string archivo){
    // Leer el archivo .csv
    ifstream fin; // creo el objeto para escritura
    //fin.open(path_+"\\"+ archivo); // abro el archivo
    fin.open(archivo); // Leo el archivo
    if(!fin.is_open()) throw runtime_error("Could not open file"); // Mensaje que me ayuda a saber si el archivo se a abierto
    vector<string> encabezado; // guardo el nombre de las columnas
    vector<vector<string>> rows;    // guardo el contenido en las columnas
    string palabra, linea; // variables serán usadas mas adelante
    getline(fin, linea);
    int i = 0;
    while(!fin.eof()){ // end of file -> final del archivo | eof -> retorna true o false, false -> no esta en el final del archivo
        // 24/02/2022,67751810,Andrea,F,65,00001,Toalla,3,40,120
        getline(fin, linea); // me ayuda a leer una linea del csv
        stringstream s(linea); // me ayuda a separar las palabras dentro de la linea
        while(getline(s, palabra, ',')){
            array[i] = stof(palabra);
            i++;
        }
    }
    fin.close();
}

int main() {
    //char *archivo = "../models/bunny.ply";
    //modelo.Load(archivo);
    srand(time(NULL));
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
    Shader lightingShader("../E5-Curvas/2.2.basic_lighting.vs", "../E5-Curvas/2.2.basic_lighting.fs");
    Shader lightCubeShader("../E5-Curvas/2.2.light_cube.vs", "../E5-Curvas/2.2.light_cube.fs");
    int n_vertices = 58806;
    // 11936988
    // 1806336

    string csvPath = "../E5-Curvas/vertices_60.csv";
    int rows = countRows(csvPath) * 6;
    float vertices[rows];
    lecturaMultiplesColumnas(vertices,  csvPath);

    /*
    vector<vector<vec3>> matriz;
    vector<vec3> puntos;
    for(float x=-5.0; x < 10; x += 1.0){
        vector<vec3> vec_puntos;
        for(float z = -5.0; z  < 10; z+= 1.0){
            //float y = 5*x*x + 3*x + 4;
            //float x4 = x*x;
            float y = (rand() % 7) - 3;
            vec_puntos.emplace_back(vec3(x,y,z));
            puntos.emplace_back(vec3(x,y,z));
        }
        matriz.emplace_back(vec_puntos);
    }
    GLuint vao_puntos;
    GLint POSITION_ATTRIBUTE=0;
    glGenVertexArrays( 1, &vao_puntos );
    glBindVertexArray( vao_puntos );
    GLuint vbos[1];
    glGenBuffers( 1, vbos );
    glBindBuffer( GL_ARRAY_BUFFER, vbos[0] );
    glBufferData( GL_ARRAY_BUFFER, puntos.size() * sizeof(vec3), puntos.data(), GL_STATIC_DRAW );
    glEnableVertexAttribArray( POSITION_ATTRIBUTE );
    // position attribute
    glVertexAttribPointer( POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );*/
    /*int numTriangles = 722;
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numTriangles * sizeof(Triangle), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    Triangle* trianglesData = static_cast<Triangle*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, numTriangles * sizeof(Triangle), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
    int i = 0;
    while(i < 12996){
        vec3 punto1 = vec3(vertices[i], vertices[i + 1], vertices[i+2]);
        vec3 punto2 = vec3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);
        vec3 punto3 = vec3(vertices[i + 12], vertices[i + 13], vertices[i+14]);
        trianglesData[i].point1 = punto1;
        trianglesData[i].point2 = punto2;
        trianglesData[i].point3 = punto3;
        trianglesData[i].color = vec3(0.0, 1.0, 0.0);
        i+=18;
    }*/

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    unsigned int VBO, cubeVAO; // VAO: Vertex Array Object - VBO: Vertex Buffer Object
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    int N_TRIANGLES = 20000000;
    //int N_TRIANGLES = 663166;
    //int N_TRIANGLES = 200704;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 0.0f, 1.0f);
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
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, N_TRIANGLES);
        //esfera.display(lightingShader);
        //pEsfera->display(lightingShader);
        /*cout << endl << currentFrame;
        for (auto &obj : objetos) {
            obj->actualizarPosicion(currentFrame);
            cout << "("<< obj->xt << "-" << obj->yt<<")";
            obj->display(lightingShader);
        }
        modelo.display(lightingShader);*/

        /*lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(vao_puntos);
        glDrawArrays(GL_POINTS,0, puntos.size());
        glBindVertexArray(0);*/

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
            float x = rand()%10;
            float y = rand()%10;
            float z = rand()%10;
            Objeto *pE = new Esfera(glm::vec3(x,y,z));
            pE->centro = vec3(x,y,z);
            pE->v0 = 20;
            pE->a0 = 50 + rand() % 20;
            pE->x0 = x;
            pE->y0 = y;
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

/*
    int extremo = matriz.size();
    int z = 0;
    for(int i = 0; i < matriz.size(); i++){
        for(int j = 0; j < matriz.size(); j++){
            int right = j + 1, left = j - 1, up = i - 1, down = i + 1;
             if(i < extremo - 1 && i >= 1 && j >= 1 && j < extremo - 1){
                puntos[z].y = (matriz[i][right].y + matriz[down][j].y + matriz[i+1][j+1].y +
                        matriz[i][left].y + matriz[up][j].y + matriz[i-1][j-1].y + matriz[i-1][j+1].y
                        + matriz[i+1][j-1].y) / 8.0f;
            }
             if(i == 0 && j == 0){
                 puntos[z].y = (matriz[i][right].y + matriz[down][j].y + matriz[i+1][j+1].y + matriz[i][j].y) / 4.0f;
             }
             if(i == 0 && j == extremo - 1){
                 puntos[z].y = (matriz[i][left].y + matriz[down][j].y + matriz[i+1][j-1].y + matriz[i][j].y) / 4.0f;
             }
             if(j > 0 && j < extremo - 1 && i == 0){
                 puntos[z].y = (matriz[i][left].y + matriz[down][j].y + matriz[i+1][j-1].y + matriz[i][right].y +
                          matriz[i+1][j+1].y) / 5.0f;
             }
            if(j == 0 && i < extremo - 1 && i > 0){
                puntos[z].y = (matriz[down][j].y + matriz[up][j].y  + matriz[i-1][j+1].y + matriz[i][right].y +
                                matriz[i-1][j+1].y) / 5.0f;
            }
            if(j == extremo - 1 && i < extremo - 1 && i > 0){
                puntos[z].y = (matriz[i][left].y + matriz[down][j].y + matriz[up][j].y + matriz[i+1][j-1].y  +
                                matriz[i-1][j-1].y) / 5.0f;
            }
            if(j > 0 && j < extremo - 1 && i == extremo - 1){
                puntos[z].y = (matriz[i][left].y + matriz[up][j].y + matriz[i-1][j+1].y + matriz[i][right].y +
                                matriz[i-1][j-1].y) / 5.0f;
            }

             if(j == 0 && i == extremo - 1){
                 puntos[z].y = (matriz[i][right].y + matriz[up][j].y + matriz[i-1][j+1].y + + matriz[i][j].y) / 4.0f;
             }
             if(i == extremo - 1 && j == extremo - 1){
                 puntos[z].y = (matriz[i][left].y + matriz[up][j].y + matriz[i-1][j-1].y + matriz[i][j].y) / 4.0f;
             }
            z++;
        }
    }
    float vertices[150];
    int idxPuntos = 0;
    int idxNormal = 3;
    int n_cols = extremo;
    int cantidadTriangulo = (matriz.size() - 1) * (matriz.size() - 1);
    for(int i = 0 ; i < puntos.size() - 5; i++){
        if(25 / (i + 1) == 0) continue;
        // Primer triangulo
        vec3 puntoInicial = puntos[i];
        vec3 puntoSiguiente = puntos[i + 1];
        vec3 puntoSiguienteFila = puntos[i + n_cols];
        vec3 normal1 = calcularNormal(puntoInicial, puntoSiguiente, puntoSiguienteFila);
        // Segundo triangulo
        vec3 puntoSiguienteFilaMasUno = puntos[i + n_cols + 1];
        vec3 normal2 = calcularNormal(puntoSiguiente, puntoSiguienteFila, puntoSiguienteFilaMasUno);

    }*/

/*for(float x=-10.0; x < 10.0; x+=0.1){
    for(float y=-10.0; y < 10.0; y+=0.1){
        float z = 5 - x*x - y*y;
        puntos.emplace_back(vec3(x,y,z));
    }
}*/