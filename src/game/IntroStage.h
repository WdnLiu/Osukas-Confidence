#pragma once
#include "stage.h"
#include "framework/entities/entityUI.h"

class IntroStage : Stage
{
public:
    IntroStage();

    std::vector<EntityUI*> buttons;

    void onKeyDown(SDL_KeyboardEvent event);

    void render(void) override;
    void update(double seconds_elapsed) override;

    void switchstage(int flag) override;
};