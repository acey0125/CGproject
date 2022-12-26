#pragma once
// glew glut
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
class Mesh
{
public:
    // OpenGL 对象
    GLuint vao, vbo, ebo;
    GLuint diffuseTexture;  // 漫反射纹理

    // 顶点属性
    std::vector<glm::vec3> vertexPosition;
    std::vector<glm::vec2> vertexTexcoord;
    std::vector<glm::vec3> vertexNormal;

    // glDrawElements 函数的绘制索引
    std::vector<int> index;

    Mesh() {};
    void bindData();
    void draw(GLuint program);
};