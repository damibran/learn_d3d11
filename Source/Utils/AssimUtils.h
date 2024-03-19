#pragma once

#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

namespace dmbrn
{
	inline DirectX::SimpleMath::Vector3 toD3d(const aiVector3D& vec)
	{
		return DirectX::SimpleMath::Vector3{ vec.x,vec.y,vec.z };
	}

	inline DirectX::SimpleMath::Quaternion toD3d(const aiQuaternion& vec)
	{
		return DirectX::SimpleMath::Quaternion{ vec.x,vec.y,vec.z,vec.w };
	}
	
	// row major to row major
	// also as i get it assimp math vector is column (translation is in last column) so m*v and it needs to be transposed
	inline DirectX::SimpleMath::Matrix toD3d(const aiMatrix4x4& omat)
	{
		//return DirectX::SimpleMath::Matrix
		//{
		//	mat.a1, mat.b1, mat.c1, mat.d1,
		//	mat.a2, mat.b2, mat.c2, mat.d2,
		//	mat.a3, mat.b3, mat.c3, mat.d3,
		//	mat.a4, mat.b4, mat.c4, mat.d4,
		//};
		aiMatrix4x4 mat=omat;
		mat.Transpose();
		return DirectX::SimpleMath::Matrix
		{
			mat.a1, mat.a2, mat.a3, mat.a4,
			mat.b1, mat.b2, mat.b3, mat.b4,
			mat.c1, mat.c2, mat.c3, mat.c4,
			mat.d1, mat.d2, mat.d3, mat.d4,
		};
	}

	//glm::vec3 getScale(const glm::mat4 mat);

	//glm::vec3 operator/(const glm::vec3& vec, const float s);
}