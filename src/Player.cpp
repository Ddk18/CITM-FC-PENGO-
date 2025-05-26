
#include "Player.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>

Player::Player(const Point& p, State s, Look view) :
	Entity(p, PLAYER_PHYSICAL_WIDTH, PLAYER_PHYSICAL_HEIGHT, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE)
{
	state = s;
	look = view;
	map = nullptr;
	score = 0;
}
Player::~Player()
{
}
AppStatus Player::Initialise()
{
	int i;
	const int n = PLAYER_FRAME_SIZE;





	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_PLAYER, "images/pinguinos/pinguino 1.png") != AppStatus::OK)
	{
		LOG("Failed to load player sprite texture");
		return AppStatus::ERROR;
	}

	render = new Sprite(data.GetTexture(Resource::IMG_PLAYER));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for player sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)PlayerAnim::NUM_ANIMATIONS);
	
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_RIGHT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 7*n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 3*n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_UP, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_UP, { 5*n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_DOWN, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_DOWN, { 0, 0, n, n });

	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_RIGHT, { (float)6 * n + (i * n), 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_LEFT, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_LEFT, { (float)2 * n + (i * n), 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_DOWN, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_DOWN, { (float)0 * n + (i * n), 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::WALKING_UP, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::WALKING_UP, { (float)4 * n + (i * n), 0, n, n });

	
	sprite->SetAnimationDelay((int)PlayerAnim::PUSHING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::PUSHING_RIGHT, { (float)6 * n + (i * n), n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::PUSHING_LEFT, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::PUSHING_LEFT, { (float)2 * n + (i * n), n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::PUSHING_DOWN, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::PUSHING_DOWN, { (float)0 * n + (i * n), n, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::PUSHING_UP, ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)PlayerAnim::PUSHING_UP , { (float)4 * n + (i * n), n, n, n });

		
	sprite->SetAnimation((int)PlayerAnim::IDLE_DOWN);

	return AppStatus::OK;
}

void Player::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}
bool Player::IsLookingRight() const
{
	return look == Look::RIGHT;
}
bool Player::IsLookingLeft() const
{
	return look == Look::LEFT;
}
bool Player::IsLookingDown() const
{
	return look == Look::DOWN;
}
bool Player::IsLookingUp() const
{
	return look == Look::UP;
}
void Player::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
PlayerAnim Player::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (PlayerAnim)sprite->GetAnimation();
}


void Player::FreezeAnimationFrame() {
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	int currentAnim = sprite->GetAnimation();

	// Almacenar el delay original solo si no se ha guardado previamente
	if (originalAnimationDelays.find(currentAnim) == originalAnimationDelays.end()) {
		originalAnimationDelays[currentAnim] = sprite->GetAnimationDelay(currentAnim);
	}

	// Establecer un delay muy alto para "congelar" la animación
	sprite->SetAnimationDelay(currentAnim, 20);
}

// Función para restaurar el delay original de la animación actual
void Player::RestoreAnimationFrame() {
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	int currentAnim = sprite->GetAnimation();

	// Restaurar el delay original si existe
	if (originalAnimationDelays.find(currentAnim) != originalAnimationDelays.end()) {
		sprite->SetAnimationDelay(currentAnim, originalAnimationDelays[currentAnim]);
	}
}

// Función Stop que congela la animación sin cambiar a la animación idle
void Player::Stop()
{
	dir = { 0, 0 }; // Detener movimiento

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	if (sprite != nullptr)
	{
		if (look == Look::LEFT)
			sprite->SetAnimation((int)PlayerAnim::IDLE_LEFT);
		else if (look == Look::RIGHT)
			sprite->SetAnimation((int)PlayerAnim::IDLE_RIGHT);
		else if (look == Look::UP)
			sprite->SetAnimation((int)PlayerAnim::IDLE_UP);
		else if (look == Look::DOWN)
			sprite->SetAnimation((int)PlayerAnim::IDLE_DOWN);

		sprite->FreezeAnimationFrame(); // Detener animación
	}
}



// Ejemplo de función para reanudar el movimiento
void Player::ResumeMovement() {
	RestoreAnimationFrame();
	// Aquí puedes definir la animación de movimiento que corresponda, por ejemplo:
	SetAnimation((int)PlayerAnim::WALKING_RIGHT);
	// Se deben actualizar posición, dirección, etc.
}




void Player::StartWalkingLeft()
{
	state = State::WALKING;
	look = Look::LEFT;
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	if (sprite && sprite->GetAnimation() != (int)PlayerAnim::WALKING_LEFT)
		SetAnimation((int)PlayerAnim::WALKING_LEFT);
	
}
void Player::StartWalkingRight()
{
	state = State::WALKING;
	look = Look::RIGHT;
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	if (sprite && sprite->GetAnimation() != (int)PlayerAnim::WALKING_RIGHT)
		SetAnimation((int)PlayerAnim::WALKING_RIGHT);
}
void Player::StartWalkingDown()
{
	state = State::WALKING;
	look = Look::DOWN;
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	if (sprite && sprite->GetAnimation() != (int)PlayerAnim::WALKING_DOWN)
		SetAnimation((int)PlayerAnim::WALKING_DOWN);
}
void Player::StartWalkingUp()
{
	state = State::WALKING;
	look = Look::UP;
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	if (sprite && sprite->GetAnimation() != (int)PlayerAnim::WALKING_UP)
		SetAnimation((int)PlayerAnim::WALKING_UP);
	
}

//Cambio de animaciones 
void Player::ChangeAnimRight()
{
	look = Look::RIGHT;
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_RIGHT);    break; 
		case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_RIGHT); break;
		case State::PUSHING: SetAnimation((int)PlayerAnim::PUSHING_RIGHT); break;
		
	}
}
void Player::ChangeAnimLeft()
{
	look = Look::LEFT;
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_LEFT);    break;
		case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_LEFT); break;
		case State::PUSHING: SetAnimation((int)PlayerAnim::PUSHING_LEFT); break;
		
	}
}
void Player::ChangeAnimUp()
{
	look = Look::UP;
	switch (state)
	{
	case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_UP);    break;
	case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_UP); break;
	case State::PUSHING: SetAnimation((int)PlayerAnim::PUSHING_UP); break;
	
	}
}
void Player::ChangeAnimDown()
{
	look = Look::DOWN;
	switch (state)
	{
	case State::IDLE:	 SetAnimation((int)PlayerAnim::IDLE_DOWN);    break;
	case State::WALKING: SetAnimation((int)PlayerAnim::WALKING_DOWN); break;
	case State::PUSHING: SetAnimation((int)PlayerAnim::PUSHING_DOWN); break;
	
	}
}

