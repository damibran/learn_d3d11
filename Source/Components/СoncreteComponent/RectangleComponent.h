#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <string>
#define _USE_MATH_DEFINES
#include "../../RenderData/ConstantBuffer.h"
#include "../../RenderData/IndexBuffer.h"
#include "../../RenderData/RasterState.h"
#include "../../RenderData/Shaders.h"
#include "../../RenderData/VertexBuffer.h"
#include "../Transform.h"
#include "Components/IGameComponent.h"
#include <directxtk/SimpleMath.h>
#include <math.h>

namespace dmbrn {
	class RectangleComponent : public IGameComponent {
	public:
		RectangleComponent(GameToComponentBridge bridge, RastState& rs, const std::wstring& shaderPath, const TransformComponent& trans)
			: IGameComponent(bridge)
			, transform(trans)
			, rastState(rs)
			, shaders(bridge.device.getDevice(), shaderPath)
			, vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData)
			, indexBuffer(bridge.device.getDevice(), indexBufferData)
			, constBuf(bridge.device.getDevice(), modelMat)
		{
		}

		void Update(float) override
		{
		}

		void RenderDataUpdate() override
		{
			auto mat = constBuf.map(bridge.device.getContext());

			mat->model = transform.getMatrix();

			mat->model = mat->model.Transpose();
		}

		void Draw() override
		{
			rastState.bind(bridge.device.getContext());

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			vertexBuffer.bindVertexBuffer(bridge.device.getContext());
			indexBuffer.bind(bridge.device.getContext());
			shaders.bindShaders(bridge.device.getContext());

			constBuf.bindToVertex(bridge.device.getContext(), 1);

			bridge.device.getContext()->DrawIndexed(6, 0, 0);
		}

		TransformComponent transform;

	private:
		RastState& rastState;
		Shaders shaders;

		struct VertexBufferData {
			struct alignas(16) Vertex
			{
				DirectX::SimpleMath::Vector3 pos;
				DirectX::SimpleMath::Vector4 color;
			};

			D3D11_INPUT_ELEMENT_DESC desc[2] = {
				D3D11_INPUT_ELEMENT_DESC {
					"SV_POSITION",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					0,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 },
				D3D11_INPUT_ELEMENT_DESC {
					"COLOR",
					0,
					DXGI_FORMAT_R32G32B32A32_FLOAT,
					0,
					offsetof(Vertex, color),
					D3D11_INPUT_PER_VERTEX_DATA,
					0 }
			};

			Vertex data[4] = {
				Vertex { DirectX::SimpleMath::Vector3(-0.5, -0.5, 0.5), DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(-0.5, 0.5, 0.5), DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(0.5, -0.5, 0.5), DirectX::SimpleMath::Vector4(0, 0, 1, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(0.5, 0.5, 0.5), DirectX::SimpleMath::Vector4(0, 0, 0, 1) }
			};
		} vertexBufferData;

		VertexBuffer<decltype(vertexBufferData)> vertexBuffer;

		struct IndexBufferData {
			static inline DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;

			UINT data[6] = {
				0, 2, 1,
				2, 3, 1
			};
		} indexBufferData;

		IndexBuffer<decltype(indexBufferData)> indexBuffer;

		struct SModelMat {
			DirectX::SimpleMath::Matrix model;
		} modelMat;

		ConstantBuffer<decltype(modelMat)> constBuf;
	};
}