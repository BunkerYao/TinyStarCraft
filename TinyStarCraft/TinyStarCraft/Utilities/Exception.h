#pragma once

/** Maximum length of the exception message. */
#define TINYSC_MAX_EXCEPTION_MESSAGE_LENGTH 512

namespace TinyStarCraft
{

/**
 *	An extended exception class which carries function names and line number
 */
class Exception : std::runtime_error
{
public:
    /**
     *	A useful method to throw a exception with formatted message.
     *
     *  @remarks
     *  Use the macro TINYSC_THROWF which also calls this function.
     */
    static void _throwf(const char* funcName, int line, const char* format, ...)
    {
        char str[TINYSC_MAX_EXCEPTION_MESSAGE_LENGTH];
        va_list args;
        va_start(args, format);
        ::sprintf_s(str, TINYSC_MAX_EXCEPTION_MESSAGE_LENGTH, format, args);
        va_end(args);

        throw Exception(str, funcName, line);
    }

    /**	Get the function name. */
    const char* getFunctionName() const
    {
        return mFunctionName;
    }

    /** Get line number. */
    int getLine() const
    {
        return mLine;
    }

public:
    Exception(const char* message, const char* funcName, int line)
        : std::runtime_error(message),
          mFunctionName(funcName),
          mLine(line)
    {}

private:
    const char* mFunctionName;
    int mLine;
};

// Throw out an exception with formatted message
#define TINYSC_THROWF(message, ...) Exception::_throwf(__FUNCTION__, __LINE__, message, __VA_ARGS__)

};