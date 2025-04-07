
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
	jump_delay = PLAYER_JUMP_DELAY;
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
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_RIGHT, { 8*n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_LEFT, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_LEFT, { 4*n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_UP, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_UP, { 6*n, 0, n, n });
	sprite->SetAnimationDelay((int)PlayerAnim::IDLE_DOWN, ANIM_DELAY);
	sprite->AddKeyFrame((int)PlayerAnim::IDLE_DOWN, { n, 0, n, n });

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
bool Player::IsInFirstHalfTile() const
{
	return pos.y % TILE_SIZE < TILE_SIZE / 2;
}
bool Player::IsInSecondHalfTile() const
{
	return pos.y % TILE_SIZE >= TILE_SIZE/2;
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
void Player::Stop()
{
	dir = { 0,0 };
	state = State::IDLE;
	if (IsLookingRight())	SetAnimation((int)PlayerAnim::IDLE_RIGHT);
	else if(IsLookingLeft())		SetAnimation((int)PlayerAnim::IDLE_LEFT);
	else if(IsLookingUp())		SetAnimation((int)PlayerAnim::IDLE_UP);
	else if	(IsLookingDown())		SetAnimation((int)PlayerAnim::IDLE_DOWN);
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


void Player::Update()
{
	//Player doesn't use the "Entity::Update() { pos += dir; }" default behaviour.
	//Instead, uses an independent behaviour for each axis.
	MoveX();
	MoveY();

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
}

void Player::MoveX()
{
	AABB box;
	int prev_x = pos.x;

	

	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
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
			if (state == State::WALKING) Stop();
		}
	}
	else if (IsKeyDown(KEY_RIGHT))
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
			if (state == State::WALKING) Stop();
		}
	}
	else
	{
		if (state == State::WALKING) Stop();
	}
}
void Player::MoveY()
{
	AABB box;

	box = GetHitbox();
	int prev_y = pos.y;

	if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
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
		if (map->TestCollisionWallLeft(box))
		{
			pos.y = prev_y;
			if (state == State::WALKING) Stop();
		}
	}
	else if (IsKeyDown(KEY_DOWN))
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
		if (map->TestCollisionWallRight(box))
		{
			pos.y = prev_y;
			if (state == State::WALKING) Stop();
		}
	}
	else
	{
		if (state == State::WALKING) Stop();
	}
	if (map->TestCollisionGround(box, &pos.y))
	{
		if (state == State::PUSHING) Stop();	

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