#pragma once
#include "GameObject.h"
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
namespace NCL {
	namespace CSC8503 {
		class PlayerObject : public GameObject	{
		public:
			PlayerObject(string name) : GameObject(name) {}
			void update(float dt) override;
		};
	}
}
