#pragma once
#ifdef WIN32
#include "Core/Types.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#ifdef USE_ASSERT
# define HRCHECK(x) CHECK(SUCCEEDED(x))
#else
# define HRCHECK(x) x
#endif

namespace Kepler
{
}

#endif