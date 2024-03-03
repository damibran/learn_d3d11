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

		}

		// Inherited via IGameComponent
		void Update(float dt) override
		{
			
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

		void onMouseMove(const InputDevice::MouseMoveEventArgs& args)
		{
			if (bridge.window.getInput().IsKeyDown(Keys::LeftShift)) return;

			bridge.window.lockCursor();


		}
	};
}