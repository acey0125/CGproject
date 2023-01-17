/*
Final project for computer graphics class 2022 autumn
*/
#include <fstream>
#include "Model.h"
#include "Camera.h"
#include "ShadowAndTex.h"
#include "VideoRecorder.h"
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <windows.h>


#define WindowWidth 2200
#define WindowHeight 1500
#define BITMAP_ID 0x4D42
using namespace std;
//基本模型绘制
Model box;
GLuint boxTexture;


//模型数组与天空盒
vector<Model> models;  
Model skybox; 


//着色器程序
GLuint ModelRender;
GLuint ShadowRender;
GLuint SkyBoxRender; 

//天空盒纹理与阴影纹理
GLuint skyboxTexture;   
GLuint shadowTexture;  

//相机
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
glm::vec3 rotate3 = { -90,0.0,90 };
glm::vec3 scale3 = { 0.2,0.2,0.2 };

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
bool isRecording = false;
bool isRecordOver = false;
int Isfly = 0;
int choose = 0;
float lastX = 0, lastY = 0;
int shadowMapResolution = 1024;   
float cameraSpeed = 0.5f;
float upspeed = 0.0f;
float downspeed = -1.0f;
int vcounter = 0;
int counter0 = 0;
int counter1 = 0;
int counter2 = 0;
GLuint shadowMapFBO;//帧缓冲


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// 鼠标滚轮函数
void mouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.fovy -= xoffset;
}
bool SnapScreen(int width, int height, const char* file)
{
    unsigned char* image;          //接受图像数据
    FILE* fp;             //文件指针
    BITMAPFILEHEADER FileHeader;     //接受位图文件头
    BITMAPINFOHEADER InfoHeader;     //接受位图信息头

    FileHeader.bfType = BITMAP_ID;                                                  //ID设置为位图的id号
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);       //实际图像数据的位置在文件头和信息头之后
    FileHeader.bfReserved1 = 0;                                                     //必须设置为0
    FileHeader.bfReserved2 = 0;                                                     //必须设置为0
    FileHeader.bfSize = height * width * 24 + FileHeader.bfOffBits;                       //BMP图像文件大小

    InfoHeader.biXPelsPerMeter = 0;                                               //水平分辨率，这里暂时设为0就是
    InfoHeader.biYPelsPerMeter = 0;                                               //垂直分辨率，这里暂时设为0就是
    InfoHeader.biClrUsed = 0;                                                     //图像使用的颜色，这里暂时设为0就是
    InfoHeader.biClrImportant = 0;                                                //重要的颜色数，这里暂时设为0就是                         //垂直分辨率，这里暂时设为0就是
    InfoHeader.biPlanes = 1;														  //必须设置为1
    InfoHeader.biCompression = BI_RGB;                                              //设置为BI_RGB时,表示图像并没有彩色表
    InfoHeader.biBitCount = 24;                                                     //图像的位数
    InfoHeader.biSize = sizeof(BITMAPINFOHEADER);                                   //结构体的大小
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
    //像素格式设置4字节对齐
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //接收出像素的数据
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

VideoRecorder recorder;
void idle() {
    if (!isRecordOver && isRecording)
    {// 开始录像
        isRecordOver = isRecording;
        recorder.RecordInit(60, WindowWidth, WindowHeight, "record.mp4");
    }
    else if (isRecordOver && isRecording)
    {// 录像
        recorder.GrabFrame();
    }
    else if (isRecordOver && !isRecording)
    {// 结束录像
        isRecordOver = isRecording;
        recorder.Finish();
    }
}

