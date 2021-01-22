#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/RotatingCube.h"
#include "../CSC8503Common/PlayerObject.h"
#include "../CSC8503Common/Barrier.h"
#include "../CSC8503Common/Pendulum.h"
#include "../CSC8503Common/EnemyController.h"


using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(int state, bool multiplayer)	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = true;
	this->multiplayer = multiplayer;
	gameStatus = state;
	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

float elapsedTime = 0;
void TutorialGame::UpdateGame(float dt) {
	if (gameStatus == 0) {
		elapsedTime += dt;

		if (multiplayer) {
			EnemyController* currentEnemy = (EnemyController*)enemy;
			currentEnemy->jump = false;

			if (currentEnemy->needsNodes) {
				currentEnemy->GetTransform().SetPosition(updateList.at(0)->GetTransform().GetPosition() + Vector3(0, 0, 10));
				PathFind(enemy->GetTransform().GetPosition() / 2, Vector3(50, 0, 0));
				currentEnemy->needsNodes = false;
				currentEnemy->resetTimer = 0;
				currentEnemy->index = 0;
			}

			Ray ray(currentEnemy->GetTransform().GetPosition() + Vector3(0,1,-2), Vector3(0,0,-1));
			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				if (closestCollision.rayDistance < 90) {
					currentEnemy->jump = true;
				}
			}

	}

		UpdateKeys();
		

		if (elapsedTime >= 1.0f  && finalPlatform->finished == false ) {
			score -= 10;
			elapsedTime = 0;
			if (score <= 0) { 
				score = 0; 
				gameStatus = 1;
			}
		}

		SelectObject();
		MoveSelectedObject();
		physics->Update(dt);

		Debug::Print("Score: " + std::to_string(score), Vector2(78, 5), Vector4(1, 1, 1, 1));

		for (GameObject* objects : updateList) {
			objects->update(dt);
		}

		if (finalPlatform->finished == true) {
			gameStatus = 1; 
			if (multiplayer) {
				if (!finalPlatform->playerCollision) {
					score = 0;
				}
			}
		}

		for (int i = 0; i < bonusList.size(); i++) {
			if (bonusList.at(i)->isAlive == false) {
				if (bonusList.at(i)->playerCollision){ score += 25; }

				std::vector<GameObject*>::iterator worldPosition = std::find(world->gameObjects.begin(), world->gameObjects.end(), bonusList.at(i));
				world->gameObjects.erase(worldPosition);

				std::vector<GameObject*>::iterator updatePosition = std::find(updateList.begin(), updateList.end(), bonusList.at(i));
				updateList.erase(updatePosition);

				bonusList.erase(bonusList.begin() + i);
			}
		}


	
		Vector3 camPos = updateList.at(0)->GetTransform().GetPosition() + Vector3(0, 90, 80);
		world->GetMainCamera()->SetPosition(camPos);
		//world->GetMainCamera()->UpdateCamera(dt);

		
	}
	else {

		SelectObject();
		if (gameStatus == 4) {
			if (score != 0) {
				Debug::Print("YOU WIN!", Vector2(33.5, 40));
			}
			else {
				Debug::Print("YOU LOSE!", Vector2(33.5, 40));
			}
			Debug::Print("FINAL SCORE: " + std::to_string(score), Vector2(33.5, 45));
			Debug::Print("MENU", Vector2(44, 51.5f));
			GameObject* object = AddFloorToWorld(Vector3(-15, -2.5f, -452.5f), Vector3(1, 10, 80), 0, 90, 0, "START", Vector4(0.5625f, 0.92960f, 0.5625f, 1));
			object->SetWorldID(99);
		}
		else {
			Debug::Print("SINGLEPLAYER", Vector2(36.5, 51.5f));
			Debug::Print("MULTIPLAYER", Vector2(36.5, 61.5f));
			GameObject* object = AddFloorToWorld(Vector3(-15, -2.5f, -452.5f), Vector3(1, 10, 80), 0, 90, 0, "START", Vector4(0.5625f, 0.92960f, 0.5625f, 1));
			object->SetWorldID(99);
			object = AddFloorToWorld(Vector3(-15, -40, -452.5f), Vector3(1, 10, 80), 0, 90, 0, "MULTIPLAYER", Vector4(0.5625f, 0.92960f, 0.5625f, 1));
			object->SetWorldID(100);

		}
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject	= nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}
}


