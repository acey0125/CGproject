#include "Camera.h"

glm::mat4 Camera::getViewMatrix(bool IsEuler)
{
    if (IsEuler)  // ʹ��ŷ���Ǹ����������
    {
        direction.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw)); // �������z�Ḻ����
    }
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::getProjectionMatrix(bool IsPerspective)
{
    if (IsPerspective) // ͸��ͶӰ
    {
        return glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
    }
    return glm::ortho(left, right, bottom, top, zNear, zFar);
}
