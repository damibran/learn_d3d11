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
		float NearZ = 0.01;
		float FarZ = 100;

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

			mat->viewproj = getView() * proj;
			mat->viewproj = mat->viewproj.Transpose();

			mat->world_pos = transform.position;
		}

		void bindCB(DeviceWrapper& device)
		{
			viewCB.bindToVertex(device.getContext(), 0);
			viewCB.bindToFragment(device.getContext(), 0);
		}

		void updateCamera()
		{
			if (perspective)
				updatePerspProj();
			else
				updateOrthoProj();
		}

		DirectX::SimpleMath::Matrix getView()const 
		{
			//TODO actually 3x3 part can be transposed and negated translation added after
			return transform.getMatrix().Invert();
		}

	private:

		struct alignas(16) VPMat
		{
			DirectX::SimpleMath::Matrix viewproj;
			DirectX::SimpleMath::Vector3 world_pos;
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

			proj = DirectX::SimpleMath::Matrix::CreateOrthographic(Width, Height, NearZ, FarZ);
		}

		// you should set FovAngleY, AspectRatio, NearZ, FarZ by direct access
		void updatePerspProj()
		{
			proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(FovAngleY, AspectRatio, NearZ, FarZ);
		}
	};
}