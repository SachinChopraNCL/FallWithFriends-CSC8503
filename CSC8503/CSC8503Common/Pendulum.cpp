#include "Pendulum.h"
float angle = 0;
void NCL::CSC8503::Pendulum::update(float dt){
	stateMachine->Update(dt);

}

void NCL::CSC8503::Pendulum::SwingLeft(float dt) {
	angle = 1 * dt * swingSpeed;
	Vector3 pos = transform.GetPosition();
	float x = pivot.x + ((pos.x - pivot.x) * cos(angle)) - ((pos.y - pivot.y) * sin(angle));
	float y = pivot.y + ((pos.x - pivot.x) * sin(angle)) + ((pos.y - pivot.y) * cos(angle));
	pos.x = x;
	pos.y = y;
	transform.SetPosition(pos);
	counter += dt;
}

void NCL::CSC8503::Pendulum::SwingRight(float dt) {
	angle = -1 * dt * swingSpeed;
	Vector3 pos = transform.GetPosition();
	float x = pivot.x + ((pos.x - pivot.x) * cos(angle)) - ((pos.y - pivot.y) * sin(angle));
	float y = pivot.y + ((pos.x - pivot.x) * sin(angle)) + ((pos.y - pivot.y) * cos(angle));
	pos.x = x;
	pos.y = y;
	transform.SetPosition(pos);
	counter -= dt;
}