void Player::UpdatePush(float dt) {
	if (pushProgress == 0.0f) {
		map->SetTile(pushedTileCoord.x, pushedTileCoord.y, Tile::AIR);  // ✅ Eliminar del mapa
	}

	pushProgress += pushSpeed * dt;
	float totalDistance = Vector2Length(Vector2Subtract(pushingEnd, pushingStart));

	if (pushProgress >= totalDistance) {
		int endX = (int)(pushingEnd.x / TILE_SIZE);
		int endY = (int)(pushingEnd.y / TILE_SIZE);
		Tile finalTile = map->GetTileIndex(endX, endY);

		if (!map->IsTileSolid(finalTile)) {
			map->SetTile(endX, endY, pushedTileType);
 		}

		hasMovingBlock = false;
		isPushingBlock = false;
		state = State::IDLE;
		return;
	}

	// Interpolamos la posición visual del bloque
	float t = pushProgress / totalDistance;
	movingBlockPos = Vector2Add(pushingStart, Vector2Scale(Vector2Subtract(pushingEnd, pushingStart), t));

}


void Player::ChangeAnimByLook() {
	switch (look) {
	case Look::RIGHT: currentAnim = &animPushRight; break;
	case Look::LEFT: currentAnim = &animPushLeft; break;
	case Look::UP: currentAnim = &animPushUp; break;
	case Look::DOWN: currentAnim = &animPushDown; break;
	}
}

