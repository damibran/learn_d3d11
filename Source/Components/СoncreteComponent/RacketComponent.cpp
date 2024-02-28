#include "RacketComponent.h"

#include <iostream>
#include <d3dcompiler.h>
#include "Game.h"

namespace dmbrn
{

	RacketComponent::RacketComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale, 
		DirectX::SimpleMath::Vector2 offset /*= DirectX::SimpleMath::Vector2(0, 0)*/, Keys key_up /*= Keys::W*/, Keys key_down /*= Keys::S*/) :
		IGameComponent(game), 
		shaders(game.device.getDevice(), shaderPath), 
		indexBuffer(game.device.getDevice(),indexBufferData),
		scale(scale), translation(offset), keyUp(key_up), keyDown(key_down)
	{
		sModelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

		initialAABB.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
		initialAABB.Extents = DirectX::SimpleMath::Vector3(0.5, 0.5, 1);
		initialAABB.Transform(currentAABB, sModelMat.model);
	}

	void RacketComponent::Initialize()
	{
		vertexBuffer.Initialize(game.device.getDevice(), shaders.getVertexBC(), vertexBufferData);

		

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
		D3D11_SUBRESOURCE_DATA InitData;
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
		indexBuffer.bind(game.device.getContext());
		shaders.bindShaders(game.device.getContext());

		game.device.getContext()->VSSetConstantBuffers(0, 1, &constantBufferModel);

		game.device.getContext()->DrawIndexed(6, 0, 0);
	}

	void RacketComponent::DestroyResources()
	{
		//TODO: release all
		constantBufferModel->Release();
		rastState->Release();
		vertexBuffer.Destroy();
	}

}