#include "Scene.h"
#include <stdio.h>
#include "Globals.h"
#include <ctime>

static clock_t inicioTiempo = 0;

void ResetTiempoTranscurrido()
{
	inicioTiempo = clock();
}

double tiempoTranscurrido();
Scene::Scene()
{
	player = nullptr;
	level = nullptr;
	enemies = nullptr;
	shots = nullptr;

	camera.target = { 0, 0 };				//Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom

	debug = DebugMode::OFF;
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
	if (level != nullptr)
	{
		level->Release();
		delete level;
		level = nullptr;
	}
	for (Entity* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	if (enemies != nullptr)
	{
		enemies->Release();
		delete enemies;
		enemies = nullptr;
	}
	if (shots != nullptr)
	{
		delete shots;
		shots = nullptr;
	}
}
AppStatus Scene::Init()
{


	//Create player
	player = new Player({ 0,0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	//Initialise player
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	//Create enemy manager
	enemies = new EnemyManager();
	if (enemies == nullptr)
	{
		LOG("Failed to allocate memory for Enemy Manager");
		return AppStatus::ERROR;
	}
	//Initialise enemy manager
	if (enemies->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Enemy Manager");
		return AppStatus::ERROR;
	}

	//Create shot manager 
	shots = new ShotManager();
	if (shots == nullptr)
	{
		LOG("Failed to allocate memory for Shot Manager");
		return AppStatus::ERROR;
	}
	//Initialise shot manager
	if (shots->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Shot Manager");
		return AppStatus::ERROR;
	}

	//Create level 
	level = new TileMap();
	if (level == nullptr)
	{
		LOG("Failed to allocate memory for Level");
		return AppStatus::ERROR;
	}
	//Initialise level
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	//Load level
	if (GenerateProceduralLevel() != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}

	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);
	//Assign the tile map reference to the shot manager to check collisions when shots are shot
	shots->SetTileMap(level);
	//Assign the shot manager reference to the enemy manager so enemies can add shots
	enemies->SetShotManager(shots);

	ResetTiempoTranscurrido();

	return AppStatus::OK;
}

AppStatus Scene::GenerateProceduralLevel()
{
	const int size = LEVEL_WIDTH * LEVEL_HEIGHT;
	int* map = new int[size];
	std::fill_n(map, size, (int)Tile::AIR);
	enemies->SetTileMap(level);


	// Bordes exteriores sólidos
	for (int y = 0; y < LEVEL_HEIGHT; ++y) {
		for (int x = 0; x < LEVEL_WIDTH; ++x) {
			if (x == 0 || x == LEVEL_WIDTH - 1 || y == 0 || y == LEVEL_HEIGHT - 1) {
				map[y * LEVEL_WIDTH + x] = (int)Tile::BLOCK_BLUE;
			}
		}
	}

	// Bloques aleatorios
	for (int i = 0; i < 100; ++i) {
		int x = GetRandomValue(1, LEVEL_WIDTH - 2);
		int y = GetRandomValue(1, LEVEL_HEIGHT - 2);
		map[y * LEVEL_WIDTH + x] = (int)Tile::BLOCK_SQUARE1_TL;
	}

	// Posicionar al jugador
	int playerX = 2, playerY = 2;
	map[playerY * LEVEL_WIDTH + playerX] = (int)Tile::PLAYER;

	if (level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT) != AppStatus::OK) {
		delete[] map;
		return AppStatus::ERROR;
	}

	player->SetPos({ playerX * TILE_SIZE, playerY * TILE_SIZE });
	player->InitScore();
	level->ClearObjectEntityPositions();

	// Enemigos aleatorios
	const int numEnemies = 5;
	for (int i = 0; i < numEnemies; ++i) {
		int x = GetRandomValue(1, LEVEL_WIDTH - 2);
		int y = GetRandomValue(1, LEVEL_HEIGHT - 2);

		int idx = y * LEVEL_WIDTH + x;

		// Solo colocar si hay aire (evita superponer con bloques/jugador)
		if (map[idx] == (int)Tile::AIR) {
			Point pos = { x * TILE_SIZE, y * TILE_SIZE };

			// Área de visión: centrada horizontalmente, 6x2 tiles
			Point areaPos = { pos.x - 3 * TILE_SIZE, pos.y };
			AABB visionArea(areaPos, 6 * TILE_SIZE, 2 * TILE_SIZE);

			enemies->Add(pos, EnemyType::SNOBEE, visionArea, Look::LEFT);
		}
	}

	delete[] map;
	return AppStatus::OK;
}



void Scene::Update()
{
	Point p1, p2;
	AABB hitbox;

	// Modo depuración
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}

	// Congelar escena si el jugador ha muerto
	if (player->GetState() == State::DEAD)
	{
		player->Stop(); // Asegura que el jugador no se mueva
		if (IsKeyPressed(KEY_SPACE))
		{
			Release();
			Init();
		}
		return;
	}


	if (tiempoTranscurrido() > 20) {
		player->SetState(State::WIN);
	}


	if (player->GetState() == State::WIN)
	{
		player->Stop(); // Asegura que el jugador no se mueva
		if (IsKeyPressed(KEY_SPACE))
		{
			Release();
			Init();
		}
		return;
	}



	// Actualización normal
	level->Update();
	player->Update();
	CheckObjectCollisions();

	hitbox = player->GetHitbox();
	enemies->Update(hitbox);
	shots->Update(hitbox);

	// Colisión enemigo → muerte 
	for (Enemy* e : enemies->GetAll())
	{
		if (player->GetHitbox().TestAABB(e->GetHitbox()))
		{
			player->SetState(State::DEAD);
			player->Stop(); // Detiene inmediatamente
			break;
		}
	}


}



void Scene::Render()
{
	BeginMode2D(camera);

	level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects();
		enemies->Draw();
		player->Draw();
		shots->Draw();
		player->RenderMovingBlock();

	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		enemies->DrawDebug();
		player->DrawDebug(GREEN);
		shots->DrawDebug(GRAY);
	}
	

	EndMode2D();

	RenderGUI();
}
void Scene::Release()
{
	level->Release();
	player->Release();
	ClearLevel();
}
void Scene::CheckObjectCollisions()
{
	AABB player_box, obj_box;

	player_box = player->GetHitbox();
	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if (player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());

			//Delete the object
			delete* it;
			//Erase the object from the vector and get the iterator to the next valid element
			it = objects.erase(it);
		}
		else
		{
			//Move to the next object
			++it;
		}
	}
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	enemies->Release();
	shots->Clear();

}
void Scene::RenderObjects() const
{
	for (Object* obj : objects)
	{
		obj->Draw();
	}
}
void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
}
void Scene::RenderGUI() const
{
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);

	if (player->GetState() == State::DEAD)
	{
		const char* msg = "GAME OVER";
		int textWidth = MeasureText(msg, 32);
		DrawText(msg, (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2, 32, RED);
	}

	if (player->GetState() == State::WIN)
	{
		const char* msg = "WIN";
		int textWidth = MeasureText(msg, 32);
		DrawText(msg, (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT / 2, 32, GREEN);
	}
}
double tiempoTranscurrido()
{
	clock_t actual = clock();
	double segundos = double(actual - inicioTiempo) / CLOCKS_PER_SEC;
	return segundos;
}

