
// STL includes
#include <vector>
#include <set>
#include <string>
#include <deque>
#include <map>

// IO includes
#include <fstream>
#include <iostream>
#include <sstream>


// when using debug mode, used to see file and line number of memory leaks
// this should be AFTER #includes of outside libraries (the map of new to DEBUG_NEW can cause issues for outside libraries)
// but before includes of our code to include all our code in the checks
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>	
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#include <GL/glew.h>
#include "Input.h"
