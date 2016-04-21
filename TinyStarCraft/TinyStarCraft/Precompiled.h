#pragma once

#include <strsafe.h>

#include <cassert>
#include <cstdio>
#include <exception>
#include <functional>
#include <string>
#include <vector>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

/** Maximum length of the exception message. */
#define MAX_EXCEPTION_MESSAGE_LENGTH 512
