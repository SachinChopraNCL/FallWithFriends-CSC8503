#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class RotatingCube : public GameObject {
		public:
			RotatingCube(float rotateSpeed, string name):GameObject(name){ this->rotateSpeed = rotateSpeed; }
			void update(float dt) override;
			float rotateSpeed;
		};

	}
}