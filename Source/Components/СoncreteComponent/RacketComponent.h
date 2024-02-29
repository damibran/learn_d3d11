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

		RacketComponent(GameToComponentBridge bridge, RastState& rs, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
			DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), Keys key_up = Keys::W, Keys key_down = Keys::S) :
			IGameComponent(bridge),
			rastState(rs),
			shaders(bridge.device.getDevice(), shaderPath),
			vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData),
			indexBuffer(bridge.device.getDevice(), indexBufferData),
			constBuf(bridge.device.getDevice(), modelMat),
			scale(scale), translation(offset), keyUp(key_up), keyDown(key_down)
		{
			modelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

			initialAABB.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
			initialAABB.Extents = DirectX::SimpleMath::Vector3(0.5, 0.5, 1);
			initialAABB.Transform(currentAABB, modelMat.model);
		}

		// Inherited via IGameComponent

		void Initialize() override
		{

		}

		void Update(float dt) override
		{
			if (bridge.window.getInput().IsKeyDown(keyUp))
			{
				translation.y += dt * 1;
			}
			if (bridge.window.getInput().IsKeyDown(keyDown))
			{
				translation.y -= dt * 1;
			}
		}

		void PhysicsUpdate(float) override
		{
			initialAABB.Transform(currentAABB, DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0)));
		}

		void CollisionUpdate(float dt) override
		{
			DirectX::SimpleMath::Plane topPlane({ 0,1,0 }, { 0,-1,0 });
			DirectX::SimpleMath::Plane botPlane({ 0,-1,0 }, { 0,1,0 });

			if (currentAABB.Intersects(topPlane))
			{
				translation.y -= dt * 1;// actually here should be depenetration action
				return;
			}
			else if (currentAABB.Intersects(botPlane))
			{
				translation.y += dt * 1;// actually here should be depenetration action
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
			//TODO: release all
		}

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
				Vertex{DirectX::SimpleMath::Vector4(-0.5,-0.5, 0.5, 1.0f),DirectX::SimpleMath::Vector4(1,0,0, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(-0.5,0.5,0.5, 1.0f),DirectX::SimpleMath::Vector4(0,1,0, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(0.5, -0.5,0.5, 1.0f),DirectX::SimpleMath::Vector4(0,0,1, 1.0f)},
				Vertex{DirectX::SimpleMath::Vector4(0.5,0.5,0.5,1),DirectX::SimpleMath::Vector4(0,0,0,1)}
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