#include "Snobee.h"
#include "Sprite.h"

SNOBEE::SNOBEE(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	attack_delay = 0;
	state = SNOBEEState::ROAMING;

	current_step = 0;
	current_frames = 0;
}
AppStatus SNOBEE::Initialise(const Point& pos, EnemyType type, const AABB& area, TileMap* map)
{
	this->map = map;
	this->visibility_area = area;
	this->look = Look::RIGHT; // o LEFT, o según `type` si quieres variarlo

	state = SNOBEEState::ROAMING;
	this->map = map;

	int i;
	const int n = SNOBEE_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_ENEMIES));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for sno-bee sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)SNOBEEAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)SNOBEEAnim::IDLE_RIGHT, SNOBEE_ANIM_DELAY);
	sprite->AddKeyFrame((int)SNOBEEAnim::IDLE_RIGHT, { 1, 7*n, n, n });
	sprite->SetAnimationDelay((int)SNOBEEAnim::IDLE_LEFT, SNOBEE_ANIM_DELAY);
	sprite->AddKeyFrame((int)SNOBEEAnim::IDLE_LEFT, { 3*n, n, n, n});

	sprite->SetAnimationDelay((int)SNOBEEAnim::WALKING_RIGHT, SNOBEE_ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)SNOBEEAnim::WALKING_RIGHT, { (float)i*n, 2*n, n, n });
	sprite->SetAnimationDelay((int)SNOBEEAnim::WALKING_LEFT, SNOBEE_ANIM_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)SNOBEEAnim::WALKING_LEFT, { (float)i*n, 2*n, -n, n });

	sprite->SetAnimationDelay((int)SNOBEEAnim::ATTACK_RIGHT, SNOBEE_ANIM_DELAY);
	sprite->AddKeyFrame((int)SNOBEEAnim::ATTACK_RIGHT, { 0, 3*n, n, n });
	sprite->AddKeyFrame((int)SNOBEEAnim::ATTACK_RIGHT, { n, 3*n, n, n });
	sprite->SetAnimationDelay((int)SNOBEEAnim::ATTACK_LEFT, SNOBEE_ANIM_DELAY);
	sprite->AddKeyFrame((int)SNOBEEAnim::ATTACK_LEFT, { 0, 3*n, -n, n });
	sprite->AddKeyFrame((int)SNOBEEAnim::ATTACK_LEFT, { n, 3*n, -n, n });

	this->look = look;
	if(look == Look::LEFT)        sprite->SetAnimation((int)SNOBEEAnim::IDLE_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)SNOBEEAnim::IDLE_RIGHT);
	
	visibility_area = area;

	InitPattern();

	return AppStatus::OK;
}
void SNOBEE::InitPattern()
{
	//Multiplying by 3 ensures sufficient time for displaying all 3 frames of the
	//walking animation, resulting in smoother transitions and preventing the animation
	//from appearing rushed or incomplete
	const int n = SNOBEE_ANIM_DELAY*3;

	pattern.push_back({ {0, 0}, 2*n, (int)SNOBEEAnim::IDLE_RIGHT });
	pattern.push_back({ {SNOBEE_SPEED, 0}, n, (int)SNOBEEAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)SNOBEEAnim::IDLE_RIGHT });
	pattern.push_back({ {SNOBEE_SPEED, 0}, n, (int)SNOBEEAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)SNOBEEAnim::IDLE_RIGHT });
	
	pattern.push_back({ {0, 0}, 2*n, (int)SNOBEEAnim::IDLE_LEFT });
	pattern.push_back({ {-SNOBEE_SPEED, 0}, n, (int)SNOBEEAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)SNOBEEAnim::IDLE_LEFT });
	pattern.push_back({ {-SNOBEE_SPEED, 0}, n, (int)SNOBEEAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)SNOBEEAnim::IDLE_LEFT });
	
	current_step = 0;
	current_frames = 0;
}




