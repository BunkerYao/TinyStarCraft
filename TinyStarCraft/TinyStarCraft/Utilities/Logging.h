#pragma once

namespace TinyStarCraft
{

enum class ELogMessageType
{
    eInfo = 0, eWarning = 1, eError = 2
};

/**
  	Prints a message line in stdout.
@remarks
    Format is:
    [HH:MM:SS] [function] msgType: formatted message
    Don't use this function directly, use macros TINYSC_LOGLINE_INFO, TINYSC_LOGLINE_WARN and TINYSC_LOGLINE_ERR instead.
 */
void _LogLine(ELogMessageType msgType, const char* function, const char* format, ...);


/** Logout an information. */
#define TINYSC_LOGLINE_INFO(format, ...) _LogLine(ELogMessageType::eInfo, __FUNCTION__, format, __VA_ARGS__);

/** Logout an warning message. */
#define TINYSC_LOGLINE_WARN(format, ...) _LogLine(ELogMessageType::eWarning, __FUNCTION__, format, __VA_ARGS__);

/** Logout an error message. */
#define TINYSC_LOGLINE_ERR(format, ...) _LogLine(ELogMessageType::eError, __FUNCTION__, format, __VA_ARGS__);

/** Logout a D3D API call infomation */
#define TINYSC_LOGLINE_D3D_INFO(func, hr) _LogLine(ELogMessageType::eInfo, __FUNCTION__, func ## " 0x%08x %s.", hr, ::DXGetErrorDescription(hr))

/** Logout a D3D API call warning */
#define TINYSC_LOGLINE_D3D_WARN(func, hr) _LogLine(ELogMessageType::eWarning, __FUNCTION__, func ## " 0x%08x %s.", hr, ::DXGetErrorDescription(hr))

/** Logout a D3D API call error */
#define TINYSC_LOGLINE_D3D_ERR(func, hr) _LogLine(ELogMessageType::eError, __FUNCTION__, func ## " 0x%08x %s.", hr, ::DXGetErrorDescription(hr))

};
