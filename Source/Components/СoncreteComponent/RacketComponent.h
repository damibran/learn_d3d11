#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Input/Keys.h"
#include "Components/IGameComponent.h"
#include "SimpleMath.h"
#include "../../RenderData/VertexBuffer.h"

namespace dmbrn
{
	class RacketComponent :public IGameComponent
	{
	public:
		RacketComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
			DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), Keys key_up = Keys::W, Keys key_down = Keys::S)
			: IGameComponent(game), shaderPath(shaderPath), scale(scale), translation(offset), keyUp(key_up), keyDown(key_down)
		{
			sModelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

			initialAABB.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
			initialAABB.Extents = DirectX::SimpleMath::Vector3(0.5, 0.5, 1);
			initialAABB.Transform(currentAABB, sModelMat.model);
		}
		// Inherited via IGameComponent
		void Initialize() override;
		void Update(float) override;
		void PhysicsUpdate(float) override;
		void CollisionUpdate(float) override;
		void RenderDataUpdate() override;
		void Draw() override;
		void DestroyResources() override;

		DirectX::BoundingBox getAABB() const
		{
			return currentAABB;
		}

	private:
		std::wstring shaderPath;
		ID3DBlob* vertexShaderByteCode;
		ID3DBlob* pixelShaderByteCode;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		
		ID3D11Buffer* indexBuffer;
		ID3D11RasterizerState* rastState;

		struct VertexBufferData
		{
			struct Vertex
			{
				DirectX::SimpleMath::Vector4 pos;
				DirectX::SimpleMath::Vector4 color;
			};

			D3D11_INPUT_ELEMENT_DESC desc[2] =
			{
				D3D11_INPUT_ELEMENT_DESC {
					"SV_POSITION",
					0,
					DXGI_FORMAT_R32G32B32A32_FLOAT,
					0,
					0,
					D3D11_INPUT_PER_VERTEX_DATA,
					0},
					D3D11_INPUT_ELEMENT_DESC {
					"COLOR",
					0,
					DXGI_FORMAT_R32G32B32A32_FLOAT,
					0,
					offsetof(Vertex,color),
					D3D11_INPUT_PER_VERTEX_DATA,
					0}
			};

			Vertex data[4] =
			{
				Vertex{DirectX::SimpleMath::Vector4(-0.5,-0.5, 0, 1.0f),DirectX::SimpleMath::Vector4(1,0,0, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(-0.5,0.5,0, 1.0f),DirectX::SimpleMath::Vector4(0,1,0, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(0.5, -0.5,0, 1.0f),DirectX::SimpleMath::Vector4(0,0,1, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(0.5,0.5,0,1),DirectX::SimpleMath::Vector4(0,0,0,1)}
			};
		}vertexBufferData;

		VertexBuffer<VertexBufferData> vertexBuffer;

		UINT indices[6] =
		{
			0,1,2,
			2,1,3
		};

	private:
		DirectX::SimpleMath::Vector2 scale;
		DirectX::SimpleMath::Vector2 translation;
		Keys keyUp, keyDown;
		struct SModelMat
		{
			DirectX::SimpleMath::Matrix model;
		}sModelMat;
		ID3D11Buffer* constantBufferModel;

		DirectX::BoundingBox initialAABB;
		DirectX::BoundingBox currentAABB;
	};
}