void MoveView()
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Window, true);
    if (glfwGetKey(m_Window, GLFW_KEY_V) == GLFW_PRESS)
    {
        if (vcounter > 10)
        {        
            isRecording = !isRecording;
            vcounter = 0;
        }
        else vcounter++;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        upspeed = 5.0f;
    }
    //相机位置
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

    //光源位置
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
        if (counter0 > 20)
        {
            models[0].show = !models[0].show;
            models[1].show = !models[1].show;
            counter0 = 0;
        }
        else counter0++;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (counter1 > 20)
        {
            models[2].show = !models[2].show;
            models[3].show = !models[3].show;
            counter1 = 0;
        }
        else counter1++;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (counter2 > 20)
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
    // 生成着色器程序对象
    ModelRender = getShaderProgram("shaders/fshader.fsh", "shaders/vshader.vsh");
    ShadowRender = getShaderProgram("shaders/shadow.fsh", "shaders/shadow.vsh");
    SkyBoxRender = getShaderProgram("shaders/skybox.fsh", "shaders/skybox.vsh");

    //树1-0
    Model tree1 = Model(0);
    tree1.translate = glm::vec3(2.5, -0.4, 2);
    tree1.scale = glm::vec3(0.0025, 0.0025, 0.0025);
    tree1.load("models/tree/tree02.obj");
    tree1.Change(0.0025, 0.0025, 0.0025, 2.5, -0.4, 2);
    models.push_back(tree1);
    //树2-1
    Model tree2 = Model(1);
    tree2.translate = glm::vec3(2.5, -0.4, 2);
    tree2.scale = glm::vec3(0.0025, 0.0025, 0.0025);
    tree2.load("models/tree/tree01.obj");
    tree2.Change(0.0025, 0.0025, 0.0025, 2.5, -0.4, 2);
    tree2.show = false;
    models.push_back(tree2);
    //树3-2
    Model tree3 = Model(2);
    tree3.translate = glm::vec3(10, -0.4, 7);
    tree3.scale = glm::vec3(0.0015, 0.0015, 0.0015);
    tree3.load("models/tree/tree02.obj");
    tree3.Change(0.0015, 0.0015, 0.0015, 10, -0.4, 7);
    models.push_back(tree3);
    //树4-3
    Model tree4 = Model(3);
    tree4.translate = glm::vec3(10, -0.4, 7);
    tree4.scale = glm::vec3(0.0015, 0.0015, 0.0015);
    tree4.load("models/tree/tree01.obj");
    tree4.Change(0.0015, 0.0015, 0.0015, 10, -0.4, 7);
    tree4.show = false;
    models.push_back(tree4);
    //地面-4
    Model plane = Model(4);
    plane.translate = glm::vec3(0, -8.4, 0);
    plane.scale = glm::vec3(1, 1, 1);
    plane.rotate = glm::vec3(0, 0, 0);
    plane.load("models/ground/grass.obj");
    models.push_back(plane);
    //球门
    Model goal = Model(5);
    goal.translate = glm::vec3(2.5, 0, 0);
    goal.rotate = glm::vec3(-90, 0, 90);
    goal.scale = glm::vec3(0.2, 0.2, 0.2);
    goal.show = false;
    goal.load("models/goal/goal/goal.obj");
    models.push_back(goal);

    //树-6
    Model tree = Model(6);
    tree.translate = glm::vec3(18, 0, 15);
    tree.scale = glm::vec3(0.01, 0.01, 0.01);
    tree.load("models/tree/file.obj");
    tree.Change(0.01, 0.01, 0.01, 18, 0, 15);
    models.push_back(tree);

    //还是树-7
    Model trees = Model(7);
    trees.translate = glm::vec3(18, 0, 15);
    trees.scale = glm::vec3(0.01, 0.01, 0.01);
    trees.load("models/tree/file1.obj");
    trees.Change(0.01, 0.01, 0.01, 18, 0, 15);
    trees.show = false;
    models.push_back(trees);


    // 光源标志
    Model vlight = Model(8);
    vlight.translate = glm::vec3(1, 0, -1);
    vlight.rotate = glm::vec3(225, 0, 0);
    vlight.scale = glm::vec3(0.3, 0.3, 0.3);
    vlight.load("models/camera/Sun.obj");
    models.push_back(vlight);

    //建筑1
    Model b1 = Model(11);
    b1.translate = glm::vec3(0, 0, 45);
    b1.scale = glm::vec3(0.8, 0.8, 0.8);
    b1.load("models/building-obj/obj/Residential Buildings 004.obj");
    b1.show = true;
    models.push_back(b1);
    //建筑2
    Model b2 = Model(12);
    b2.translate = glm::vec3(20, 0, 45);
    b2.scale = glm::vec3(0.8, 0.8, 0.8);
    b2.load("models/building-obj/obj/Residential Buildings 010.obj");
    b2.show = true;
    models.push_back(b2);
    //建筑3
    Model b3 = Model(13);
    b3.translate = glm::vec3(-20, 0, 45);
    b3.scale = glm::vec3(0.8, 0.8, 0.8);
    b3.load("models/building-obj/obj/Residential Buildings 003.obj");
    b3.show = true;
    models.push_back(b3);
    //建筑4
    Model b4 = Model(14);
    b4.translate = glm::vec3(20, 1.5, 90);
    b4.scale = glm::vec3(0.8, 0.8, 0.8);
    b4.load("models/building-obj/obj/Residential Buildings 001.obj");
    b4.show = true;
    models.push_back(b4);

    //建筑5
    Model b5 = Model(15);
    b5.translate = glm::vec3(0, 1.5, 90);
    b5.scale = glm::vec3(0.8, 0.8, 0.8);
    b5.load("models/building-obj/obj/Residential Buildings 007.obj");
    b5.show = true;
    models.push_back(b5);

    //建筑6
    Model b6 = Model(16);
    b6.translate = glm::vec3(-20, 0.5, 90);
    b6.scale = glm::vec3(0.8, 0.8, 0.8);
    b6.load("models/building-obj/obj/Residential Buildings 002.obj");
    b6.show = true;
    models.push_back(b6);

    //外圈树1
    Model ltree1 = Model(17);
    ltree1.translate = glm::vec3(30, -0.4, 50);
    ltree1.scale = glm::vec3(0.0025, 0.0025, 0.0025);
    ltree1.load("models/tree/tree02.obj");
    models.push_back(ltree1);

    //外圈树2
    Model rtree1 = Model(18);
    rtree1.translate = glm::vec3(-30, -0.4, 50);
    rtree1.scale = glm::vec3(0.0025, 0.0025, 0.0025);
    rtree1.load("models/tree/tree02.obj");
    models.push_back(rtree1);

    //植物1
    Model plant1 = Model(19);
    plant1.translate = glm::vec3(10, 0, 90);
    plant1.scale = glm::vec3(0.015, 0.015, 0.015);
    plant1.load("models/plant/3d-model.obj");
    models.push_back(plant1);

    //植物2
    Model plant2 = Model(20);
    plant2.translate = glm::vec3(-10, 1, 90);
    plant2.scale = glm::vec3(0.5, 0.5, 0.5);
    plant2.load("models/plant/indoor plant_02.obj");
    models.push_back(plant2);

    ////车
    Model car = Model(22);
    car.rotate = glm::vec3(0, 90, 0);
    car.translate = glm::vec3(20, 0, 20);
    car.scale = glm::vec3(2.0, 2.0, 2.0);
    car.load("models/car/car.obj");
    models.push_back(car);

    //天空盒正方体
    Mesh cube;
    cube.vertexPosition = { 
        glm::vec3(-1, -1, -1),glm::vec3(1, -1, -1),glm::vec3(-1, 1, -1),glm::vec3(1, 1, -1),
        glm::vec3(-1, -1, 1),glm::vec3(1, -1, 1),glm::vec3(-1, 1, 1),glm::vec3(1, 1, 1)
    };
    cube.index = { 0,3,1,0,2,3,1,5,4,1,4,0,4,2,0,4,6,2,5,6,4,5,7,6,2,6,7,2,7,3,1,7,5,1,3,7 };
    cube.bindData();
    skybox.meshes.push_back(cube);

    //立方体贴图
    vector<const GLchar*> faces;
    faces.push_back("skybox/right.jpg");
    faces.push_back("skybox/left.jpg");
    faces.push_back("skybox/top.jpg");
    faces.push_back("skybox/bottom.jpg");
    faces.push_back("skybox/back.jpg");
    faces.push_back("skybox/front.jpg");
    skyboxTexture = TextureMap(faces);

    //光源投影
    shadowCamera.left = -20;
    shadowCamera.right = 20;
    shadowCamera.bottom = -20;
    shadowCamera.top = 20;
    shadowCamera.position = glm::vec3(0, 45, 15);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glGenFramebuffers(1, &shadowMapFBO);
    //阴影纹理
    glGenTextures(1, &shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // 阴影纹理绑定到帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //开启深度测试
    glEnable(GL_DEPTH_TEST);  
}


