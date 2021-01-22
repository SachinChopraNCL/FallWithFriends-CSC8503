#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Barrier : public GameObject {
		public:
			Barrier(float speed, string name) : GameObject(name) { this->speed = speed; }
			void update(float dt) override;
			float speed;
			float elapsed = 0;
			int direction = 1;
		};
	}
}