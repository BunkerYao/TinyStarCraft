#include "Precompiled.h"
#include "Time.h"

namespace TinyStarCraft
{

LARGE_INTEGER Time::gFrequency;
LARGE_INTEGER Time::gInitTime;
float Time::gFrameTime = 0.0f;
float Time::gDeltaFrameTime = 0.0f;
size_t Time::gFrameCount = 0;

void Time::init()
{
    ::QueryPerformanceFrequency(&gFrequency);
    ::QueryPerformanceCounter(&gInitTime);
}

void Time::startFrame()
{
    LARGE_INTEGER currentTime;
    ::QueryPerformanceCounter(&currentTime);

    float lastFrameTime = gFrameTime;
    gFrameTime = (float)(currentTime.QuadPart - gInitTime.QuadPart) / (float)gFrequency.QuadPart;
    gDeltaFrameTime = gFrameTime - lastFrameTime;

    gFrameCount++;
}



}
