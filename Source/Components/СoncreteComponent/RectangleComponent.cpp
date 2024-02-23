#include "RectangleComponent.h"

#include <iostream>
#include <d3dcompiler.h>
#include "Game.h"

namespace dmbrn
{
	void RectangleComponent::Initialize()
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

		game.device.getDevice()->CreateVertexShader(
			vertexShaderByteCode->GetBufferPointer(),
			vertexShaderByteCode->GetBufferSize(),
			nullptr, &vertexShader);

		game.device.getDevice()->CreatePixelShader(
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

		game.device.getDevice()->CreateInputLayout(
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

		game.device.getDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

		// Fill in a buffer description.
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(UINT) * std::size(indices);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		game.device.getDevice()->CreateBuffer(&bufferDesc, &InitData, &indexBuffer);

		CD3D11_RASTERIZER_DESC rastDesc = {};
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		game.device.getDevice()->CreateRasterizerState(&rastDesc, &rastState);
	}

	void RectangleComponent::Update(float dt)
	{
		// what now here is nothing to do ((
	}

	void RectangleComponent::Draw()
	{
		game.device.getContext()->RSSetState(rastState);

		game.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		game.device.getContext()->IASetInputLayout(layout);
		UINT strides[] = { sizeof(Vertex) };
		UINT offsets[] = { 0 };
		game.device.getContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
		game.device.getContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		game.device.getContext()->VSSetShader(vertexShader, nullptr, 0);
		game.device.getContext()->PSSetShader(pixelShader, nullptr, 0);

		game.device.getContext()->DrawIndexed(6, 0, 0);
	}

	void RectangleComponent::DestroyResources()
	{
		//TODO: release all
		rastState->Release();
		indexBuffer->Release();
		vertexBuffer->Release();
		layout->Release();
		pixelShader->Release();
		vertexShader->Release();
		pixelShaderByteCode->Release();
		vertexShaderByteCode->Release();
	}
}