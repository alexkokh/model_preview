#pragma once

#define FREEGLUT_STATIC

#if defined DEBUG || defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\glext.h>
#include <gl\freeglut.h>
#include <vector>
#include "DAEReader.h"
#include "vmath.h"
#include "LoadShaders.h"

using namespace vmath;



