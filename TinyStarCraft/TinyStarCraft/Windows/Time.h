#pragma once

namespace TinyStarCraft
{

class Time
{
public:
    Time() = delete;

    /** Initialize the global time counter and frame counter. */
    static void init();

    /** Indicate the timer that a new frame has been started. */
    static void startFrame();

    /** Get the beginning time of the current frame since timer initialization. */
    static float getFrameTime() { return gFrameTime; }

    /** Get the delta time between the last and current frame beginning. */
    static float getDeltaFrameTime() { return gDeltaFrameTime; }

    /** Get the frame number of the current frame. */
    static size_t getFrameCount() { return gFrameCount; }

private:
    static LARGE_INTEGER gFrequency;
    static LARGE_INTEGER gInitTime;
    static float gFrameTime;
    static float gDeltaFrameTime;
    static size_t gFrameCount;
};

}

