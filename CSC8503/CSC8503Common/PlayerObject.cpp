#include "PlayerObject.h"

void NCL::CSC8503::PlayerObject::update(float dt) {
	Vector3 forward = this->GetTransform().GetOrientation() * Vector3(0, 0, -1);

	float force = this->GetPhysicsObject()->collidingWithFloor == true ? 275 : 80;

	
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		this->GetPhysicsObject()->AddForce(forward * force);
	}
	
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		this->GetPhysicsObject()->AddTorque(Vector3(0, 550, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		this->GetPhysicsObject()->AddTorque(Vector3(0, -550,0 ));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE) && this->GetPhysicsObject()->collidingWithFloor == true) {
		this->GetPhysicsObject()->AddForce(Vector3(0, 750, 0));
	}


	if (this->GetTransform().GetPosition().y < -100) {
		this->GetTransform().SetPosition(Vector3(100, 40, 1070));
	}

}
