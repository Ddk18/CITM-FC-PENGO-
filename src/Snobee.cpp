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
SNOBEE::~SNOBEE()
{
}
AppStatus SNOBEE::Initialise(Look look, const AABB& area)
{
	int i;
	const int n = SNOBEE_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_ENEMIES));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for slime sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)SNOBEEAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)SNOBEEAnim::IDLE_RIGHT, SNOBEE_ANIM_DELAY);
	sprite->AddKeyFrame((int)SNOBEEAnim::IDLE_RIGHT, { 0, 2*n, n, n });
	sprite->SetAnimationDelay((int)SNOBEEAnim::IDLE_LEFT, SNOBEE_ANIM_DELAY);
	sprite->AddKeyFrame((int)SNOBEEAnim::IDLE_LEFT, { 0, 2*n, -n, n });

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
bool SNOBEE::Update(const AABB& box)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int anim_id;

	if (state == SNOBEEState::ROAMING)
	{
		if (IsVisible(box))
		{
			state = SNOBEEState::ATTACK;
			//The attack animation consists of 2 frames, with the second one being when
			//we throw the shot. Wait for a frame before initiating the attack.
			attack_delay = SNOBEE_ANIM_DELAY;

			if (look == Look::LEFT)	sprite->SetAnimation((int)SNOBEEAnim::ATTACK_LEFT);
			else					sprite->SetAnimation((int)SNOBEEAnim::ATTACK_RIGHT);
		}
		else
		{
			pos += pattern[current_step].speed;
			current_frames++;

			if (current_frames == pattern[current_step].frames)
			{
				current_step++;
				current_step %= pattern.size();
				current_frames = 0;
				
				anim_id = pattern[current_step].anim;
				sprite->SetAnimation(anim_id);
				UpdateLook(anim_id);
			}
		}
	}
	else if (state == SNOBEEState::ATTACK)
	{
		if(!IsVisible(box))
		{
			state = SNOBEEState::ROAMING;

			//Continue with the previous animation pattern before initiating the attack
			anim_id = pattern[current_step].anim;
			sprite->SetAnimation(anim_id);
		}
		else
		{
			attack_delay--;
			if (attack_delay == 0)
			{
				

				//The attack animation consists of 2 frames. Wait for a complete loop
				//before shooting again
				attack_delay = 2* SNOBEE_ANIM_DELAY;
			}
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