void SNOBEE::UpdateMovementAI(const AABB& playerBox)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	const int tileSize = 16;
	int baseSpeed = std::max(1, SNOBEE_SPEED / 2); // Protección contra división por cero

	if (stepsRemaining == 0)
	{
		// Determinar dirección de movimiento
		if (IsVisible(playerBox))
		{
			// Movimiento hacia el jugador
			Point playerPos = playerBox.pos;

			if (abs(playerPos.x - pos.x) > abs(playerPos.y - pos.y)) {
				if (playerPos.x < pos.x) {
					movement = { -baseSpeed, 0 };
					look = Look::LEFT;
					sprite->SetAnimation((int)SNOBEEAnim::WALKING_LEFT);
				}
				else {
					movement = { baseSpeed, 0 };
					look = Look::RIGHT;
					sprite->SetAnimation((int)SNOBEEAnim::WALKING_RIGHT);
				}
			}
			else {
				if (playerPos.y < pos.y) {
					movement = { 0, -baseSpeed };
				}
				else {
					movement = { 0, baseSpeed };
				}
				sprite->SetAnimation((look == Look::LEFT) ? (int)SNOBEEAnim::WALKING_LEFT
					: (int)SNOBEEAnim::WALKING_RIGHT);
			}
		}
		else {
			// Movimiento aleatorio
			int dir = GetRandomValue(0, 3);
			switch (dir) {
			case 0: movement = { -baseSpeed, 0 }; look = Look::LEFT;  sprite->SetAnimation((int)SNOBEEAnim::WALKING_LEFT); break;
			case 1: movement = { baseSpeed, 0 };  look = Look::RIGHT; sprite->SetAnimation((int)SNOBEEAnim::WALKING_RIGHT); break;
			case 2: movement = { 0, -baseSpeed }; break;
			case 3: movement = { 0, baseSpeed };  break;
			}
		}

		// Comprobar colisiones ANTES de moverse
		AABB box = GetHitbox();

		bool blocked = false;
		if (movement.x < 0) blocked = map->TestCollisionWallLeft(box);
		else if (movement.x > 0) blocked = map->TestCollisionWallRight(box);
		else if (movement.y < 0) blocked = map->TestCollisionWallUp(box);
		else if (movement.y > 0) blocked = map->TestCollisionWallDown(box);

		if (blocked)
		{
			movement = { 0, 0 };
			stepsRemaining = 0;

			// Animación idle si no puede moverse
			if (look == Look::LEFT)  sprite->SetAnimation((int)SNOBEEAnim::IDLE_LEFT);
			else if (look == Look::RIGHT) sprite->SetAnimation((int)SNOBEEAnim::IDLE_RIGHT);

			return;
		}

		stepsRemaining = tileSize / baseSpeed;
	}

	// Movimiento activo
	pos += movement;
	stepsRemaining--;

	// Al terminar, volver a idle
	if (stepsRemaining == 0)
	{
		if (look == Look::LEFT)
			sprite->SetAnimation((int)SNOBEEAnim::IDLE_LEFT);
		else if (look == Look::RIGHT)
			sprite->SetAnimation((int)SNOBEEAnim::IDLE_RIGHT);
	}

	sprite->Update();
}



void SNOBEE::MoveOneTileInDirection(Look dir)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);

	const int tileSize = 16;
	int baseSpeed = std::max(1, SNOBEE_SPEED / 2);  // Protección contra 0

	if (stepsRemaining == 0) {
		switch (dir) {
		case Look::LEFT:
			movement = { -baseSpeed, 0 };
			sprite->SetAnimation((int)SNOBEEAnim::WALKING_LEFT);
			break;
		case Look::RIGHT:
			movement = { baseSpeed, 0 };
			sprite->SetAnimation((int)SNOBEEAnim::WALKING_RIGHT);
			break;
		case Look::UP:
			movement = { 0, -baseSpeed };
			break;
		case Look::DOWN:
			movement = { 0, baseSpeed };
			break;
		}

		stepsRemaining = tileSize / baseSpeed;
		look = dir;
	}

	pos += movement;
	stepsRemaining--;

	if (stepsRemaining == 0) {
		if (look == Look::LEFT)
			sprite->SetAnimation((int)SNOBEEAnim::IDLE_LEFT);
		else if (look == Look::RIGHT)
			sprite->SetAnimation((int)SNOBEEAnim::IDLE_RIGHT);
	}

	sprite->Update();
}











bool SNOBEE::Update(const AABB& box)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int anim_id;

	// NUEVO: movimiento por IA (aleatorio o hacia el jugador)
	UpdateMovementAI(box);

	// Mantenimiento de estado ATTACK (mantiene animación y delay)
	if (state == SNOBEEState::ATTACK)
	{
		if (!IsVisible(box))
		{
			state = SNOBEEState::ROAMING;
			anim_id = (look == Look::LEFT) ? (int)SNOBEEAnim::IDLE_LEFT : (int)SNOBEEAnim::IDLE_RIGHT;
			sprite->SetAnimation(anim_id);
		}
		else
		{
			attack_delay--;
			if (attack_delay == 0)
			{
				// Acción de ataque (a implementar si quieres disparo u otro efecto)
				attack_delay = 2 * SNOBEE_ANIM_DELAY;
				shoot = true;
			}
		}
	}
	else if (state == SNOBEEState::ROAMING)
	{
		if (IsVisible(box))
		{
			state = SNOBEEState::ATTACK;
			attack_delay = SNOBEE_ANIM_DELAY;
			sprite->SetAnimation((look == Look::LEFT) ? (int)SNOBEEAnim::ATTACK_LEFT : (int)SNOBEEAnim::ATTACK_RIGHT);
		}
	}

	sprite->Update();
	return shoot;
}

void SNOBEE::UpdateLook(int anim_id)
{
	SNOBEEAnim anim = (SNOBEEAnim)anim_id;
	look = (anim == SNOBEEAnim::IDLE_LEFT ||
			anim == SNOBEEAnim::WALKING_LEFT ||
			anim == SNOBEEAnim::ATTACK_LEFT) ? Look::LEFT : Look::RIGHT;
}
