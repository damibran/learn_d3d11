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
			ModelComponent(bridge, rs, il, L"./Shaders/LitModelShader.hlsl", L"Models\\Barrel\\Barrel.dae"),
			camera(cam),
			input_device_(inputDevice),
			to_collect(kc)
		{
			cam.setCenterTransform(&transform);
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


			if (velocity.Length() > 0)
			{
				auto right = velocity.Cross(DirectX::SimpleMath::Vector3::Up);

				auto updateRot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(right, -dt * rot_speed);

				transform.rotate(updateRot);

				BS.Center = localAABB.Center + transform.position;

				for (auto it = to_collect.begin(); it != to_collect.end(); ++it)
				{
					if (BS.Intersects((*it)->getAABB()))
					{
						DirectX::SimpleMath::Matrix worldToThis = transform.getMatrix().Invert();
						DirectX::SimpleMath::Vector3 localPos = (*it)->transform.position;
						localPos = DirectX::SimpleMath::Vector3::Transform(localPos, worldToThis); // so than we have position in local frame (this)

						localPos = 3. * localPos / 4.;
						BS.Radius *= 1.1;
						transform.position.y *= 1.1;

						collected.push_back(std::make_pair(std::move(*it), localPos));
						to_collect.erase(it);
						break;
					}
				}

				for (auto&& pair : collected)
				{
					auto& childTrands = pair.first->transform;
					DirectX::SimpleMath::Matrix thisToWorld = transform.getMatrix();

					childTrands.position = DirectX::SimpleMath::Vector3::Transform(pair.second, thisToWorld);
					childTrands.rotate(updateRot);
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
		std::vector < std::pair<KatamaryCollectable*, DirectX::SimpleMath::Vector3> > collected;
		std::vector<KatamaryCollectable*> to_collect;
		DirectX::BoundingSphere BS;
	};
}
