#include "Camera.h"

glm::mat4 Camera::getViewMatrix(bool IsEuler)
{
    if (IsEuler)  // 使用欧拉角更新相机朝向
    {
        direction.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw)); // 相机看向z轴负方向
    }
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::getProjectionMatrix(bool IsPerspective)
{
    if (IsPerspective) // 透视投影
    {
        return glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
    }
    return glm::ortho(left, right, bottom, top, zNear, zFar);
}
