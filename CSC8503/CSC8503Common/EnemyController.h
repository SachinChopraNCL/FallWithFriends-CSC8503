#pragma once
#include "GameObject.h"
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
namespace NCL {
	namespace CSC8503 {
		class EnemyController : public GameObject {
		public:
			EnemyController(string name) : GameObject(name) {}
			void update(float dt) override;
			std::vector<Vector3> nodes;
			Vector3 currentNode;
			bool needsNodes = false;
			int index = 0;
			float resetTimer = 0;
			float jump = false;
		};
	}
}
