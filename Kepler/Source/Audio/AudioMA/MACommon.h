#pragma once
#include "Core/Macros.h"
#include "../AudioCommon.h"

#include <miniaudio.h>

#ifdef ENABLE_DEBUG
# define MACHECK(Result) CHECK((Result == MA_SUCCESS))
#else
# define MACHECK(Result) (void)Result
#endif