void Player::Update()
{
	float dt = GetFrameTime();
	if (isPushingBlock) {
		UpdatePush(dt);
		return;
	}

	Move();
	
	

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();

}

void Player::Move()
{
	AABB box;
	int prev_x = pos.x;
	int prev_y = pos.y;

	// Handle X movement
	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
	{
		pos.x += -PLAYER_SPEED;
		if (state == State::IDLE) StartWalkingLeft();
		else
		{
			if (IsLookingRight()) ChangeAnimLeft();
			else if (IsLookingDown()) ChangeAnimLeft();
			else if (IsLookingUp()) ChangeAnimLeft();
		}
		box = GetHitbox();
		if (map->TestCollisionWallLeft(box))
		{
			pos.x = prev_x;
			if (state == State::IDLE) Stop();
		}
	}
	else if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
	{
		pos.x += PLAYER_SPEED;
		if (state == State::IDLE) StartWalkingRight();
		else
		{
			if (IsLookingLeft()) ChangeAnimRight();
			else if (IsLookingDown()) ChangeAnimRight();
			else if (IsLookingUp()) ChangeAnimRight();
		}
		box = GetHitbox();
		if (map->TestCollisionWallRight(box))
		{
			pos.x = prev_x;
			if (state == State::IDLE) Stop();
		}
	}

	// Handle Y movement
	if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
	{
		pos.y += -PLAYER_SPEED;
		if (state == State::IDLE) StartWalkingUp();
		else
		{
			if (IsLookingLeft()) ChangeAnimUp();
			else if (IsLookingRight()) ChangeAnimUp();
			else if (IsLookingDown()) ChangeAnimUp();
		}
		box = GetHitbox();
		if (map->TestCollisionWallUp(box))
		{
			pos.y = prev_y;
			if (state == State::PUSHING) Stop();
		}
	}
	else if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_LEFT))
	{
		pos.y += PLAYER_SPEED;
		if (state == State::IDLE) StartWalkingDown();
		else
		{
			if (IsLookingLeft()) ChangeAnimDown();
			else if (IsLookingRight()) ChangeAnimDown();
			else if (IsLookingUp()) ChangeAnimDown();
		}
		box = GetHitbox();
		if (map->TestCollisionWallDown(box))
		{
			pos.y = prev_y;
			if (state == State::IDLE) Stop();
		}
	}

	// Check for ground collision
	box = GetHitbox();
	if (map->TestCollisionGround(box, &pos.y))
	{
		if (state == State::PUSHING) Stop();
	}

	// Handle the case when no movement keys are pressed
	if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
	{
		// Llamar a Stop solo si no estamos ya en estado IDLE o si la animación actual no es la correcta
		Sprite* sprite = dynamic_cast<Sprite*>(render);
		bool isIdleAnimation = false;

		// Verificar si ya estamos en una animación IDLE correcta
		if (look == Look::LEFT && sprite->GetAnimation() == (int)PlayerAnim::IDLE_LEFT)
			isIdleAnimation = true;
		else if (look == Look::RIGHT && sprite->GetAnimation() == (int)PlayerAnim::IDLE_RIGHT)
			isIdleAnimation = true;
		else if (look == Look::UP && sprite->GetAnimation() == (int)PlayerAnim::IDLE_UP)
			isIdleAnimation = true;
		else if (look == Look::DOWN && sprite->GetAnimation() == (int)PlayerAnim::IDLE_DOWN)
			isIdleAnimation = true;

		if (state != State::IDLE || !isIdleAnimation)
			Stop();
	}

	// Empujar bloque si está en estado IDLE o WALKING y se presiona ESPACIO
	if ((state == State::IDLE || state == State::WALKING) && IsKeyPressed(KEY_SPACE)) {
		Point blockPos = pos;

		if (look == Look::RIGHT) blockPos.x += width;
		if (look == Look::LEFT)  blockPos.x -= TILE_SIZE;
		if (look == Look::UP)    blockPos.y -= TILE_SIZE;
		if (look == Look::DOWN)  blockPos.y += height;

		int tileX = blockPos.x / TILE_SIZE;
		int tileY = blockPos.y / TILE_SIZE;

		Tile tile = map->GetTileIndex(tileX, tileY);
		if (map->IsTileSolid(tile)) {
			// Dirección del push
			int dirX = 0, dirY = 0;
			if (look == Look::RIGHT) dirX = 1;
			else if (look == Look::LEFT) dirX = -1;
			else if (look == Look::UP) dirY = -1;
			else if (look == Look::DOWN) dirY = 1;

			int destX = tileX;
			int destY = tileY;
			while (true) {
				int nextX = destX + dirX;
				int nextY = destY + dirY;
				Tile nextTile = map->GetTileIndex(nextX, nextY);
				if (map->IsTileSolid(nextTile) || nextTile != Tile::AIR) break;
				destX = nextX;
				destY = nextY;
			}

			// NO eliminar todavía, se eliminará cuando empiece UpdatePush
			pushedTileType = tile; // Guarda el tipo original del bloque para renderizado
			pushedTileCoord = { tileX, tileY };

			state = State::PUSHING;
			ChangeAnimByLook();  // Nuevo método que agregamos abajo
			isPushingBlock = true;
			pushingStart = { (float)(tileX * TILE_SIZE), (float)(tileY * TILE_SIZE) };
			pushingEnd = { (float)(destX * TILE_SIZE), (float)(destY * TILE_SIZE) };
			pushProgress = 0.0f;
			movingBlockPos = pushingStart;
			hasMovingBlock = true;
			pushedTileCoord = { tileX, tileY };
			pushedTileType = tile; 

		}
	}


}



