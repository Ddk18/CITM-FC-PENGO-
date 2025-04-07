#pragma once
#include "Enemy.h"

#define SNOBEE_SPEED			1
#define SNOBEE_ANIM_DELAY	(4*ANIM_DELAY)


enum class SNOBEEState { ROAMING, ATTACK };
enum class SNOBEEAnim {  IDLE_LEFT, IDLE_RIGHT, WALKING_LEFT, WALKING_RIGHT,
						ATTACK_LEFT, ATTACK_RIGHT, NUM_ANIMATIONS };

struct Step
{
	Point speed;	//direction
	int frames;		//duration in number of frames
	int anim;		//graphical representation
};

class SNOBEE : public Enemy
{
public:
	SNOBEE(const Point& p, int width, int height, int frame_width, int frame_height);
	~SNOBEE();

	//Initialize the enemy with the specified look and area
	AppStatus Initialise(Look look, const AABB& area) override;

	//Update the enemy according to its logic, return true if the enemy must shoot
	bool Update(const AABB& box) override;

	
	

private:
	//Create the pattern behaviour
	void InitPattern();
	
	//Update looking direction according to the current step of the pattern
	void UpdateLook(int anim_id);

	int attack_delay;	//delay between attacks
	SNOBEEState state;

	int current_step;	//current step of the pattern
	int current_frames;	//number of frames in the current step
	std::vector<Step> pattern;
};

