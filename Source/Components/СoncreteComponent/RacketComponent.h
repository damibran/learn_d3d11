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
#include "../../RenderData/Shaders.h"
#include "../../RenderData/IndexBuffer.h"
#include "../../RenderData/ConstantBuffer.h"
#include "../../RenderData/RasterState.h"

namespace dmbrn
{
	class RacketComponent :public IGameComponent
	{
	public:
		RacketComponent(Game& game, RastState& rs, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
			DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), Keys key_up = Keys::W, Keys key_down = Keys::S);
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
		RastState& rastState;
		Shaders shaders;

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

		VertexBuffer<decltype(vertexBufferData)> vertexBuffer;

		struct IndexBufferData
		{
			static inline DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;

			UINT data[6] =
			{
				0,1,2,
				2,1,3
			};
		}indexBufferData;

		IndexBuffer<decltype(indexBufferData)> indexBuffer;

	private:
		DirectX::SimpleMath::Vector2 scale;
		DirectX::SimpleMath::Vector2 translation;

		Keys keyUp, keyDown;

		struct SModelMat
		{
			DirectX::SimpleMath::Matrix model;
		}modelMat;
		
		ConstantBuffer<decltype(modelMat)> constBuf;

		DirectX::BoundingBox initialAABB;
		DirectX::BoundingBox currentAABB;
	};
}