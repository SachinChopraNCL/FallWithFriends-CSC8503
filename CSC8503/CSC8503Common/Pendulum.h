#include "GameObject.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class Pendulum : public GameObject {
		public:
			Pendulum(float swingSpeed, string name, Vector3 initialPosition, float offset) :GameObject(name) { 
				this->swingSpeed = swingSpeed; 
				counter = 1;
				stateMachine = new StateMachine();
				pivot = initialPosition;
				pivot.y += offset;

				std::cout << pivot << std::endl;
				State* stateA = new State([&](float dt) -> void
					{
						this->SwingLeft(dt);
					}
				);

				State* stateB = new State([&](float dt) -> void
					{
						this->SwingRight(dt);
					}
				);
				stateMachine->AddState(stateA);
				stateMachine->AddState(stateB);

				stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool
					{
						return this->counter > 2;
					}
				));

				stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool
					{
						return this->counter < 0.0f;
					}
				));
			}
			void update(float dt) override;
			float swingSpeed;

			void SwingLeft(float dt);
			void SwingRight(float dt);
			StateMachine* stateMachine;
			Vector3 pivot;
			float counter;
		};

	}
}