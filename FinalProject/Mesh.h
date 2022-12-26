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
    // OpenGL ����
    GLuint vao, vbo, ebo;
    GLuint diffuseTexture;  // ����������

    // ��������
    std::vector<glm::vec3> vertexPosition;
    std::vector<glm::vec2> vertexTexcoord;
    std::vector<glm::vec3> vertexNormal;

    // glDrawElements �����Ļ�������
    std::vector<int> index;

    Mesh() {};
    void bindData();
    void draw(GLuint program);
};