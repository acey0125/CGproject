#include "VideoRecorder.h"
using namespace std;

 void VideoRecorder::RecordInit(unsigned int framerate, unsigned int width, unsigned int height, string output_name = "videoRecord.mp4")
{
    char* cmd = new char[300];
    sprintf_s(cmd, 300, "ffmpeg -r %u -f rawvideo -pix_fmt rgba -s %ux%u -i - "
        "-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip %s", framerate, width, height, output_name.c_str());
    ffmpeg = _popen(cmd, "wb");
    delete[] cmd;
    recbuffer = new int[2200 * 1500];
}

 void VideoRecorder::GrabFrame()
 {
     glReadPixels(0, 0, 2200, 1500, GL_RGBA, GL_UNSIGNED_BYTE, recbuffer);

     fwrite(recbuffer, sizeof(int) * 2200 * 1500, 1, ffmpeg);
 }

 void VideoRecorder::Finish()
 {
     delete[] recbuffer;
     _pclose(ffmpeg);
 }
 FILE* VideoRecorder::ffmpeg = NULL;
 int* VideoRecorder::recbuffer = NULL;