void Player::DrawDebug(const Color& col) const
{	
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
	
	DrawText(TextFormat("Position: (%d,%d)\nSize: %dx%d\nFrame: %dx%d", pos.x, pos.y, width, height, frame_width, frame_height), 18*16, 0, 8, LIGHTGRAY);
	DrawPixel(pos.x, pos.y, WHITE);
}
void Player::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_PLAYER);

	render->Release();
}

void Player::InitScore()
{
	score = 0;
}

void Player::IncrScore(int n)
{
	score += n;
}

int Player::GetScore()
{
	return score;
}

void Player::AddScoreForAction(ScoreAction action)
{
	switch (action)
	{
	case KILL_1_SNOWBEE:
		IncrScore(400);
		break;
	case KILL_2_SNOWBEES:
		score += 1600;
		break;
	case KILL_3_SNOWBEES:
		score += 3200;
		break;
	case DESTROY_EGG:
		score += 500;
		break;
	case ALIGN_DIAMONDS_SAFE:
		score += 10000;
		break;
	case ALIGN_DIAMONDS_DANGER:
		score += 30000;
		break;
	case FAST_CLEAR_BONUS:
		score += 5000;
		break;
	default:
		break;
	}
}

void Player::AddKillScore(int numSnoBeesKilled)
{
	if (numSnoBeesKilled <= 0) return;

	int base = 400;
	int scoreToAdd = 0;

	switch (numSnoBeesKilled)
	{
	case 1:
		scoreToAdd = base;
		break;
	case 2:
		scoreToAdd = base * 2 * 2; // 1600
		break;
	case 3:
		scoreToAdd = base * 4 * 2; // 3200
		break;
	default:
		// Si per error arriba un valor més gran, seguim amb un càlcul progressiu
		scoreToAdd = base * numSnoBeesKilled * numSnoBeesKilled;
		break;
	}

	score += scoreToAdd;
}

void Player::RenderMovingBlock() const {
	if (hasMovingBlock) {
		Rectangle srcRect = map->GetTileRect(pushedTileType);
		DrawTextureRec(map->GetTileset(), srcRect, movingBlockPos, WHITE);
	}
}
