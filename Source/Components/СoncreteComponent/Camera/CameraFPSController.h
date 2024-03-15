#pragma once

#include "Camera.h"
#include "../../IGameComponent.h"

namespace dmbrn
{
	class CameraFPSControllerComponent :public IGameComponent
	{
	public:

		float mouseSens = 1;
		float velocityMultipler = 1;

		CameraFPSControllerComponent(GameToComponentBridge bridge) :
			IGameComponent(bridge),
			camera(bridge.device, TransformComponent{})
		{
			dh = bridge.window.getInput().MouseMove.AddRaw(this, &CameraFPSControllerComponent::onMouseMove);
		}

		~CameraFPSControllerComponent()
		{
			bridge.window.getInput().MouseMove.Remove(dh);
		}

		// Inherited via IGameComponent
		void Update(float dt) override
		{
			DirectX::SimpleMath::Vector3 velocity;

			if (bridge.window.getInput().IsKeyDown(Keys::W))
				velocity += DirectX::SimpleMath::Vector3::Forward;
			if (bridge.window.getInput().IsKeyDown(Keys::S))
				velocity += DirectX::SimpleMath::Vector3::Backward;
			if (bridge.window.getInput().IsKeyDown(Keys::A))
				velocity += DirectX::SimpleMath::Vector3::Left;
			if (bridge.window.getInput().IsKeyDown(Keys::D))
				velocity += DirectX::SimpleMath::Vector3::Right;
			if (bridge.window.getInput().IsKeyDown(Keys::Q))
				velocity += DirectX::SimpleMath::Vector3::Down;
			if (bridge.window.getInput().IsKeyDown(Keys::E))
				velocity += DirectX::SimpleMath::Vector3::Up;

			velocity = velocityMultipler * DirectX::SimpleMath::Vector3::Transform(velocity, camera.transform.getRotationMatrix());

			camera.transform.translate(dt * velocity);
		}

		void RenderDataUpdate() override
		{
			camera.renderdataUpdate(bridge.device);
		}

		void Draw() override
		{
			camera.bindCB(bridge.device);
		}

		Camera camera;
	private:
		DelegateHandle dh;

		void onMouseMove(const InputDevice::MouseMoveEventArgs& args)
		{
			if (!bridge.window.getInput().IsKeyDown(Keys::RightButton)) return;

			bridge.window.lockCursor();

			DirectX::SimpleMath::Vector3 rot;

			rot.y = -args.Offset.x * 0.003 * mouseSens;
			rot.x = -args.Offset.y * 0.003 * mouseSens;

			camera.transform.rotate(rot);

			if (args.WheelDelta > 0) velocityMultipler *= 1.1;
			if (args.WheelDelta < 0) velocityMultipler *= 0.9;
		}
	};
}