#include "RacketComponent.h"

#include <iostream>
#include <d3dcompiler.h>
#include "Game.h"

namespace dmbrn
{

	RacketComponent::RacketComponent(Game& game, RastState& rs,const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
		DirectX::SimpleMath::Vector2 offset /*= DirectX::SimpleMath::Vector2(0, 0)*/, Keys key_up /*= Keys::W*/, Keys key_down /*= Keys::S*/) :
		IGameComponent(game), 
		rastState(rs),
		shaders(game.device.getDevice(), shaderPath),
		vertexBuffer(game.device.getDevice(), shaders.getVertexBC(), vertexBufferData),
		indexBuffer(game.device.getDevice(),indexBufferData),
		constBuf(game.device.getDevice(),modelMat),
		scale(scale), translation(offset), keyUp(key_up), keyDown(key_down)
	{
		modelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

		initialAABB.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
		initialAABB.Extents = DirectX::SimpleMath::Vector3(0.5, 0.5, 1);
		initialAABB.Transform(currentAABB, modelMat.model);
	}

	void RacketComponent::Initialize()
	{	

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
		auto mat = constBuf.map(game.device.getContext());

		mat->model =
			DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0));

		mat->model = mat->model.Transpose();

		constBuf.upmap(game.device.getContext());
	}

	void RacketComponent::Draw()
	{
		rastState.bind(game.device.getContext());

		game.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		vertexBuffer.bindVertexBuffer(game.device.getContext());
		indexBuffer.bind(game.device.getContext());
		shaders.bindShaders(game.device.getContext());

		constBuf.bind(game.device.getContext(), 1);

		game.device.getContext()->DrawIndexed(6, 0, 0);
	}

	void RacketComponent::DestroyResources()
	{
		//TODO: release all
	}

}