void TutorialGame::InitCamera() {
	if (gameStatus == 0) {
		world->GetMainCamera()->SetNearPlane(0.1f);
		world->GetMainCamera()->SetFarPlane(1500.0f);
		world->GetMainCamera()->SetPitch(-35.0f);
		world->GetMainCamera()->SetYaw(360.0f);
		world->GetMainCamera()->SetPosition(Vector3(90, 140, 1200));
		lockedObject = nullptr;
	}
	else {
		world->GetMainCamera()->SetNearPlane(0.1f);
		world->GetMainCamera()->SetFarPlane(1500.0f);
	}
}

void TutorialGame::LoadLevel() {

	std::ifstream infile(Assets::DATADIR + "TestGrid1.txt");

	int nodeSize;
	int gridWidth;
	int gridHeight;

	infile >> nodeSize; 
	infile >> gridWidth;
	infile >> gridHeight;

	std::vector<GameObject*> constraints;

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			char type = 0; 
			infile >> type;
			if (type == 's') {
				startingPlatform = AddFloorToWorld(Vector3(x * nodeSize * 2, 10, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, 0, 0, "FLOOR", Vector4(0.46f, 0.64f, 0.95f, 1));
				startingPlatform->GetRenderObject()->SetColour(Vector4(0.313, 0.855, 0.274, 1));
			}
			if (type == 'f') { AddFloorToWorld(Vector3(x * nodeSize * 2, -25, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, 0, 0, "FLOOR", Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 'd') { AddFloorToWorld(Vector3(x * nodeSize * 2, -10, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), -30, 0, 0, "SLOPE", Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 'u') { AddFloorToWorld(Vector3(x * nodeSize * 2, -15, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 30, 0, 0, "SLOPE", Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 'n') { AddFloorToWorld(Vector3(x * nodeSize * 2, -5, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, 0, 0, "FLOOR", Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 'r') { AddRotatingTower(Vector3(x * nodeSize * 2, -19, y * nodeSize * 2), Vector3(nodeSize, nodeSize, nodeSize), Vector3(nodeSize, nodeSize, nodeSize), 20, 0); }
			if (type == 'e') { AddFloorToWorld(Vector3(x * nodeSize * 2, -20, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), -30, 0, 0, "SLOPE", Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 'g') { AddPushTile(Vector3(x * nodeSize * 2, -35, y * nodeSize * 2), Vector3(nodeSize * 4, 1, nodeSize * 4), 0, Vector4(0.96f, 0.76f, 0.22f, 1)); }
			if (type == 'v') { AddFloorToWorld(Vector3(x * nodeSize * 2, -35, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0,0, 0, "FLOOR",Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 'l') { AddRotatingTower(Vector3(x * nodeSize * 2, 10, y * nodeSize * 2), Vector3(40, 10, 2), Vector3(50, 10, 2), 20, 0); }
			if (type == 'q') { AddBarrier(Vector3(x * nodeSize * 2 + 5, -5, y * nodeSize * 2), Vector3(1, 10, 25), 90, 0.1f, Vector4(0, 0, 0, 1)); }
			if (type == 'k') { AddBarrier(Vector3(x * nodeSize * 2 + 5, -5, y * nodeSize * 2), Vector3(1, 10, 25), 90, 0.2f, Vector4(0, 0, 0, 1)); }
			if (type == 'a') { AddBarrier(Vector3(x * nodeSize * 2 + 5, -5, y * nodeSize * 2), Vector3(1, 10, 25), 90, 0.15f, Vector4(0, 0, 0, 1)); }
			if (type == 'w') { AddPendulum(Vector3(x * nodeSize * 2 , 5, y * nodeSize * 2), 10, 1, 0); }
			if (type == 'p') { constraints.push_back(AddFloorToWorld(Vector3(x * nodeSize * 2, -35, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, 0, 0, "FLOOR", Vector4(0.46f, 0.64f, 0.95f, 1))); }
			if (type == 'y') { AddBonusToWorld(Vector3(x * nodeSize * 2, -25, y * nodeSize * 2), Vector3(1, 1, 1)); }
			if (type == 'o') { AddBonusToWorld(Vector3(x * nodeSize * 2, 5, y * nodeSize * 2), Vector3(1, 1, 1)); }
			if (type == 'j') { AddBonusToWorld(Vector3(x * nodeSize * 2, -17, y * nodeSize * 2), Vector3(1, 1, 1)); }
			if (type == 'm') { AddMotorTile(Vector3(x * nodeSize * 2, -35, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, Vector4(0.96f, 0.76f, 0.22f, 1)); }
			if (type == 'z') { AddFloorToWorld(Vector3(x * nodeSize * 2, -35, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, 0, 0, "FLOOR", Vector4(0.46f, 0.64f, 0.95f, 1)); }
			if (type == 't') { AddFloorToWorld(Vector3(x * nodeSize * 2, -25, y * nodeSize * 2), Vector3(1, nodeSize * 2, nodeSize), 0, 90, 0, "FLOOR", Vector4(0.5625f, 0.92960f, 0.5625f, 1)); }
			if (type == 'b') {
				finalPlatform = AddFloorToWorld(Vector3(x * nodeSize * 2, -35, y * nodeSize * 2), Vector3(nodeSize * 5, 1, nodeSize * 5), 0, 0, 0, "FLOOR", Vector4(0.46f, 0.64f, 0.95f, 1));
				finalPlatform->GetRenderObject()->SetColour(Vector4(0.855, 0.313, 0.274, 1));
				finalPlatform->isLast = true;
			}
		}
	}

	if (constraints.size() >= 3) {
		float maxDistance = 50;
		GameObject* start = constraints.at(0);
		GameObject* middle = constraints.at(1);
		GameObject* end = constraints.at(2);

		middle->GetPhysicsObject()->SetInverseMass(100);
		PositionConstraint* constraint = new PositionConstraint(start, middle, maxDistance);
		world->AddConstraint(constraint);
		constraint = new PositionConstraint(middle, end, maxDistance);
		world->AddConstraint(constraint);

		start = end;
		middle = constraints.at(3);
		end = constraints.at(4);

		middle->GetPhysicsObject()->SetInverseMass(100);
		constraint = new PositionConstraint(start, middle, maxDistance);
		world->AddConstraint(constraint);
		constraint = new PositionConstraint(middle, end, maxDistance);
		world->AddConstraint(constraint);
	}
}

void TutorialGame::InitWorld() {
	floorList.clear();
	world->ClearAndErase();
	physics->Clear();
	if (gameStatus == 0) {
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
		updateList.push_back(AddPlayerToWorld(Vector3(90, 50, 1070)));
		LoadLevel();
		if (multiplayer) {
			enemy = AddEnemyToWorld(Vector3(100, 50, 1070));
			PathFind(enemy->GetTransform().GetPosition() / 2, Vector3(50, 0, 0));
		}
	}
	else {
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
		AddMenu();
	}
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));
}

void TutorialGame::AddMenu() {


}



void TutorialGame::PathFind(Vector3 position, Vector3 endpoint) {
	testNodes.clear();
	NavigationGrid grid = NavigationGrid("TestGrid1.txt");
	NavigationPath outPath;
	bool  found = grid.FindPath(position, endpoint, outPath);
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos * 2);
	}
	EnemyController* enemyCast = (EnemyController*)enemy;
	enemyCast->nodes.clear();
	enemyCast->nodes = testNodes;
}





GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, const Vector3& size, float tilt, float direction, float type, string name, Vector4 colour, bool addToList) {
	GameObject* floor = new GameObject("FLOOR");
	Quaternion rotation = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), direction) * Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), tilt);
	Vector3 floorSize = size;
	if (name == "SLOPE") { floorSize.z += 1.5; }
	OBBVolume* volume = new OBBVolume(floorSize, rotation);
	volume->name = name;
	floor->SetBoundingVolume((CollisionVolume*)volume);

	floor->GetTransform()
			.SetScale(size * 2)
			.SetPosition(position)
			.SetOrientation(rotation);

	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, nullptr, basicShader));
	floor->GetRenderObject()->SetColour(colour);
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetPhysicsObject()->name = "FLOOR";
	
	world->AddGameObject(floor);
	return floor;
}

void  TutorialGame::AddPushTile(const Vector3& position, Vector3& size, float direction, Vector4 colour) {
	GameObject* floor = new GameObject("PUSH-TILE");
	Quaternion rotation = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), direction);
	Vector3 floorSize = size;
	OBBVolume* volume = new OBBVolume(floorSize, rotation);
	volume->name = "FLOOR";
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position)
		.SetOrientation(rotation);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, nullptr, basicShader));
	floor->GetRenderObject()->SetColour(colour);
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetPhysicsObject()->name = "PUSHTILE";

	world->AddGameObject(floor);
}

