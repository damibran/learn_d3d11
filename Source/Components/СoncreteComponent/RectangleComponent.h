#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Components/IGameComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class RectangleComponent :public IGameComponent
	{
	public:
		RectangleComponent(Game& game, const std::wstring& shaderPath)
			: IGameComponent(game), shaderPath(shaderPath) {}

		RectangleComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 offset)
			: IGameComponent(game), shaderPath(shaderPath)
		{
			points[0].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[1].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[2].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
		}

		RectangleComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale, DirectX::SimpleMath::Vector2 offset)
			: IGameComponent(game), shaderPath(shaderPath)
		{
			points[0].pos *= DirectX::SimpleMath::Vector4(scale.x, scale.y, 1, 1);
			points[1].pos *= DirectX::SimpleMath::Vector4(scale.x, scale.y, 1, 1);
			points[2].pos *= DirectX::SimpleMath::Vector4(scale.x, scale.y, 1, 1);
			points[3].pos *= DirectX::SimpleMath::Vector4(scale.x, scale.y, 1, 1);

			points[0].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[1].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[2].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[3].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
		}
		// Inherited via IGameComponent
		void Initialize() override;
		void Update(float) override;
		void Draw() override;
		void DestroyResources() override;

	private:
		std::wstring shaderPath;
		ID3DBlob* vertexShaderByteCode;
		ID3DBlob* pixelShaderByteCode;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;

		ID3D11RasterizerState* rastState;

		struct Vertex
		{
			DirectX::SimpleMath::Vector4 pos;
			DirectX::SimpleMath::Vector4 color;
		};

		Vertex points[4] =
		{
			Vertex{DirectX::SimpleMath::Vector4(0, 0, 0, 1.0f),DirectX::SimpleMath::Vector4(1,0,0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(0,1,0, 1.0f),DirectX::SimpleMath::Vector4(0,1,0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(1, 0,0, 1.0f),DirectX::SimpleMath::Vector4(0,0,1, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(1,1,0,1),DirectX::SimpleMath::Vector4(0,0,0,1)}
		};

		UINT indices[6] =
		{
			0,1,2,
			2,1,3
		};
	};
}