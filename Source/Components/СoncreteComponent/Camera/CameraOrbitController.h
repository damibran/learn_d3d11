#pragma once

#include "../../IGameComponent.h"
#include "../../Transform.h"
#include "Camera.h"

namespace dmbrn
{
	class CameraOrbitController :public IGameComponent
	{
	public:

		float mouseSens = 1;

		CameraOrbitController(GameToComponentBridge bridge, TransformComponent* cntr)
			: IGameComponent(bridge),
			camera(bridge.device, TransformComponent{}),
			center(cntr)
		{
			bridge.window.getInput().MouseMove.AddRaw(this, &CameraOrbitController::onMouseMove);
			camera.transform.position = center->position + radius * DirectX::SimpleMath::Vector3::Forward;
		}

		// Inherited via IGameComponent
		void Update(float dt) override
		{
			camera.transform.position = center->position + radius * (camera.transform.getRotationMatrix().Forward());
		}

		void RenderDataUpdate() override
		{
			camera.renderdataUpdate(bridge.device);
		}

		// camera controller should always be drawn before everything else!
		void Draw() override
		{
			camera.bindCB(bridge.device);
		}

	private:
		Camera camera;
		TransformComponent* center;

		float radius = 4;

		void onMouseMove(const InputDevice::MouseMoveEventArgs& args)
		{
			if (!bridge.window.getInput().IsKeyDown(Keys::MiddleButton)) return;

			bridge.window.lockCursor();

			DirectX::SimpleMath::Matrix cm = camera.transform.getRotationMatrix();

			float mul = 0.001;

			DirectX::SimpleMath::Vector3 rot;

			rot.y = args.Offset.x * 0.003 * mouseSens;
			rot.x = args.Offset.y * 0.003 * mouseSens;

			camera.transform.rotate(rot);

			if (args.WheelDelta > 0) radius *= 1.1;
			if (args.WheelDelta < 0) radius *= 0.9;
		}
	};
}