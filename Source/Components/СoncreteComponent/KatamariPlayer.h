#pragma once

#include "Components/СoncreteComponent/ModelComponent.h"
#include "Components/СoncreteComponent/Camera/CameraOrbitController.h"
#include "Components/СoncreteComponent/KatamaryCollectable.h"

namespace dmbrn
{
	class KatamariPlayer :public ModelComponent
	{
	public:
		KatamariPlayer(GameToComponentBridge bridge, RastState& rs, InputLayout<VertexType>* il, CameraOrbitController& cam, InputDevice& inputDevice, const std::vector<KatamaryCollectable*>& kc) :
			ModelComponent(bridge, rs, il, L"./Shaders/ModelShader.hlsl", L"Models\\Barrel\\Barrel.dae"),
			camera(cam),
			input_device_(inputDevice),
			game_collectables_(kc)
		{
			cam.setCenterTransform(&transform);
			collected.resize(game_collectables_.size());
			BS.Center = localAABB.Center;
			BS.Radius = std::max(localAABB.Extents.x, std::max(localAABB.Extents.y, localAABB.Extents.z));
		}

		void Update(float dt) override
		{
			bool move = false;
			ModelComponent::Update(dt);

			DirectX::SimpleMath::Vector3 velocity;
			DirectX::SimpleMath::Matrix cam_matrix = camera.camera.transform.getRotationMatrix();

			DirectX::SimpleMath::Vector3 cam_f_ZX = DirectX::SimpleMath::Vector3{ cam_matrix.Forward().x,0,cam_matrix.Forward().z };
			cam_f_ZX.Normalize();

			if (input_device_.IsKeyDown(Keys::W))
			{
				move = true;
				velocity += cam_f_ZX;
			}
			if (input_device_.IsKeyDown(Keys::S))
			{
				move = true;
				velocity -= cam_f_ZX;
			}
			if (input_device_.IsKeyDown(Keys::A))
			{
				move = true;
				velocity += cam_matrix.Left();
			}
			if (input_device_.IsKeyDown(Keys::D))
			{
				move = true;
				velocity += cam_matrix.Right();
			}

			velocity.Normalize();

			transform.position += dt * move_speed * velocity;
			camera.Update(dt);


			if (move)
			{
				auto right = velocity.Cross(DirectX::SimpleMath::Vector3::Up);
				auto updateRot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(right, -dt * rot_speed);

				transform.rotate(updateRot);

				BS.Center = localAABB.Center + transform.position;

				for (int i = 0; i < game_collectables_.size(); ++i)
				{
					if (!collected[i].first && BS.Intersects(game_collectables_[i]->getAABB()))
					{
						collected[i].first = true;
						DirectX::SimpleMath::Matrix worldToThis = transform.getMatrix().Invert();
						collected[i].second = game_collectables_[i]->transform.position;
						collected[i].second = DirectX::SimpleMath::Vector3::Transform(collected[i].second, worldToThis); // so than we have position in local frame (this)
					}
				}

				for (int i = 0; i < collected.size(); ++i)
				{
					if (collected[i].first)
					{
						auto& childTrands = game_collectables_[i]->transform;
						DirectX::SimpleMath::Matrix thisToWorld = transform.getMatrix();

						childTrands.position = DirectX::SimpleMath::Vector3::Transform(collected[i].second, thisToWorld);
						childTrands.rotate(updateRot);
					}
				}
			}
		}

		void RenderDataUpdate() override
		{
			ModelComponent::RenderDataUpdate();
		}
		void Draw() override
		{
			ModelComponent::Draw();
		}

		float move_speed = 10;
		float rot_speed = 10;
	private:
		CameraOrbitController& camera;
		InputDevice& input_device_;
		std::vector < std::pair<bool, DirectX::SimpleMath::Vector3> > collected;
		std::vector<KatamaryCollectable*> game_collectables_;
		DirectX::BoundingSphere BS;
	};
}