void  TutorialGame::AddMotorTile(const Vector3& position, Vector3& size, float direction, Vector4 colour) {
	GameObject* floor = new GameObject("MOTOR-TILE");
	Quaternion rotation = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), direction);
	Vector3 floorSize = size;
	OBBVolume* volume = new OBBVolume(floorSize, rotation);
	volume->name = "FLOOR";
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position)
		.SetOrientation(rotation);
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, nullptr, basicShader));
	floor->GetRenderObject()->SetColour(colour);
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetPhysicsObject()->name = "MOTORTILE";
	floor->GetRenderObject()->SetColour(Vector4(0.631f, 1, 0.53f, 1));
	world->AddGameObject(floor);
}

void  TutorialGame::AddBarrier(const Vector3& position, Vector3& size, float direction, float speed, Vector4 colour) {
	GameObject* barrier = new Barrier(speed, "BARRIER");
	Quaternion rotation = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), direction);
	OBBVolume* volume = new OBBVolume(size, rotation);
	barrier->SetBoundingVolume((CollisionVolume*)volume);
	barrier->GetTransform()
		.SetScale(size * 2)
		.SetPosition(position)
		.SetOrientation(rotation);

	barrier->SetPhysicsObject(new PhysicsObject(&barrier->GetTransform(), barrier->GetBoundingVolume()));
	barrier->SetRenderObject(new RenderObject(&barrier->GetTransform(), cubeMesh, nullptr, basicShader));
	barrier->GetRenderObject()->SetColour(colour);
	barrier->GetPhysicsObject()->SetInverseMass(0);
	barrier->GetPhysicsObject()->InitCubeInertia();
	barrier->GetPhysicsObject()->name = "FLOOR";
	updateList.push_back(barrier);

	world->AddGameObject(barrier);
}



