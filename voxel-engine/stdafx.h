// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _ITERATOR_DEBUG_LEVEL 0  
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ctime>
#include <math.h>
#include <vector>
#include <array>
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <valarray>
#include <functional>
#include <queue>
#include <atomic>
#include <iostream>

#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment(lib, "glew32.lib")
//#include <ft2build.h>

#include "lib\glm\glm.hpp"
#include "lib\glm\gtc\matrix_transform.hpp"
#include "lib\glm\gtc\type_ptr.hpp"