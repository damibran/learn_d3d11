#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Input/Keys.h"
#include "Components/IGameComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class BallComponent :public IGameComponent
	{
	public:
		BallComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
			DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), float spd = 0)
			: IGameComponent(game), shaderPath(shaderPath), scale(scale), translation(offset), speed(spd)
		{
			sModelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

			AABB.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
			AABB.Extents = DirectX::SimpleMath::Vector3(1, 1, 1);
			AABB.Transform(AABB, sModelMat.model); // may be UB here
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
			Vertex{DirectX::SimpleMath::Vector4(0, 0, 0, 1.0f),DirectX::SimpleMath::Vector4(0.1,0,0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(0,1,0, 1.0f),DirectX::SimpleMath::Vector4(0,0.1,0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(1, 0,0, 1.0f),DirectX::SimpleMath::Vector4(0,0,0.1, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(1,1,0,1),DirectX::SimpleMath::Vector4(0,0,0,1)}
		};

		UINT indices[6] =
		{
			0,1,2,
			2,1,3
		};
	private:
		DirectX::SimpleMath::Vector2 scale;
		DirectX::SimpleMath::Vector2 translation;
		float speed;
		DirectX::SimpleMath::Vector2 moveDir = DirectX::SimpleMath::Vector2(1, 0);
		struct SModelMat
		{
			DirectX::SimpleMath::Matrix model;
		}sModelMat;
		ID3D11Buffer* constantBufferModel;

		DirectX::BoundingBox AABB;
	};
}