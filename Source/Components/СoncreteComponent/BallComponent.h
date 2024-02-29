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
#include "../../RenderData/RasterState.h"
#include "GameToBallBridge.h"

namespace dmbrn
{
	class BallComponent :public IGameComponent
	{
	public:

		BallComponent(GameToComponentBridge bridge, GameToBallBridge gtbb, RastState& rs, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
			const RacketComponent& lRckt, const RacketComponent& rRckt, DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), float spd = 0) :
			IGameComponent(bridge),
			game2BallBridge(gtbb),
			rastState(rs),
			shaders(bridge.device.getDevice(), shaderPath),
			vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData),
			indexBuffer(bridge.device.getDevice(), indexBufferData),
			constBuf(bridge.device.getDevice(), modelMat),
			scale(scale), lRacket(lRckt), rRacket(rRckt), translation(offset), speed(spd)
		{
			modelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

			initialBS.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
			initialBS.Radius = 0.5 * scale.x; // for some reason transform for sphere doesnt apply scale !
			initialBS.Transform(currentBS, modelMat.model);
		}

		// Inherited via IGameComponent
		void Initialize() override
		{

		}

		void Update(float dt) override
		{
			translation += dt * speed * moveDir;
		}

		void PhysicsUpdate(float) override
		{
			initialBS.Transform(currentBS, DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0)));
		}

		void CollisionUpdate(float dt) override
		{
			if (currentBS.Intersects(lRacket.getAABB()))
			{
				translation -= dt * speed * moveDir; // actually here should be depenetration action

				DirectX::SimpleMath::Vector2 normal(1, 0);

				moveDir = moveDir.Reflect(moveDir, normal);

				float dy = 10 * (currentBS.Center.y - lRacket.getAABB().Center.y);
				moveDir.y += dy;

				moveDir.Normalize();

				speed += 0.05;
				return;
			}
			else if (currentBS.Intersects(rRacket.getAABB()))
			{
				translation -= dt * speed * moveDir;// actually here should be depenetration action

				DirectX::SimpleMath::Vector2 normal(-1, 0);
				moveDir = moveDir.Reflect(moveDir, normal);

				float dy = 10 * (currentBS.Center.y - rRacket.getAABB().Center.y);
				moveDir.y += dy;

				moveDir.Normalize();

				speed += 0.05;
				return;
			}

			DirectX::SimpleMath::Plane topPlane({ 0,1,0 }, { 0,-1,0 });
			DirectX::SimpleMath::Plane botPlane({ 0,-1,0 }, { 0,1,0 });

			if (currentBS.Intersects(topPlane))
			{
				translation -= dt * speed * moveDir;// actually here should be depenetration action

				moveDir = moveDir.Reflect(moveDir, DirectX::SimpleMath::Vector2(topPlane.Normal().x, topPlane.Normal().y));
				return;
			}
			else if (currentBS.Intersects(botPlane))
			{
				translation -= dt * speed * moveDir;// actually here should be depenetration action

				moveDir = moveDir.Reflect(moveDir, DirectX::SimpleMath::Vector2(topPlane.Normal().x, topPlane.Normal().y));
				return;
			}

			DirectX::SimpleMath::Plane rightPlane({ 1,0,0 }, { -1,0,0 });
			DirectX::SimpleMath::Plane leftPlane({ -1,0,0 }, { 1,0,0 });

			if (currentBS.Intersects(rightPlane))
			{
				translation = DirectX::SimpleMath::Vector2(0, 0);
				game2BallBridge.lRacketScore++;
				speed = 0.2;
				return;
			}
			else if (currentBS.Intersects(leftPlane))
			{
				translation = DirectX::SimpleMath::Vector2(0, 0);
				game2BallBridge.rRacketScore++;
				speed = 0.2;
				return;
			}
		}

		void RenderDataUpdate() override
		{
			auto mat = constBuf.map(bridge.device.getContext());

			mat->model =
				DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0));

			mat->model = mat->model.Transpose();

			constBuf.upmap(bridge.device.getContext());
		}

		void Draw() override
		{
			rastState.bind(bridge.device.getContext());

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			vertexBuffer.bindVertexBuffer(bridge.device.getContext());
			indexBuffer.bind(bridge.device.getContext());
			shaders.bindShaders(bridge.device.getContext());

			constBuf.bind(bridge.device.getContext(), 1);

			bridge.device.getContext()->DrawIndexed(6, 0, 0);
		}

		void DestroyResources() override
		{

		}

		float speed;

	private:
		GameToBallBridge game2BallBridge;
		RastState& rastState;
		Shaders shaders;

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
				Vertex{DirectX::SimpleMath::Vector4(-0.5,-0.5, 0.5, 1.0f),DirectX::SimpleMath::Vector4(1,0,0, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(-0.5,0.5,0.5, 1.0f),DirectX::SimpleMath::Vector4(0,1,0, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(0.5, -0.5,0.5, 1.0f),DirectX::SimpleMath::Vector4(0,0,1, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(0.5,0.5,0.5,1),DirectX::SimpleMath::Vector4(0,0,0,1)}
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