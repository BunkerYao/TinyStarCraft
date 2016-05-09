#pragma once

#define MAX_DEBUG_OUTPUT_LENGTH 512

namespace TinyStarCraft
{

/**
 *	Print a formatted message on output window
 */
void OutputDebugStringF(const char* format, ...)
{
    char str[MAX_DEBUG_OUTPUT_LENGTH];

    va_list args;
    va_start(args, format);
    ::StringCbVPrintf(str, MAX_DEBUG_OUTPUT_LENGTH, format, args);
    va_end(args);

    ::OutputDebugString(str);
}


};
