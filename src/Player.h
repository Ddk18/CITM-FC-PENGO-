#pragma once
#include "Entity.h"
#include "TileMap.h"
#include <unordered_map>



enum ScoreAction {
	KILL_1_SNOWBEE,       // 400 punts
	KILL_2_SNOWBEES,      // 1600 punts
	KILL_3_SNOWBEES,      // 3200 punts
	DESTROY_EGG,          // 500 punts
	ALIGN_DIAMONDS_SAFE,  // 10.000 punts
	ALIGN_DIAMONDS_DANGER,// 30.000 punts
	FAST_CLEAR_BONUS      // 5.000 punts
	// No incloem SACSEJAR_PARET ni MORIR ja que no donen punts
};

//tamaño del lado del sprite
#define PLAYER_FRAME_SIZE		16

//tamaño real del personaje ingame
#define PLAYER_PHYSICAL_WIDTH	15
#define PLAYER_PHYSICAL_HEIGHT	15

//Horizontal and vertical speed 
#define PLAYER_SPEED			2


//Logic states
enum class State { IDLE, WALKING, PUSHING, DEAD, WIN };

//Rendering states
enum class PlayerAnim {
	IDLE_LEFT, IDLE_RIGHT, IDLE_UP, IDLE_DOWN,
	WALKING_LEFT, WALKING_RIGHT,WALKING_DOWN, WALKING_UP,
	PUSHING_LEFT, PUSHING_RIGHT, PUSHING_DOWN, PUSHING_UP,
	NUM_ANIMATIONS
};

class Player: public Entity
{
public:
	Player(const Point& p, State s, Look view);
	~Player();
	
	AppStatus Initialise();
	void SetTileMap(TileMap* tilemap);

	void Update();
	void DrawDebug(const Color& col) const;
	void Release();

	void FreezeAnimationFrame();
	void RestoreAnimationFrame();
	void Stop();
	void ResumeMovement();
	TileMap* map;

	State GetState() const { return state; }
	void SetState(State s) { state = s; }

	void InitScore();
	void IncrScore(int n);
	void AddScoreForAction(ScoreAction action);
	void AddKillScore(int numSnoBeesKilled);
	int GetScore();

	void UpdatePush(float dt);
	void ChangeAnimByLook();
	Animation* currentAnim = nullptr;
	Animation animPushRight, animPushLeft, animPushUp, animPushDown;

	Tile pushedTileType = Tile::AIR;
	Point pushedTileCoord;
	Vector2 movingBlockPos = { 0.0f, 0.0f };
	bool hasMovingBlock = false;
	void RenderMovingBlock() const;


private:
	
	int score;
	bool IsLookingRight() const;
	bool IsLookingLeft() const;
	bool IsLookingDown() const;
	bool IsLookingUp() const;

	//Player mechanics
	void Move();


	//Animation management
	std::unordered_map<int, int> originalAnimationDelays;
	void SetAnimation(int id);
	PlayerAnim GetAnimation();
	void StartWalkingLeft();
	void StartWalkingRight();
	void StartWalkingUp();
	void StartWalkingDown();
	void StartClimbingUp();
	void StartClimbingDown();
	void ChangeAnimRight();
	void ChangeAnimLeft();
	void ChangeAnimUp();
	void ChangeAnimDown();



	State state;
	Look look;

	//Reference to the TileMap object
	//This class does not own the object, it only holds a reference to it
	

	bool isPushingBlock = false;
	Vector2 pushingStart = { 0.0f, 0.0f };
	Vector2 pushingEnd = { 0.0f, 0.0f };


	float pushSpeed = 200.0f; // pixeles por segundo
	float pushProgress = 0.0f;



};

