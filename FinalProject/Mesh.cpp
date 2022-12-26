#include "Mesh.h"

void Mesh::bindData()
{
    // ���������������
    glGenVertexArrays(1, &vao); // ����1�������������
    glBindVertexArray(vao);  	// �󶨶����������

    // ��������ʼ�����㻺����� ������NULL �Ȳ�������
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertexPosition.size() * sizeof(glm::vec3) +
        vertexTexcoord.size() * sizeof(glm::vec2) +
        vertexNormal.size() * sizeof(glm::vec3),
        NULL, GL_STATIC_DRAW);

    // ��λ��
    GLuint offset_position = 0;
    GLuint size_position = vertexPosition.size() * sizeof(glm::vec3);
    glBufferSubData(GL_ARRAY_BUFFER, offset_position, size_position, vertexPosition.data());
    glEnableVertexAttribArray(0);   // ��ɫ���� (layout = 0) ��ʾ����λ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(offset_position));
    // ����������
    GLuint offset_texcoord = size_position;
    GLuint size_texcoord = vertexTexcoord.size() * sizeof(glm::vec2);
    glBufferSubData(GL_ARRAY_BUFFER, offset_texcoord, size_texcoord, vertexTexcoord.data());
    glEnableVertexAttribArray(1);   // ��ɫ���� (layout = 1) ��ʾ��������
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(offset_texcoord));
    // ������
    GLuint offset_normal = size_position + size_texcoord;
    GLuint size_normal = vertexNormal.size() * sizeof(glm::vec3);
    glBufferSubData(GL_ARRAY_BUFFER, offset_normal, size_normal, vertexNormal.data());
    glEnableVertexAttribArray(2);   // ��ɫ���� (layout = 2) ��ʾ����
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(offset_normal));

    // �������� ebo
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), index.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::draw(GLuint program)
{
    glBindVertexArray(vao);

    // ������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glUniform1i(glGetUniformLocation(program, "texture"), 0);

    // ����
    glDrawElements(GL_TRIANGLES, this->index.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
