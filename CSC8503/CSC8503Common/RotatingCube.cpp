#include "RotatingCube.h"
#include "OBBVolume.h"

void NCL::CSC8503::RotatingCube::update(float dt){
	this->GetTransform().SetOrientation(GetTransform().GetOrientation() * Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), dt * rotateSpeed));
	OBBVolume* vol = (OBBVolume*)this->GetBoundingVolume();
	vol->rotation = this->GetTransform().GetOrientation();
}
