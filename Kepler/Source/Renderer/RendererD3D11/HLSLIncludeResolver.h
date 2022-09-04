#pragma once
#include "D3D11Common.h"

namespace Kepler
{
	struct THLSLIncludeResolverD3D11 : public ID3DInclude
	{
	public:
		THLSLIncludeResolverD3D11(const TString& InMyPath);

		HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
		HRESULT Close(LPCVOID Data) override;

	private:
		TString MyPath{};
		TString OutPath{};
	};
}