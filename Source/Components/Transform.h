#pragma once

#include <directxtk/SimpleMath.h>

namespace dmbrn {
	struct TransformComponent {
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 scale;

		// DirectX::SimpleMath::Matrix globalTransformMatrix = DirectX::SimpleMath::Matrix(1.0f); // hierarchy transform with self trans matrix

		//************************************
		// Parameter: DirectX::SimpleMath::Vector3 pos
		// Parameter: DirectX::SimpleMath::Vector3 rot pitch x, yaw y, roll z order YawPitchRoll
		// Parameter: DirectX::SimpleMath::Vector3 scale
		//************************************
		TransformComponent(DirectX::SimpleMath::Vector3 pos = { 0, 0, 0 },
			DirectX::SimpleMath::Vector3 rot = { 0, 0, 0 },
			DirectX::SimpleMath::Vector3 scale = { 1, 1, 1 })
			: position(pos)
			, scale(scale)
			, rotation{ DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rot) }
		{
		}

		[[nodiscard]] DirectX::SimpleMath::Matrix getRotationMatrix() const
		{
			return DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		}

		DirectX::SimpleMath::Matrix getMatrix() const
		{
			DirectX::SimpleMath::Matrix m = DirectX::SimpleMath::Matrix::CreateScale(scale) * getRotationMatrix();
			m.Translation(position);
			return m;
		}

		void translate(const DirectX::SimpleMath::Vector3& v)
		{
			position += v;
		}

		void rotate(const DirectX::SimpleMath::Vector3& r)
		{
			//auto qr = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(r);
			//rotation = DirectX::SimpleMath::Quaternion::Concatenate(rotation, qr);

			rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation.ToEuler() + r);
		}

		void rotate(const DirectX::SimpleMath::Quaternion& q)
		{
			//auto qr = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(r);
			//rotation = DirectX::SimpleMath::Quaternion::Concatenate(rotation, qr);

			rotation *= q;
		}

		void setRad(const DirectX::SimpleMath::Vector3 rad)
		{
			rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rad);
		}

		void setQuat(const DirectX::SimpleMath::Quaternion q)
		{
			rotation = q;
		}

		DirectX::SimpleMath::Vector3 getRotationRad() const
		{

			return rotation.ToEuler();
		}

	private:
		DirectX::SimpleMath::Quaternion rotation;
	};
}