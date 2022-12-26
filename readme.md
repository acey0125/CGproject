# CGproject
glew + glfw
基本的图形绘制管线、模型导入与生成、相机移动、光源、阴影效果、天空盒
#文件结构
Mesh.h 纹理数据绑定与绘制
Model.h 模型数据导入
ShadowAndTex.h 文件读取与纹理
Camera.h 照相机控制（包括光源）
main.c 基本键盘控制、初始化操作、绘制操作

目前已完成要求：变换、光照、场景漫游、模型导入
需要完成的要求：基本建模表达、模型导出、纹理编辑、动画与截取保存功能

其中基本建模表达指的是不用模型导入直接绘制，需要和相关按键结合，实现效果就是按下某个按键可以在鼠标处生成模型  
模型导出需要和物体点击相结合，实现效果是点击某个模型生成obj文件  
纹理编辑也要和物体点击相结合，实现效果是点击某个模型更换纹理  
动画保存和截取，需要和相关按键结合，实现效果为按下某个按键截图或者开始录制视频


model.h里新增了成员变量show，和成员函数Change，show控制当前模型是否展示、Change函数用于计算当前模型包围盒的相对大小  
main.cpp里308行增加m.show的判断，用于控制纹理切换前后的对应的模型展示  
main.cpp里319行增加了Mouse函数，用于响应鼠标右键点击切换纹理效果  
main.cpp里117-190新增了几个模型

更新变换模块与基本架构