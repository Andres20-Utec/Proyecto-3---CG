//
// Created by hgallegos on 5/10/2022.
//

#ifndef LEARNOPENGL_OBJETO_H
#define LEARNOPENGL_OBJETO_H
#include "../include/glad/glad.h"
#include "../include/glm/gtx/string_cast.hpp"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <vector>
#include "shader_m.h"
#include "BoundingVolume.h"
using namespace std;
using namespace glm;


class Objeto {
public:
    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> textureCoords;
    vector<GLuint> indices;
    GLuint indices_size;
    vec3 color;
    float v0, x0, y0, z0, a0, xt, yt, zt;
    float tiempo_inicial = 0;
    GLuint vao;
    mat4 model;
    vec3 centro;
    vec3 velocidad;
    bool visible=true;
    BoundingSphere *bs;
    bool mueve = true;
    float escala = 20;
    GLint POSITION_ATTRIBUTE=0, NORMAL_ATTRIBUTE=1, TEXCOORD0_ATTRIBUTE=8;
    Objeto() {
        color = vec3(1, 1, 1);
        bs = new BoundingSphere;
    }
    virtual GLuint setup()=0;
    virtual void display(Shader &sh)=0;
    virtual void actualizarPosicion(float tiempo)=0;
    virtual void obtenerBS() {}
    virtual void actualizarBS() { };
};

class Esfera:public Objeto{
public:
    //vec3 centro;
    float radius;
    int slices, stacks;
    Esfera() {
        escala = 0.5;
        centro = vec3(0.0);
        color = vec3(1, 1, 1);
    }
    Esfera(vec3 _centro) {
        escala = 0.5;
        centro = _centro;
        color = vec3(1, 1, 1);
    }
    Esfera(vec3 _centro, float _radius, int _slices, int _stacks) {
        escala = 0.5;
        centro = _centro;
        radius = _radius;
        slices = _slices;
        stacks = _stacks;
        color = vec3(1, 1, 1);
    }

    GLuint setup();

    void display(Shader &sh);
    void actualizarPosicion(float tiempo);
    void actualizarBS();
};

class Plano: public Objeto{
public:
    vec3 normal;
    float d;
    Plano(vec3 _normal, float _d){ normal=_normal; d=_d; }
    void display(Shader &sh){
        model = mat4(1.0f);
        model = scale(model, vec3(escala));
        model = translate(model, centro);
        sh.setMat4("model", model);
    }
    void actualizarPosicion(float tiempo){}
    void actualizarBS() {}
};


#endif //LEARNOPENGL_OBJETO_H
