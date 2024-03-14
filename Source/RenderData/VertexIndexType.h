#pragma once

#include <array>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include "InputLayout.h"

namespace dmbrn
{
	struct VertexType
	{
		struct alignas(16) Object
		{
			DirectX::SimpleMath::Vector3 pos;
			DirectX::SimpleMath::Vector3 normal;
			DirectX::SimpleMath::Vector2 texCoord;
		};

		static inline std::array<D3D11_INPUT_ELEMENT_DESC, 3> desc = {
				D3D11_INPUT_ELEMENT_DESC {
					"SV_POSITION",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					0,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 },
				D3D11_INPUT_ELEMENT_DESC {
					"NORMAL",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					offsetof(Object, normal),
					D3D11_INPUT_PER_VERTEX_DATA,
					0 },
					D3D11_INPUT_ELEMENT_DESC {
					"TEXCOORD",
					0,
					DXGI_FORMAT_R32G32_FLOAT,
					0,
					offsetof(Object, texCoord),
					D3D11_INPUT_PER_VERTEX_DATA,
					0 }
		};
	};

	struct IndexType
	{
		typedef UINT Object;

		static inline DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
	};
}