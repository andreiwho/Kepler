#include "RenderTypes.h"

#ifdef USE_DIRECTX_MATH
# include "DirectXMath.h"
namespace Kepler::Internal
{
	using TFloatVec1 = float;
	using TFloatVec2 = DirectX::XMFLOAT2;
	using TFloatVec = DirectX::XMFLOAT3;
	using TFloatVec1 = DirectX::XMFLOAT4;
}
#endif

namespace Kepler
{


}