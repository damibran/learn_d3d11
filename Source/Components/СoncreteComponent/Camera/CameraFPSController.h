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
				velocity.z += 1;
			if (bridge.window.getInput().IsKeyDown(Keys::S))
				velocity.z -= 1;
			if (bridge.window.getInput().IsKeyDown(Keys::A))
				velocity.x -= 1;
			if (bridge.window.getInput().IsKeyDown(Keys::D))
				velocity.x += 1;
			if (bridge.window.getInput().IsKeyDown(Keys::Q))
				velocity.y -= 1;
			if (bridge.window.getInput().IsKeyDown(Keys::E))
				velocity.y += 1;

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

			rot.y = args.Offset.x * 0.003 * mouseSens;
			rot.x = args.Offset.y * 0.003 * mouseSens;

			camera.transform.rotate(rot);

			if (args.WheelDelta > 0) velocityMultipler *= 1.1;
			if (args.WheelDelta < 0) velocityMultipler *= 0.9;
		}
	};
}