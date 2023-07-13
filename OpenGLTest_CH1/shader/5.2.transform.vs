#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform;
            // uniform : constante, hasta que se le cambie de shader o otra matriz
            //aPos: posición de los vértices | los vertices llegan uno a uno al vertex shader
            // matrix_model = mat4
void main()
{
	gl_Position = transform * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}