void display()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        if (Isfly == 1)
        {
            float allspeed = upspeed + downspeed;
            camera.position.y += allspeed;
            if (upspeed > 0)
                upspeed -= 1.5f;
            if (camera.position.y < 5)
                camera.position.y = 5;
       }
        else
        {
            upspeed = 0.0f;
        }
        Model cube = Model(9);
        if (Iscubic == true)
            tmpMatrix = movMatrix;
        cube.translate = glm::vec3(5, 2, 2) + tmpMatrix;
        cube.rotate = rotate1;
        cube.scale = scale1;
        if (Iscubic != true)
        {
            cube.load("models/basic/cube.obj");
            models.push_back(cube);
        }


        Model tai = Model(10);
        if (Istai == true)
            tmpMatrix = movMatrix;
        tai.translate = glm::vec3(5, 2, 0) + tmpMatrix;
        tai.rotate = rotate2;
        tai.scale = scale2;
        if (Istai != true)
        {
            tai.load("models/basic/tai.obj");
            models.push_back(tai);
        }


        Model ball = Model(21);
        if (Isball == true)
            tmpMatrix = movMatrix;
        ball.translate = glm::vec3(5, 2, 0) + tmpMatrix;
        ball.rotate = rotate3;
        ball.scale = scale3;
        if (Isball != true)
        {
            ball.load("models/basic/Sun.obj");
            models.push_back(ball);
        }



        glfwPollEvents();    
        ImGui_ImplGlfwGL3_NewFrame();

        MoveView();

        models[8].translate = shadowCamera.position + glm::vec3(0, 0, 2);
        //阴影渲染
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
                else if (m.id == 21 && Isball == true)
                    continue;
                else m.draw(ModelRender);
            }
        }

        //天空盒渲染
        glUseProgram(SkyBoxRender);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WindowWidth, WindowHeight);
        glUniformMatrix4fv(glGetUniformLocation(SkyBoxRender, "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix(true)));
        glUniformMatrix4fv(glGetUniformLocation(SkyBoxRender, "projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix(true)));
        //天空盒纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glUniform1i(glGetUniformLocation(SkyBoxRender, "skybox"), 1);
        skybox.translate = camera.position;
        //关闭深度
        glDepthMask(GL_FALSE);
        skybox.draw(SkyBoxRender);
        glDepthMask(GL_TRUE);

        //主渲染
        glUseProgram(ModelRender);
        glUniformMatrix4fv(glGetUniformLocation(ModelRender, "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix(true)));
        glUniformMatrix4fv(glGetUniformLocation(ModelRender, "projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix(true)));
        glm::mat4 shadowVP = shadowCamera.getProjectionMatrix(false) * shadowCamera.getViewMatrix(false);
        glUniformMatrix4fv(glGetUniformLocation(ModelRender, "shadowVP"), 1, GL_FALSE, glm::value_ptr(shadowVP));
        //传入纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowTexture);
        glUniform1i(glGetUniformLocation(ModelRender, "shadowtex"), 1);

        //光源位置
        glUniform3fv(glGetUniformLocation(ModelRender, "lightPos"), 1, glm::value_ptr(shadowCamera.position));
        //相机位置
        glUniform3fv(glGetUniformLocation(ModelRender, "cameraPos"), 1, glm::value_ptr(camera.position));
        //光源颜色
        glUniform3fv(glGetUniformLocation(ModelRender, "lightColor"), 1, glm::value_ptr(lightColor));
        //绘制
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
                else if (m.id == 21 && Isball == true)
                    continue;
                else m.draw(ModelRender);
            }
        }
          
        if (Iscubic != true)
            models.pop_back();
        if (Istai != true)
            models.pop_back();
        if (Isball != true)
            models.pop_back();
        {
            ImGui::Text("This is control window for application");
            ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.3f, 0.9f);

            ImGui::RadioButton("FlyingMode", &Isfly, 0);ImGui::SameLine();
            ImGui::RadioButton("WalkingMode", &Isfly, 1);
            ImGui::SliderFloat("LightX", &shadowCamera.position.x, -200.0f, 200.0f);
            ImGui::SliderFloat("LightY", &shadowCamera.position.y, -60.0f, 60.0f);
            ImGui::SliderFloat("LightZ", &shadowCamera.position.z, -200.0f, 200.0f);
            ImGui::ColorEdit3("Environment Light color", (float*)&lightColor);
            ImGui::Checkbox("Goal", &Isgoal);
            ImGui::Checkbox("Cubic", &Iscubic);
            ImGui::SliderFloat("cubicx", &rotate1.x, -90, 90);
            ImGui::SliderFloat("cubicy", &rotate1.y, -90, 90);
            ImGui::SliderFloat("cubicz", &rotate1.z, -90, 90);
            ImGui::SliderFloat("scalex1", &scale1.x, 0.5, 3);
            ImGui::SliderFloat("scaley1", &scale1.y, 0.5, 3);
            ImGui::SliderFloat("scalez1", &scale1.z, 0.5, 3);
            ImGui::Checkbox("Tai", &Istai);            
            ImGui::SliderFloat("taix", &rotate2.x, -90, 90);
            ImGui::SliderFloat("taiy", &rotate2.y, -90, 90);
            ImGui::SliderFloat("taiz", &rotate2.z, -90, 90);
            ImGui::SliderFloat("scalex2", &scale2.x, 0.5, 3);
            ImGui::SliderFloat("scaley2", &scale2.y, 0.5, 3);
            ImGui::SliderFloat("scalez2", &scale2.z, 0.5, 3);
            ImGui::Checkbox("Ball", &Isball);             
            ImGui::SliderFloat("ballx", &rotate3.x, -90, 90);
            ImGui::SliderFloat("bally", &rotate3.y, -90, 90);
            ImGui::SliderFloat("ballz", &rotate3.z, -90, 90);
            ImGui::SliderFloat("scalex3", &scale3.x, 0.1, 0.5);
            ImGui::SliderFloat("scaley3", &scale3.y, 0.1, 0.5);
            ImGui::SliderFloat("scalez3", &scale3.z, 0.1, 0.5);
            

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("pitch: %.3f", camera.pitch); ImGui::SameLine();
            ImGui::Text("yaw: %.3f", camera.yaw);ImGui::SameLine();
            ImGui::Text("roll: %.3f", camera.roll);
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(m_Window);  //双缓冲   
        idle();

    }
}
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (xpos > 700 || ypos > 700)
    {
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
                        model[j][i] = modelview[4 * j + i];
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

