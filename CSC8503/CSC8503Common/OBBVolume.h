#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
namespace NCL {
	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type = VolumeType::OBB;
			halfSizes = halfDims;
		}
		OBBVolume(const Maths::Vector3& halfDims, Quaternion _rotation) {
			type		= VolumeType::OBB;
			halfSizes	=  halfDims;
			rotation    = _rotation;
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

		Quaternion  rotation;
		string  name;
		Maths::Vector3 halfSizes;
	};
}

