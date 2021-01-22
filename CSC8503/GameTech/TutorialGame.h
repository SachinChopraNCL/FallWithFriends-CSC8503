#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObject.h"
#include "../../Common/Assets.h"
#include "../CSC8503Common/NavigationGrid.h"

#include <vector>
#include <fstream>


namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame(int state, bool multiplayer = false);
			~TutorialGame();

			virtual void UpdateGame(float dt);
			int  gameStatus = 0;
			int  score = 1000;
			bool multiplayer = false;


		protected:
			void InitialiseAssets();
			void MainMenu();
			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void BridgeConstraint();
	
			bool SelectObject();
			void MoveSelectedObject();
				

			void AddMenu();

			void PathFind(Vector3 position, Vector3 endpoint);
			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& floorSize, float tilt, float direction, float type, string name, Vector4 colour, bool addToList = true);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddRotatingCube(const Vector3& position, Vector3 dimensions, Vector3 scale, float rotateSpeed, float inverseMass);
			void        AddRotatingTower(const Vector3& position, Vector3 dimensions, Vector3 scale, float rotateSpeed, float inverseMass);
			void        AddBarrier(const Vector3& position, Vector3& size, float direction, float speed, Vector4 colour);
			void        AddPushTile(const Vector3& position, Vector3& size, float direction, Vector4 colour);
			void		AddMotorTile(const Vector3& position, Vector3& size, float direction, Vector4 colour);
			void        AddPendulum(const Vector3& position, float radius, float speed, float inverseMass = 10.0f);
			void        LoadLevel();
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position, Vector3& scale);
			GameObject* finalPlatform;
			GameObject* startingPlatform;
			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
			GameObject*         enemy;

			
			bool useGravity;
			bool inSelectionMode;
			bool isJumping = false;
			bool tracking = false;
			std::vector<Vector3> testNodes;
			bool lose = false;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			StateGameObject* testStateObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;
			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;


			std::vector<GameObject*> updateList;
			std::vector<GameObject*> bonusList;
			std::vector<GameObject*> obstacleList;
			std::vector<GameObject*> floorList;
			std::vector<GameObject*> wallList;
			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

		};
	}
}

