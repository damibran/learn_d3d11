#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#include <d3dcompiler.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Components/IGameComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class TriangleComponent :public IGameComponent
	{
	public:
		TriangleComponent(GameToComponentBridge bridge, const std::wstring& shaderPath)
			: IGameComponent(bridge), shaderPath(shaderPath) {}

		TriangleComponent(GameToComponentBridge bridge, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 offset)
			: IGameComponent(bridge), shaderPath(shaderPath)
		{
			points[0].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[1].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[2].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
		}

		TriangleComponent(GameToComponentBridge bridge, const std::wstring& shaderPath, float angleDeg, DirectX::SimpleMath::Vector2 offset)
			: IGameComponent(bridge), shaderPath(shaderPath)
		{
			points[0].pos = DirectX::SimpleMath::Vector4(std::cosf(DirectX::XMConvertToRadians(angleDeg)) / 2., std::sinf(DirectX::XMConvertToRadians(angleDeg)) / 2., 0., 1.);
			points[1].pos = DirectX::SimpleMath::Vector4(std::cosf(DirectX::XMConvertToRadians(angleDeg + 120)) / 2., std::sinf(DirectX::XMConvertToRadians(angleDeg + 120)) / 2., 0., 1.);
			points[2].pos = DirectX::SimpleMath::Vector4(std::cosf(DirectX::XMConvertToRadians(angleDeg + 240)) / 2., std::sinf(DirectX::XMConvertToRadians(angleDeg + 240)) / 2., 0., 1.);



			points[0].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[1].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
			points[2].pos += DirectX::SimpleMath::Vector4(offset.x, offset.y, 0., 0.);
		}
		// Inherited via IGameComponent

		void Initialize() override
		{
			ID3DBlob* errorVertexCode = nullptr;
			HRESULT res = D3DCompileFromFile(shaderPath.c_str(),
				nullptr /*macros*/,
				nullptr /*include*/,
				"VSMain",
				"vs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0,
				&vertexShaderByteCode,
				&errorVertexCode);

			if (FAILED(res)) {
				// If the shader failed to compile it should have written something to the error message.
				if (errorVertexCode) {
					char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

					std::cout << compileErrors << std::endl;

					assert(false);
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
			}

			//D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

			D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShaderByteCode, nullptr);

			bridge.device.getDevice()->CreateVertexShader(
				vertexShaderByteCode->GetBufferPointer(),
				vertexShaderByteCode->GetBufferSize(),
				nullptr, &vertexShader);

			bridge.device.getDevice()->CreatePixelShader(
				pixelShaderByteCode->GetBufferPointer(),
				pixelShaderByteCode->GetBufferSize(),
				nullptr, &pixelShader);

			D3D11_INPUT_ELEMENT_DESC inputElements[] = {
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

			bridge.device.getDevice()->CreateInputLayout(
				inputElements,
				2,
				vertexShaderByteCode->GetBufferPointer(),
				vertexShaderByteCode->GetBufferSize(),
				&layout);

			D3D11_BUFFER_DESC vertexBufDesc = {};
			vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufDesc.CPUAccessFlags = 0;
			vertexBufDesc.MiscFlags = 0;
			vertexBufDesc.StructureByteStride = 0;
			vertexBufDesc.ByteWidth = sizeof(Vertex) * std::size(points);

			D3D11_SUBRESOURCE_DATA vertexData = {};
			vertexData.pSysMem = points;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			bridge.device.getDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

			CD3D11_RASTERIZER_DESC rastDesc = {};
			rastDesc.CullMode = D3D11_CULL_NONE;
			rastDesc.FillMode = D3D11_FILL_SOLID;
			bridge.device.getDevice()->CreateRasterizerState(&rastDesc, &rastState);
		}

		void Update(float dt) override
		{
			// what now here is nothing to do ((
		}

		void Draw() override
		{
			bridge.device.getContext()->RSSetState(rastState);

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			bridge.device.getContext()->IASetInputLayout(layout);
			UINT strides[] = { sizeof(Vertex) };
			UINT offsets[] = { 0 };
			bridge.device.getContext()->IASetVertexBuffers(0, 1, &vb, strides, offsets);
			bridge.device.getContext()->VSSetShader(vertexShader, nullptr, 0);
			bridge.device.getContext()->PSSetShader(pixelShader, nullptr, 0);

			bridge.device.getContext()->Draw(3, 0);
		}

		void DestroyResources() override
		{
			//TODO: release all
			rastState->Release();
			vb->Release();
			layout->Release();
			pixelShader->Release();
			vertexShader->Release();
			pixelShaderByteCode->Release();
			vertexShaderByteCode->Release();
		}

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