#pragma once

#include "Components/СoncreteComponent/ModelComponent.h"
#include "Components/СoncreteComponent/Camera/CameraOrbitController.h"

namespace dmbrn
{
	class KatamariPlayer :public ModelComponent
	{
	public:
		KatamariPlayer(GameToComponentBridge bridge, RastState& rs, InputLayout<VertexType>* il, CameraOrbitController& cam, InputDevice& inputDevice) :
			ModelComponent(bridge, rs, il, L"./Shaders/ModelShader.hlsl", L"Models\\Barrel\\Barrel.dae"),
			camera(cam),
			input_device_(inputDevice)
		{
			cam.setCenterTransform(&transform);
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
				transform.rotate(DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(cam_matrix.Right(), -dt * rot_speed));
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
	};
}
