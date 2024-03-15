#pragma once

#include "../../../RenderData/ConstantBuffer.h"
#include "../../Transform.h"

namespace dmbrn
{
	class Camera
	{
	public:
		bool perspective = true;
		float FovAngleY = DirectX::XMConvertToRadians(90);
		float AspectRatio = 1.6; // width to height
		float NearZ = 0.0001;
		float FarZ = 1000;

		TransformComponent transform;

		Camera(DeviceWrapper& device, const TransformComponent& trans)
			: transform(trans)
			, viewCB(device.getDevice(), vpMat)
		{
			perspective = true;
			updateCamera();
		}

		void renderdataUpdate(DeviceWrapper& device)
		{
			auto mat = viewCB.map(device.getContext());

			//TODO actually 3x3 part can be transposed and translation added afer
			mat->viewproj = transform.getMatrix().Invert() * proj;

			mat->viewproj = mat->viewproj.Transpose();
		}

		void bindCB(DeviceWrapper& device)
		{
			viewCB.bindToVertex(device.getContext(), 0);
		}

		void updateCamera()
		{
			if (perspective)
				updatePerspProj();
			else
				updateOrthoProj();
		}

	private:

		struct VPMat {
			DirectX::SimpleMath::Matrix viewproj;
		} vpMat;

		DirectX::SimpleMath::Matrix proj;

		ConstantBuffer<decltype(vpMat)> viewCB;

		// you should set FovAngleY, AspectRatio, NearZ, FarZ by direct access
		void updateOrthoProj()
		{
			float    SinFov;
			float    CosFov;
			DirectX::XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

			float Height = CosFov / SinFov;
			float Width = Height * AspectRatio;

			proj = DirectX::XMMatrixOrthographicLH(Width, Height, NearZ, FarZ);
		}

		// you should set FovAngleY, AspectRatio, NearZ, FarZ by direct access
		void updatePerspProj()
		{
			proj = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ);
		}
	};
}