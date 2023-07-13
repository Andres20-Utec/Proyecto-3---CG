#ifndef GLUT_PLY_H
#define GLUT_PLY_H
#include "../include/glad/glad.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <vector>
#include "Objeto.h"
#include "shader_m.h"
using namespace std;
using namespace glm;


class Model_PLY: public Objeto {
public:
    Model_PLY();
    //vec3 centro;
    int		Load(char *filename);
    void    imprimir();
    int     enviar_GPU();
    GLuint  setup() { return enviar_GPU(); }
    void    display(Shader &sh);
    void    actualizarDatos(float t);
    void    actualizarPosicion(float tiempo) {}
    void    obtenerBS();
    void    actualizarBS();
};

#endif
