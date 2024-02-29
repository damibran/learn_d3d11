#include "BallComponent.h"

#include <iostream>
#include "Game.h"

namespace dmbrn
{

	BallComponent::BallComponent(Game& game, RastState& rs, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
		const RacketComponent& lRckt, const RacketComponent& rRckt, DirectX::SimpleMath::Vector2 offset /*= DirectX::SimpleMath::Vector2(0, 0)*/, float spd /*= 0*/)
		: IGameComponent(game),
		rastState(rs),
		shaders(game.device.getDevice(), shaderPath),
		vertexBuffer(game.device.getDevice(), shaders.getVertexBC(), vertexBufferData),
		indexBuffer(game.device.getDevice(), indexBufferData),
		constBuf(game.device.getDevice(), modelMat),
		scale(scale), lRacket(lRckt), rRacket(rRckt), translation(offset), speed(spd)
	{
		modelMat.model = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0));

		initialBS.Center = DirectX::SimpleMath::Vector3(0, 0, 0);
		initialBS.Radius = 0.5 * scale.x; // for some reason transform for sphere doesnt apply scale !
		initialBS.Transform(currentBS, modelMat.model);
	}

	void BallComponent::Initialize()
	{

	}

	void BallComponent::Update(float dt)
	{
		translation += dt * speed * moveDir;
	}

	void BallComponent::PhysicsUpdate(float)
	{
		initialBS.Transform(currentBS, DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0)));
	}

	void BallComponent::CollisionUpdate(float dt)
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
			game.lRacketScore++;
			speed = 0.2;
			return;
		}
		else if (currentBS.Intersects(leftPlane))
		{
			translation = DirectX::SimpleMath::Vector2(0, 0);
			game.rRacketScore++;
			speed = 0.2;
			return;
		}
	}

	void BallComponent::RenderDataUpdate()
	{
		auto mat = constBuf.map(game.device.getContext());

		mat->model =
			DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 }) *
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(translation.x, translation.y, 0));

		mat->model = mat->model.Transpose();

		constBuf.upmap(game.device.getContext());
	}

	void BallComponent::Draw()
	{
		rastState.bind(game.device.getContext());

		game.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		vertexBuffer.bindVertexBuffer(game.device.getContext());
		indexBuffer.bind(game.device.getContext());
		shaders.bindShaders(game.device.getContext());

		constBuf.bind(game.device.getContext(), 1);

		game.device.getContext()->DrawIndexed(6, 0, 0);
	}

	void BallComponent::DestroyResources()
	{

	}
}