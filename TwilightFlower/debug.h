#pragma once 

//Choose debug level
//level 1 max
//level 2 mid
//level 3 min

#define DEBUG_LEVEL_1 1
#define DEBUG_LEVEL_2 0
#define DEBUG_LEVEL_3 0

#define DEBUG_L1(__expr__) do { if (DEBUG_LEVEL_1) { __expr__; } } while(0)
#define DEBUG_L2(__expr__) do { if (DEBUG_LEVEL_2 || DEBUG_LEVEL_1) { __expr__; } } while(0)
#define DEBUG_L3(__expr__) do { if (DEBUG_LEVEL_3 || DEBUG_LEVEL_2 || DEBUG_LEVEL_1) { __expr__; } } while(0)