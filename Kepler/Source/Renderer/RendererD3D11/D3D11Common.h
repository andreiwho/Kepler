#pragma once
#ifdef WIN32
#include "Core/Types.h"
#include "Core/Containers/DynArray.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#ifdef USE_ASSERT
# define HRCHECK(x) CHECK(SUCCEEDED(x))
# define HRCHECK_NOTHROW(x) CHECK_NOTHROW(SUCCEEDED(x))
#else
# define HRCHECK(x) (void)x
# define HRCHECK_NOTHROW(x) (void)x
#endif

#define SAFE_ADD_REF(p) if(!!(p)) (p)->AddRef()
#define SAFE_RELEASE(p) if(!!(p)) (p)->Release() 

namespace ke
{
	struct TCommonImageState
	{
		Array<ID3D11RenderTargetView*> RenderTargetViews{};
		Array<ID3D11DepthStencilView*> DepthStencilViews{};
		Array<ID3D11ShaderResourceView*> ShaderResourceView{};
	};
}

#endif