/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

void TutorialGame::AddPendulum(const Vector3& position, float radius, float speed, float inverseMass) {
	GameObject* sphere = new Pendulum(speed,"Pendulum",position, 80);
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh,nullptr, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	
	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();
	updateList.push_back(sphere);
	world->AddGameObject(sphere);
}


GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}


float rand(int range, int buffer) {
	 return (rand() % range) + buffer;
}

void TutorialGame::AddRotatingTower(const Vector3& position, Vector3 dimensions, Vector3 scale, float rotateSpeed, float inverseMass) {
	updateList.push_back(AddRotatingCube(position, dimensions, scale, rand(40,40), inverseMass));
}

GameObject* TutorialGame::AddRotatingCube(const Vector3& position, Vector3 dimensions, Vector3 scale, float rotateSpeed, float inverseMass) {
	GameObject* rotatingCube = new RotatingCube(rotateSpeed, "ROTATING-OBSTACLE");

	OBBVolume* volume = new OBBVolume(dimensions);

	rotatingCube->SetBoundingVolume((CollisionVolume*)volume);

	rotatingCube->GetTransform()
		.SetPosition(position)
		.SetScale(scale *2);

	rotatingCube->SetRenderObject(new RenderObject(&rotatingCube->GetTransform(), cubeMesh,nullptr, basicShader));
	rotatingCube->SetPhysicsObject(new PhysicsObject(&rotatingCube->GetTransform(), rotatingCube->GetBoundingVolume()));

	rotatingCube->GetPhysicsObject()->SetInverseMass(inverseMass);
	rotatingCube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(rotatingCube);
	obstacleList.push_back(rotatingCube);
	return rotatingCube;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}


GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 10.0f;
	float inverseMass = 0.5;
	float radius = 2;
	float halfHeight = 5;
	GameObject* character = new PlayerObject("PLAYER");
	character->SetWorldID(0);
	CapsuleVolume* volume = new CapsuleVolume(halfHeight,radius);

	volume->name = "DAVE";

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(8.5f, 10, 8.5f))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->GetRenderObject()->SetColour(Vector4(0.76, 0.76, 0.76, 1));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetPhysicsObject()->name = "PLAYER";
	character->GetPhysicsObject()->friction = 0.98;
	character->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 10.0f;
	float inverseMass = 0.5;
	float radius = 2;
	float halfHeight = 5;
	GameObject* character = new EnemyController("ENEMY");
	character->SetWorldID(0);
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);

	volume->name = "DAVE";

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(8.5f, 10, 8.5f))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->GetRenderObject()->SetColour(Vector4(0.76, 0.76, 0.76, 1));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetPhysicsObject()->name = "ENEMY";
	character->GetPhysicsObject()->friction = 0.98;
	character->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(character);
	updateList.push_back(character);
	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position, Vector3& scale ) {
	GameObject* apple = new RotatingCube(40, "BONUS-ITEM");

	SphereVolume* volume = new SphereVolume(5);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(scale)
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1, 0.41f, 0.70f,1));
	apple->GetPhysicsObject()->name = "BONUS";
	apple->needsImpulse = false;
	world->AddGameObject(apple);
	bonusList.push_back(apple);
	updateList.push_back(apple);

	return apple;
}



/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
Vector4 prevColour;
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::RIGHT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(prevColour);
				selectionObject = nullptr;
				lockedObject = nullptr;
			}
		}
		
		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT) && selectionObject == nullptr) {
			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				prevColour = selectionObject->GetRenderObject()->GetColour();
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}


		if (selectionObject && gameStatus == 0) {
			string position = "(" + std::to_string((int)selectionObject->GetTransform().GetPosition().x) + "," + std::to_string((int)selectionObject->GetTransform().GetPosition().y) + "," + std::to_string((int)selectionObject->GetTransform().GetPosition().z) + ")";
			Debug::Print("Object Name: " + selectionObject->GetName(), Vector2(5, 15));
			Debug::Print("Current Position: " + position, Vector2(5, 20));
			float inverseMass = selectionObject->GetPhysicsObject()->GetInverseMass();

			Debug::Print("Inverse Mass: " + std::to_string(round(inverseMass)), Vector2(5,25));
			string volName; 
			switch (selectionObject->GetBoundingVolume()->type) {
				case(NCL::VolumeType::AABB): volName = "AABB"; break;
				case(NCL::VolumeType::OBB):  volName = "OBB"; break;
				case(NCL::VolumeType::Sphere): volName = "SPHERE"; break;
				case(NCL::VolumeType::Capsule):volName = "CAPSULE"; break;
			}
			Debug::Print("Collision Volume: " + volName, Vector2(5,30));
		}

		if (selectionObject) {
			if ((selectionObject->GetWorldID() == 99 || selectionObject->GetWorldID() == 100) && gameStatus == 2) {
				gameStatus = 3;
				if (selectionObject->GetWorldID() == 100) {
					multiplayer = true;
				}
			}
			if (selectionObject->GetWorldID() == 99 && gameStatus == 4) {
				gameStatus = 6;
			}
		}
	}
	else {
		if (selectionObject) {
			selectionObject->GetRenderObject()->SetColour(prevColour);
			selectionObject = nullptr;
			lockedObject = nullptr;
		}
	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;
	}

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}