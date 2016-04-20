#pragma once

#include <cassert>
#include <exception>
#include <functional>
#include <string>
#include <vector>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/** Maximum length of the exception message. */
#define MAX_EXCEPTION_MESSAGE_LENGTH 512
