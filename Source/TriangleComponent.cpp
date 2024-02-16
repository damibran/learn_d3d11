#include "TriangleComponent.h"

#include <iostream>
#include <d3dcompiler.h>
#include "Game.h"

void dmbrn::TriangleComponent::Initialize()
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
			"POS",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	game.device.getDevice()->CreateInputLayout(
		inputElements,
		1,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&layout);

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	game.device.getDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	game.device.getDevice()->CreateRasterizerState(&rastDesc, &rastState);
}

void dmbrn::TriangleComponent::Update()
{
}

void dmbrn::TriangleComponent::Draw()
{
	game.device.getContext()->RSSetState(rastState);

	game.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	game.device.getContext()->IASetInputLayout(layout);
	UINT strides[] = { sizeof(DirectX::XMFLOAT4) };
	UINT offsets[] = { 0 };
	game.device.getContext()->IASetVertexBuffers(0, 1, &vb, strides, offsets);
	game.device.getContext()->VSSetShader(vertexShader, nullptr, 0);
	game.device.getContext()->PSSetShader(pixelShader, nullptr, 0);

	game.device.getContext()->Draw(3, 0);
}

void dmbrn::TriangleComponent::DestroyResources()
{
	//TODO: release all
}