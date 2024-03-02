#pragma once

#include "../../../RenderData/ConstantBuffer.h"
#include "../../Transform.h"

namespace dmbrn
{
	class Camera
	{
	public:
		TransformComponent transform;

		Camera(DeviceWrapper& device, const TransformComponent& trans)
			: transform(trans)
			, viewCB(device.getDevice(), vpMat)
		{
			//setOrthoProj(4, 4, 0.001, 1);
			setPerspProj(DirectX::XMConvertToRadians(90), 1.6, 0.001, 1000);
		}

		void renderdataUpdate(DeviceWrapper& device)
		{
			auto mat = viewCB.map(device.getContext());

			mat->viewproj = transform.getMatrix().Invert()*proj;

			mat->viewproj = mat->viewproj.Transpose();

			viewCB.upmap(device.getContext());
		}

		void bindCB(DeviceWrapper& device)
		{
			viewCB.bind(device.getContext(), 0);
		}

		void setOrthoProj(
			float ViewWidth,
			float ViewHeight,
			float NearZ,
			float FarZ)
		{
			proj = DirectX::XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ);
		}

		void setPerspProj(
			float FovAngleY,
			float AspectRatio,
			float NearZ,
			float FarZ)
		{
			proj = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ);
		}

	private:

		struct VPMat {
			DirectX::SimpleMath::Matrix viewproj;
		} vpMat;

		DirectX::SimpleMath::Matrix proj;

		ConstantBuffer<decltype(vpMat)> viewCB;
	};
}