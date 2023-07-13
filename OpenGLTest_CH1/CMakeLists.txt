cmake_minimum_required(VERSION 3.22)
project(OpenGLTest)

set(CMAKE_CXX_STANDARD 17)

include_directories(${GLFW_INCLUDE_DIR})
link_libraries(${GLFW_LIBRARY})

#add_executable(OpenGLTest main1.cpp include/glad.c main.cpp)
#target_link_libraries(OpenGLTest -lOpenGL32 -lGLU32 -lglfw3)

add_executable(E5_Transformations shader/transformation.cpp shader/stb_image.cpp include/glad.c)
target_link_libraries(E5_Transformations -lOpenGL32 -lGLU32 -lglfw3)

add_executable(E6_Transformations shader/transformations2.cpp shader/stb_image.cpp include/glad.c)
target_link_libraries(E6_Transformations -lOpenGL32 -lGLU32 -lglfw3)

add_executable(Triangles shader/hello_triangle_indexed.cpp shader/stb_image.cpp include/glad.c)
target_link_libraries(Triangles -lOpenGL32 -lGLU32 -lglfw3)

add_executable(triangle shader/hello_triangle.cpp shader/stb_image.cpp include/glad.c)
target_link_libraries(triangle -lOpenGL32 -lGLU32 -lglfw3)

add_executable(Transformations shader/transformations.cpp shader/stb_image.cpp include/glad.c)
target_link_libraries(Transformations -lOpenGL32 -lGLU32 -lglfw3)

add_executable(E2_Iluminacion shader/basic_lighting_diffuse.cpp include/glad.c)
target_link_libraries(E2_Iluminacion -lOpenGL32 -lGLU32 -lglfw3)

add_executable(E3_Objetos shader/main.cpp include/glad.c shader/glut_ply.cpp shader/Objeto.cpp shader/BoundingVolume.h)
target_link_libraries(E3_Objetos -lOpenGL32 -lGLU32 -lglfw3)

add_executable(E5_Curves E5-Curvas/main.cpp include/glad.c E5-Curvas/glut_ply.cpp E5-Curvas/Objeto.cpp E5-Curvas/BoundingVolume.h)
target_link_libraries(E5_Curves -lOpenGL32 -lGLU32 -lglfw3)

add_executable(E6_Sombras Sombras/shadow_mapping_base.cpp include/glad.c Sombras/stb_image.cpp Sombras/shader.h)
target_link_libraries(E6_Sombras -lOpenGL32 -lGLU32 -lglfw3)