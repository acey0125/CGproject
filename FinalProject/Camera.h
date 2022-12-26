#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:
    // �������
    glm::vec3 position = glm::vec3(5, 5, 20);    // λ��
    glm::vec3 direction = glm::vec3(0, 0, -1);  // ���߷���
    glm::vec3 up = glm::vec3(0, 1, 0);          // ���������̶�(0,1,0)����
    float pitch = 0.0f, roll = 0.0f, yaw = 0.0f;    // ŷ����
    float fovy = 70.0f, aspect = 1.0, zNear = 0.01, zFar = 1000; // ͸��ͶӰ����
    float left = -1.0, right = 1.0, top = 1.0, bottom = -1.0; // ����ͶӰ����
    Camera() {};

    glm::mat4 getViewMatrix(bool IsEuler);
    glm::mat4 getProjectionMatrix(bool IsPerspective);
   
};