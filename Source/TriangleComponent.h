#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "IGameComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class TriangleComponent :public IGameComponent
	{
	public:
		TriangleComponent(Game& game, const std::wstring& shaderPath)
			: IGameComponent(game), shaderPath(shaderPath) {}

		TriangleComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 offset)
			: IGameComponent(game), shaderPath(shaderPath)
		{
			points[0].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[1].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[2].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
		}

		TriangleComponent(Game& game, const std::wstring& shaderPath, float angleDeg, DirectX::SimpleMath::Vector2 offset)
			: IGameComponent(game), shaderPath(shaderPath)
		{
			points[0].pos = DirectX::SimpleMath::Vector4(std::cosf(DirectX::XMConvertToRadians(angleDeg)) / 2., std::sinf(DirectX::XMConvertToRadians(angleDeg)) / 2., 0., 1.);
			points[1].pos = DirectX::SimpleMath::Vector4(std::cosf(DirectX::XMConvertToRadians(angleDeg + 120)) / 2., std::sinf(DirectX::XMConvertToRadians(angleDeg + 120)) / 2., 0., 1.);
			points[2].pos = DirectX::SimpleMath::Vector4(std::cosf(DirectX::XMConvertToRadians(angleDeg + 240)) / 2., std::sinf(DirectX::XMConvertToRadians(angleDeg + 240)) / 2., 0., 1.);



			points[0].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[1].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[2].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
		}
		// Inherited via IGameComponent
		void Initialize() override;
		void Update() override;
		void Draw() override;
		void DestroyResources() override;

	private:
		std::wstring shaderPath;
		ID3DBlob* vertexShaderByteCode;
		ID3DBlob* pixelShaderByteCode;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* vb;
		ID3D11RasterizerState* rastState;

		struct Vertex
		{
			DirectX::SimpleMath::Vector4 pos;
			DirectX::SimpleMath::Vector4 color;
		};

		Vertex points[3] =
		{
			Vertex{DirectX::SimpleMath::Vector4(0, 0.5f, 0, 1.0f),DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(0.5,-0.5,0, 1.0f),DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(-0.5f, -0.5f,0, 1.0f),DirectX::SimpleMath::Vector4(0, 0, 1, 1.0f)}
		};
	};
}