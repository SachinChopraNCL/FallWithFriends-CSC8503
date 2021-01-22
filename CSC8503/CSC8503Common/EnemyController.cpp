#include "EnemyController.h"

void NCL::CSC8503::EnemyController::update(float dt){

	float force = this->GetPhysicsObject()->collidingWithFloor == true ? 250 : 80;

	resetTimer += dt;
	std::cout << nodes.size() << std::endl;
	if (resetTimer >= 10 || (this->GetTransform().GetPosition().y < -100)) {
		resetTimer = 0;
		needsNodes = true;
	}
	else if (GetTransform().GetPosition().z + 1 < currentNode.z) {
		index++;
		resetTimer = 0;
	}
	else if (nodes.size() > 0) {
		Vector3 currentNode = nodes.at(index);
		bool xBias = abs(currentNode.x - transform.GetPosition().x) < 1;
		bool zBias = abs(currentNode.z - transform.GetPosition().z) < 1;

		if (xBias || zBias) {
			resetTimer = 0;
			index++;
		}

		if (this->GetPhysicsObject()->collidingWithFloor && jump) {
			this->GetPhysicsObject()->AddForce(Vector3(0, 850, 0));
		}
		

		Vector3 direction = currentNode - transform.GetPosition();
		direction = direction.Normalised();
		direction.y = 0;
		this->GetPhysicsObject()->AddForce(direction * force);

	}
}
