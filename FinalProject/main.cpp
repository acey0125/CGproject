/*
Final project for computer graphics class 2022 autumn
*/
#include <fstream>
#include "Model.h"
#include "Camera.h"
#include "ShadowAndTex.h"
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <windows.h>


#define WindowWidth 2200
#define WindowHeight 1500
#define BITMAP_ID 0x4D42
using namespace std;
//����ģ�ͻ���
Model box;
GLuint boxTexture;


//ģ����������պ�
vector<Model> models;  
Model skybox; 


//��ɫ������
GLuint ModelRender;
GLuint ShadowRender;
GLuint SkyBoxRender; 

//��պ���������Ӱ����
GLuint skyboxTexture;   
GLuint shadowTexture;  

//���
Camera camera;      
Camera shadowCamera;  
GLFWwindow* m_Window;
glm::vec3 lightColor;
glm::vec3 movMatrix ={0.0,0.0,0.0};
glm::vec3 tmpMatrix = { 0.0,0.0,0.0 };

glm::vec3 rotate1 = { -90,0.0,90 };
glm::vec3 scale1 = { 1.0,1.0,1.0 };
glm::vec3 rotate2 = { -90,0.0,90 };
glm::vec3 scale2 = { 1.0,1.0,1.0 };

GLdouble modelview[16];
GLdouble projection[16];
GLint viewport[4];
GLdouble poix, poiy, poiz, z;
int thisModel;
bool Isgoal = false;
bool Iscubic = true;
bool Istai = true;
bool Isball = true;
bool firstMouse = true;
float lastX = 0, lastY = 0;
int shadowMapResolution = 1024;   
float cameraSpeed = 0.035f;
int counter0 = 0;
int counter1 = 0;
int counter2 = 0;
GLuint shadowMapFBO;//֡����


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// �����ֺ���
void mouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.fovy -= xoffset;
}
bool SnapScreen(int width, int height, const char* file)
{
    unsigned char* image;          //����ͼ������
    FILE* fp;             //�ļ�ָ��
    BITMAPFILEHEADER FileHeader;     //����λͼ�ļ�ͷ
    BITMAPINFOHEADER InfoHeader;     //����λͼ��Ϣͷ

    FileHeader.bfType = BITMAP_ID;                                                  //ID����Ϊλͼ��id��
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);       //ʵ��ͼ�����ݵ�λ�����ļ�ͷ����Ϣͷ֮��
    FileHeader.bfReserved1 = 0;                                                     //��������Ϊ0
    FileHeader.bfReserved2 = 0;                                                     //��������Ϊ0
    FileHeader.bfSize = height * width * 24 + FileHeader.bfOffBits;                       //BMPͼ���ļ���С

    InfoHeader.biXPelsPerMeter = 0;                                               //ˮƽ�ֱ��ʣ�������ʱ��Ϊ0����
    InfoHeader.biYPelsPerMeter = 0;                                               //��ֱ�ֱ��ʣ�������ʱ��Ϊ0����
    InfoHeader.biClrUsed = 0;                                                     //ͼ��ʹ�õ���ɫ��������ʱ��Ϊ0����
    InfoHeader.biClrImportant = 0;                                                //��Ҫ����ɫ����������ʱ��Ϊ0����                         //��ֱ�ֱ��ʣ�������ʱ��Ϊ0����
    InfoHeader.biPlanes = 1;														  //��������Ϊ1
    InfoHeader.biCompression = BI_RGB;                                              //����ΪBI_RGBʱ,��ʾͼ��û�в�ɫ��
    InfoHeader.biBitCount = 24;                                                     //ͼ���λ��
    InfoHeader.biSize = sizeof(BITMAPINFOHEADER);                                   //�ṹ��Ĵ�С
    InfoHeader.biHeight = height;
    InfoHeader.biWidth = width;
    InfoHeader.biSizeImage = height * width * 4;

    image = (unsigned char*)malloc(sizeof(unsigned char) * InfoHeader.biSizeImage);
    if (image == NULL)
    {
        free(image);
        printf("Exception: No enough space!\n");
        return false;
    }
    //���ظ�ʽ����4�ֽڶ���
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //���ճ����ص�����
    glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image);

    fp = fopen(file, "wb");
    if (fp == NULL)
    {
        printf("Exception: Fail to open file!\n");
        return false;
    }
    fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
    fwrite(image, InfoHeader.biSizeImage, 1, fp);
    free(image);
    fclose(fp);
    return true;
}
void MoveView()
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Window, true);
    //���λ��
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.position += cameraSpeed * camera.direction;
        movMatrix += cameraSpeed * camera.direction;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.position -= cameraSpeed * camera.direction;
        movMatrix -= cameraSpeed * camera.direction;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
    {
         camera.position -= cameraSpeed * glm::normalize(glm::cross(camera.direction, camera.up));
         movMatrix -= cameraSpeed * glm::normalize(glm::cross(camera.direction, camera.up));
    }

    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.position += cameraSpeed * glm::normalize(glm::cross(camera.direction, camera.up));
        movMatrix += cameraSpeed * glm::normalize(glm::cross(camera.direction, camera.up));
    }

    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.position.y += cameraSpeed;
        movMatrix.y += cameraSpeed;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.position.y -= cameraSpeed;
        movMatrix.y -= cameraSpeed;
    }

    //��Դλ��
    if (glfwGetKey(m_Window, GLFW_KEY_J) == GLFW_PRESS)
        shadowCamera.position.x += cameraSpeed;
    if (glfwGetKey(m_Window, GLFW_KEY_L) == GLFW_PRESS)
        shadowCamera.position.x -= cameraSpeed;
    if (glfwGetKey(m_Window, GLFW_KEY_K) == GLFW_PRESS)
        shadowCamera.position.z -= cameraSpeed;
    if (glfwGetKey(m_Window, GLFW_KEY_I) == GLFW_PRESS)
        shadowCamera.position.z += cameraSpeed;
    if (glfwGetKey(m_Window, GLFW_KEY_U) == GLFW_PRESS)
        shadowCamera.position.y += cameraSpeed;
    if (glfwGetKey(m_Window, GLFW_KEY_O) == GLFW_PRESS)
        shadowCamera.position.y -= cameraSpeed;
    if (glfwGetKey(m_Window, GLFW_KEY_P) == GLFW_PRESS)
        SnapScreen(WindowWidth, WindowHeight, "screen.bmp");

    if (glfwGetKey(m_Window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (counter0 > 50)
        {
            models[0].show = !models[0].show;
            models[1].show = !models[1].show;
            counter0 = 0;
        }
        else counter0++;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (counter1 > 50)
        {
            models[2].show = !models[2].show;
            models[3].show = !models[3].show;
            counter1 = 0;
        }
        else counter1++;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (counter2 > 50)
        {
            models[6].show = !models[6].show;
            models[7].show = !models[7].show;
            counter2 = 0;
        }
        else counter2++;
    }
}


