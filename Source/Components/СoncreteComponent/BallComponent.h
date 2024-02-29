#pragma once

#include <memory>
#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Input/Keys.h"
#include "Components/IGameComponent.h"
#include "RacketComponent.h"
#include "SimpleMath.h"
#include "../../RenderData/Shaders.h"
#include "../../RenderData/VertexBuffer.h"
#include "../../RenderData/IndexBuffer.h"
#include "../../RenderData/ConstantBuffer.h"

namespace dmbrn
{
	class BallComponent :public IGameComponent
	{
	public:
		BallComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale, 
			const RacketComponent& lRckt, const RacketComponent& rRckt,
			DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), float spd = 0);
		// Inherited via IGameComponent
		void Initialize() override;
		void Update(float) override;
		void PhysicsUpdate(float) override;
		void CollisionUpdate(float) override;
		void RenderDataUpdate() override;
		void Draw() override;
		void DestroyResources() override;

		float speed;

	private:
		Shaders shaders;
		ID3D11RasterizerState* rastState;

		struct VertexBufferData
		{
			struct Vertex
			{
				DirectX::SimpleMath::Vector4 pos;
				DirectX::SimpleMath::Vector4 color;
			};

			D3D11_INPUT_ELEMENT_DESC desc[2] = {
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
		DirectX::SimpleMath::Vector2 moveDir = DirectX::SimpleMath::Vector2(1, 0);

		struct SModelMat
		{
			DirectX::SimpleMath::Matrix model;
		}modelMat;

		ConstantBuffer<decltype(modelMat)> constBuf;

		DirectX::BoundingSphere initialBS;
		DirectX::BoundingSphere currentBS;

		const RacketComponent& lRacket;
		const RacketComponent& rRacket;
	};
}