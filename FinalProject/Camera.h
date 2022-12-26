#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:
    // 相机参数
    glm::vec3 position = glm::vec3(5, 5, 20);    // 位置
    glm::vec3 direction = glm::vec3(0, 0, -1);  // 视线方向
    glm::vec3 up = glm::vec3(0, 1, 0);          // 上向量，固定(0,1,0)不变
    float pitch = 0.0f, roll = 0.0f, yaw = 0.0f;    // 欧拉角
    float fovy = 70.0f, aspect = 1.0, zNear = 0.01, zFar = 1000; // 透视投影参数
    float left = -1.0, right = 1.0, top = 1.0, bottom = -1.0; // 正交投影参数
    Camera() {};

    glm::mat4 getViewMatrix(bool IsEuler);
    glm::mat4 getProjectionMatrix(bool IsPerspective);
   
};