void Initialization()
{
    // ������ɫ���������
    ModelRender = getShaderProgram("shaders/fshader.fsh", "shaders/vshader.vsh");
    ShadowRender = getShaderProgram("shaders/shadow.fsh", "shaders/shadow.vsh");
    SkyBoxRender = getShaderProgram("shaders/skybox.fsh", "shaders/skybox.vsh");

    //��1-0
    Model tree1 = Model(0);
    tree1.translate = glm::vec3(2.5, -0.4, 2);
    tree1.scale = glm::vec3(0.0025, 0.0025, 0.0025);
    tree1.load("models/tree/tree02.obj");
    tree1.Change(0.0025, 0.0025, 0.0025, 2.5, -0.4, 2);
    models.push_back(tree1);
    //��2-1
    Model tree2 = Model(1);
    tree2.translate = glm::vec3(2.5, -0.4, 2);
    tree2.scale = glm::vec3(0.0025, 0.0025, 0.0025);
    tree2.load("models/tree/tree01.obj");
    tree2.Change(0.0025, 0.0025, 0.0025, 2.5, -0.4, 2);
    tree2.show = false;
    models.push_back(tree2);
    //��3-2
    Model tree3 = Model(2);
    tree3.translate = glm::vec3(10, -0.4, 7);
    tree3.scale = glm::vec3(0.0015, 0.0015, 0.0015);
    tree3.load("models/tree/tree02.obj");
    tree3.Change(0.0015, 0.0015, 0.0015, 10, -0.4, 7);
    models.push_back(tree3);
    //��4-3
    Model tree4 = Model(3);
    tree4.translate = glm::vec3(10, -0.4, 7);
    tree4.scale = glm::vec3(0.0015, 0.0015, 0.0015);
    tree4.load("models/tree/tree01.obj");
    tree4.Change(0.0015, 0.0015, 0.0015, 10, -0.4, 7);
    tree4.show = false;
    models.push_back(tree4);
    //����-4
    Model plane = Model(4);
    plane.translate = glm::vec3(0, -8.4, 0);
    plane.scale = glm::vec3(1, 1, 1);
    plane.rotate = glm::vec3(0, 0, 0);
    plane.load("models/ground/grass.obj");
    models.push_back(plane);
    //����
    Model goal = Model(5);
    goal.translate = glm::vec3(2.5, 0, 0);
    goal.rotate = glm::vec3(-90, 0, 90);
    goal.scale = glm::vec3(0.2, 0.2, 0.2);
    goal.load("models/goal/goal/goal.obj");
    models.push_back(goal);

    //��-6
    Model tree = Model(6);
    tree.translate = glm::vec3(18, 0, 15);
    tree.scale = glm::vec3(0.01, 0.01, 0.01);
    tree.load("models/tree/file.obj");
    tree.Change(0.01, 0.01, 0.01, 18, 0, 15);
    models.push_back(tree);

    //������-7
    Model trees = Model(7);
    trees.translate = glm::vec3(18, 0, 15);
    trees.scale = glm::vec3(0.01, 0.01, 0.01);
    trees.load("models/tree/file1.obj");
    trees.Change(0.01, 0.01, 0.01, 18, 0, 15);
    trees.show = false;
    models.push_back(trees);


    // ��Դ��־
    Model vlight = Model(8);
    vlight.translate = glm::vec3(1, 0, -1);
    vlight.rotate = glm::vec3(225, 0, 0);
    vlight.scale = glm::vec3(0.03, 0.03, 0.03);
    vlight.load("models/camera/camera.obj");
    models.push_back(vlight);













    //��պ�������
    Mesh cube;
    cube.vertexPosition = { 
        glm::vec3(-1, -1, -1),glm::vec3(1, -1, -1),glm::vec3(-1, 1, -1),glm::vec3(1, 1, -1),
        glm::vec3(-1, -1, 1),glm::vec3(1, -1, 1),glm::vec3(-1, 1, 1),glm::vec3(1, 1, 1)
    };
    cube.index = { 0,3,1,0,2,3,1,5,4,1,4,0,4,2,0,4,6,2,5,6,4,5,7,6,2,6,7,2,7,3,1,7,5,1,3,7 };
    cube.bindData();
    skybox.meshes.push_back(cube);

    //��������ͼ
    vector<const GLchar*> faces;
    faces.push_back("skybox/right.jpg");
    faces.push_back("skybox/left.jpg");
    faces.push_back("skybox/top.jpg");
    faces.push_back("skybox/bottom.jpg");
    faces.push_back("skybox/back.jpg");
    faces.push_back("skybox/front.jpg");
    skyboxTexture = TextureMap(faces);

    //��ԴͶӰ
    shadowCamera.left = -20;
    shadowCamera.right = 20;
    shadowCamera.bottom = -20;
    shadowCamera.top = 20;
    shadowCamera.position = glm::vec3(0, 15, 15);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glGenFramebuffers(1, &shadowMapFBO);
    //��Ӱ����
    glGenTextures(1, &shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // ��Ӱ����󶨵�֡����
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //������Ȳ���
    glEnable(GL_DEPTH_TEST);  
}


void display()
{
    while (!glfwWindowShouldClose(m_Window))
    {
       
        Model cube = Model(9);
        if (Iscubic == true)
            tmpMatrix = movMatrix;
        cube.translate = glm::vec3(5, 2, 2) + tmpMatrix;
        cube.rotate = rotate1;
        cube.scale = scale1;
        cube.load("models/basic/cube.obj");
        models.push_back(cube);

        Model tai = Model(10);
        if (Istai == true)
            tmpMatrix = movMatrix;
        tai.translate = glm::vec3(5, 2, 0) + tmpMatrix;
        tai.rotate = rotate2;
        tai.scale = scale2;
        tai.load("models/basic/tai.obj");
        models.push_back(tai);



        glfwPollEvents();    
        ImGui_ImplGlfwGL3_NewFrame();

        MoveView();

        //��ģ������Ϊ���ղ��յ�
        models[8].translate = shadowCamera.position + glm::vec3(0, 0, 2);
        //��Ӱ��Ⱦ
        glUseProgram(ShadowRender);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, shadowMapResolution, shadowMapResolution);

        shadowCamera.direction = glm::normalize(glm::vec3(0, 0, 0) - shadowCamera.position);
        glUniformMatrix4fv(glGetUniformLocation(ShadowRender, "view"), 1, GL_FALSE, glm::value_ptr(shadowCamera.getViewMatrix(false)));
        glUniformMatrix4fv(glGetUniformLocation(ShadowRender, "projection"), 1, GL_FALSE, glm::value_ptr(shadowCamera.getProjectionMatrix(false)));
        for (auto m : models)
        {
            if (m.show)
            {
                if (m.id == 5 && Isgoal == true)
                    continue;
                else if (m.id == 9 && Iscubic == true)
                    continue;
                else if (m.id == 10 && Istai == true)
                    continue;
                else if (m.id == 11 && Isball == true)
                    continue;
                else m.draw(ModelRender);
            }
        }

        //��պ���Ⱦ
        glUseProgram(SkyBoxRender);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WindowWidth, WindowHeight);
        glUniformMatrix4fv(glGetUniformLocation(SkyBoxRender, "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix(true)));
        glUniformMatrix4fv(glGetUniformLocation(SkyBoxRender, "projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix(true)));
        //��պ�����
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glUniform1i(glGetUniformLocation(SkyBoxRender, "skybox"), 1);
        skybox.translate = camera.position;
        //�ر����
        glDepthMask(GL_FALSE);
        skybox.draw(SkyBoxRender);
        glDepthMask(GL_TRUE);

        //����Ⱦ
        glUseProgram(ModelRender);
        glUniformMatrix4fv(glGetUniformLocation(ModelRender, "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix(true)));
        glUniformMatrix4fv(glGetUniformLocation(ModelRender, "projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix(true)));
        glm::mat4 shadowVP = shadowCamera.getProjectionMatrix(false) * shadowCamera.getViewMatrix(false);
        glUniformMatrix4fv(glGetUniformLocation(ModelRender, "shadowVP"), 1, GL_FALSE, glm::value_ptr(shadowVP));
        //��������
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowTexture);
        glUniform1i(glGetUniformLocation(ModelRender, "shadowtex"), 1);

        //��Դλ��
        glUniform3fv(glGetUniformLocation(ModelRender, "lightPos"), 1, glm::value_ptr(shadowCamera.position));
        //���λ��
        glUniform3fv(glGetUniformLocation(ModelRender, "cameraPos"), 1, glm::value_ptr(camera.position));
        //��Դ��ɫ
        glUniform3fv(glGetUniformLocation(ModelRender, "lightColor"), 1, glm::value_ptr(lightColor));
        //����
        for (auto m : models)
        {
            if (m.show)
            {
                if (m.id == 5 && Isgoal == true)
                    continue;
                else if (m.id == 9 && Iscubic == true)
                    continue;
                else if (m.id == 10 && Istai == true)
                    continue;
                else if (m.id == 11 && Isball == true)
                    continue;
                else m.draw(ModelRender);
            }
        }
          
        {
            ImGui::Text("This is debug window for application");
            ImGui::SliderFloat("LightX", &shadowCamera.position.x, 0.0f, 15.0f);
            ImGui::SliderFloat("LightY", &shadowCamera.position.y, 0.0f, 15.0f);
            ImGui::SliderFloat("LightZ", &shadowCamera.position.z, 0.0f, 15.0f);
            ImGui::ColorEdit3("Light color", (float*)&lightColor);
            ImGui::Checkbox("Goal", &Isgoal);
            ImGui::Checkbox("Cubic", &Iscubic);
            ImGui::SliderFloat("rotate1x", &rotate1.x, -90, 90);
            ImGui::SliderFloat("rotate1y", &rotate1.y, -90, 90);
            ImGui::SliderFloat("rotate1z", &rotate1.z, -90, 90);
            ImGui::SliderFloat("scale1x", &scale1.x, 0.5, 3);
            ImGui::SliderFloat("scale1y", &scale1.y, 0.5, 3);
            ImGui::SliderFloat("scale1z", &scale1.z, 0.5, 3);            
            ImGui::Checkbox("Tai", &Istai);
            ImGui::SliderFloat("rotate2x", &rotate2.x, -90, 90);
            ImGui::SliderFloat("rotate2y", &rotate2.y, -90, 90);
            ImGui::SliderFloat("rotate2z", &rotate2.z, -90, 90);
            ImGui::SliderFloat("scale2x", &scale2.x, 0.5, 3);
            ImGui::SliderFloat("scale2y", &scale2.y, 0.5, 3);
            ImGui::SliderFloat("scale2z", &scale2.z, 0.5, 3);

            ImGui::Checkbox("Ball", &Isball);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("pitch:",camera.pitch, "yaw:",camera.yaw,"roll:", camera.roll);
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(m_Window);  //˫����   
        models.pop_back();
        models.pop_back();
    }
}
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        for (int i = 0; i < models.size(); i++)
        {
            glGetIntegerv(GL_VIEWPORT, viewport);
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
            glGetDoublev(GL_PROJECTION_MATRIX, projection);
            glReadPixels(xpos, viewport[3] - ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
            glm::mat4x4 model;
            for (int j = 0; j < 4; j++)
            {
              for (int i = 0; i < 4; i++)
                {
                    model[j][i] = modelview[4*j + i];
                }
            }
            glm::mat4x4 pro;
            for (int j = 0; j < 4; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    pro[j][i] = projection[4 * j + i];
                }
            }
            glm::vec4 view;
            for (int j = 0; j < 4; j++)
            {
                view[j] = viewport[j];
            }
            glm::vec3 win(xpos, viewport[3] - ypos, z);
            glm::vec3 trans = glm::unProject(win, model, pro, view);
            poix = trans.x;
            poiy = trans.y;
            poiz = trans.z;
            if (poix + camera.position.x <= models[i].maxX * camera.fovy / 70 && poix + camera.position.x >= models[i].minX * camera.fovy / 70 && poiy + camera.position.y <= models[i].maxY * camera.fovy / 70 && poiy + camera.position.y >= models[i].minY * camera.fovy / 70) {
                if (i != 4 && i != 8) {
                    //cout << i << endl;
                    models[i].show = !models[i].show;
                }
            }

        }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    camera.yaw += 10 * (xpos - lastX) / WindowWidth;
    camera.yaw = glm::mod(camera.yaw + 180.0f, 360.0f) - 180.0f;
    //movMatrix += glm::vec3(cos(glm::mod(camera.yaw + 180.0f, 360.0f) - 180.0f),0,-sin(glm::mod(camera.yaw + 180.0f, 360.0f) - 180.0f));

    camera.pitch += -10 * (ypos - lastY) / WindowHeight;
    camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);
        
}

int main(int argc, char** argv)
{
    
    if (!glfwInit())
        std::cout << "GlfwInit Error" << std::endl;
    string name = "FinalProject";
    m_Window = glfwCreateWindow(WindowWidth, WindowHeight, name.c_str(), NULL, NULL);
    glfwSetWindowPos(m_Window, 300, 200);
    if (!m_Window)
    {
        glfwTerminate();
        std::cout << "Window Error" << std::endl;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_Window, MouseCallback);
    glfwSetScrollCallback(m_Window, mouseWheel);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }

    Initialization();

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(m_Window, true);
    ImGui::StyleColorsDark();

    display();

    return 0;
}

