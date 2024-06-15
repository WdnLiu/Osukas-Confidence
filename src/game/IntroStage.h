#pragma once
#include "stage.h"
#include "framework/entities/entityUI.h"

class IntroStage : Stage
{
public:
    IntroStage();

    std::vector<EntityUI*> buttons;
    std::vector<EntityUI*> optionbuttons;

    std::vector<std::string> keynames;


    EntityUI bigblacksquare;

    void renderText(Texture* font, std::string text, float interval, Vector2 position, float fontsize, Vector2 tilesize, Vector2 textbox);
    void renderLetter(Texture* font, Vector2 tileSize, char letter, Vector2 position, Vector2 size);

    void onKeyDown(SDL_KeyboardEvent event);
    void onMouseButtonUp(SDL_MouseButtonEvent event) override;

    void render(void) override;
    void update(double seconds_elapsed) override;

    void switchstage(int flag) override;
};