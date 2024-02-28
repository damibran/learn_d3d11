#include "RacketComponent.h"

#include <iostream>
#include <d3dcompiler.h>
#include "Game.h"

namespace dmbrn
{
	void RacketComponent::Initialize()
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

		vertexBuffer.Initialize(game.device.getDevice(), vertexShaderByteCode, vertexBufferData);

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

		// Fill in a buffer description.
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(SModelMat);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		InitData.pSysMem = &sModelMat;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		game.device.getDevice()->CreateBuffer(&cbDesc, &InitData,
			&constantBufferModel);
	}

	void RacketComponent::Update(float dt)
	{
		if (game.window.getInput().IsKeyDown(keyUp))
		{
			translation.y += dt * 1;
		}
		if (game.window.getInput().IsKeyDown(keyDown))
		{
			translation.y -= dt * 1;
		}
	}

	void RacketComponent::PhysicsUpdate(float)
	{
		initialAABB.Transform(currentAABB, DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0)));
	}

	void RacketComponent::CollisionUpdate(float dt)
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

	void RacketComponent::RenderDataUpdate()
	{
		D3D11_MAPPED_SUBRESOURCE res = {};
		game.device.getContext()->Map(constantBufferModel, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

		auto mat = reinterpret_cast<SModelMat*>(res.pData);
		mat->model =
			DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0));

		mat->model = mat->model.Transpose();

		game.device.getContext()->Unmap(constantBufferModel, 0);
	}

	void RacketComponent::Draw()
	{
		game.device.getContext()->RSSetState(rastState);

		game.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		vertexBuffer.bindVertexBuffer(game.device.getContext());
		game.device.getContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		game.device.getContext()->VSSetShader(vertexShader, nullptr, 0);
		game.device.getContext()->PSSetShader(pixelShader, nullptr, 0);

		game.device.getContext()->VSSetConstantBuffers(0, 1, &constantBufferModel);

		game.device.getContext()->DrawIndexed(6, 0, 0);
	}

	void RacketComponent::DestroyResources()
	{
		//TODO: release all
		constantBufferModel->Release();
		rastState->Release();
		indexBuffer->Release();
		vertexBuffer.Destroy();
		pixelShader->Release();
		vertexShader->Release();
		pixelShaderByteCode->Release();
		vertexShaderByteCode->Release();
	}

}