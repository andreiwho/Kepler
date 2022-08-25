#pragma once
#ifdef WIN32
#include "Core/Types.h"
#include "Core/Containers/DynArray.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <atlbase.h>

#ifdef USE_ASSERT
# define HRCHECK(x) CHECK(SUCCEEDED(x))
#else
# define HRCHECK(x) x
#endif

namespace Kepler
{
	struct TCommonImageState
	{
		TDynArray<ID3D11RenderTargetView*> RenderTargetViews{};
		TDynArray<ID3D11DepthStencilView*> DepthStencilViews{};
		TDynArray<ID3D11ShaderResourceView*> ShaderResourceView{};
	};
}

#endif