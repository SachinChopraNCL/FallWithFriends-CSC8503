#include "Barrier.h"
#include "OBBVolume.h"

void NCL::CSC8503::Barrier::update(float dt) {

	if (GetTransform().GetScale().y > 35 ) {
		direction = 0;
	}

	if (GetTransform().GetScale().y <= 0 && direction == 0) {
		direction = 1;
	}

	if (direction == 1) { this->GetTransform().SetScale(GetTransform().GetScale() + Vector3(0, speed, 0)); }

	if (direction == 0) { this->GetTransform().SetScale(GetTransform().GetScale() - Vector3(0, speed, 0)); }
	
	OBBVolume* volume = (OBBVolume*)this->GetBoundingVolume();
	volume->halfSizes = this->GetTransform().GetScale() / 2;
}
