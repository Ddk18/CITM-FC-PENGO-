#pragma once

#include "RenderComponent.h"
#include <vector>
#include <unordered_map>
enum class AnimMode { AUTOMATIC, MANUAL };

struct Animation
{
    int delay;
    std::vector<Rectangle> frames;
};

class Sprite : public RenderComponent
{
public:
    Sprite(const Texture2D *texture);
    ~Sprite();

    int GetAnimationDelay(int animID) const;

    void SetNumberAnimations(int num);
    void SetAnimationDelay(int id, int delay);
    void AddKeyFrame(int id, const Rectangle& rect);
    void SetAnimation(int id);
    int GetAnimation();

    void SetManualMode();
    void SetAutomaticMode();
    
    void Update();
    void NextFrame();
    void PrevFrame();

    void Draw(int x, int y) const override;
    void DrawTint(int x, int y, const Color& col) const override;
    void FreezeAnimationFrame();

    void Release() override;

private:
    int current_anim;
    int current_frame;
    int current_delay;

    std::unordered_map<int, int> animationDelays;

    const Texture2D *img;
    std::vector<Animation> animations;

    AnimMode mode;
};

