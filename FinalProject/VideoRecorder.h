#pragma once
#include<stdio.h>
#include<gl/glew.h>
#include <string>
class VideoRecorder
{
public:
    static FILE* ffmpeg;
    static int* recbuffer;

    void RecordInit(unsigned int framerate, unsigned int width, unsigned int height, std::string output_name);
    void GrabFrame();
    void Finish();
};

