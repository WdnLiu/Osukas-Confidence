#include "StageManager.h"
#include "stage.h"
#include "GameStage.h"
#include "IntroStage.h"
#include "loreStageBegin.h"
#include "framework/audio.h"

StageManager* StageManager::instance = NULL;


StageManager::StageManager()
{
	ambient_night = Vector3(0.15, 0.15, 0.25).normalize();
	ambient_day   = Vector3(0.75, 0.65, 0.25).normalize();

	stages["IntroStage"] = (Stage*) new IntroStage();
	stages["GameStage"]  = (Stage*) new GameStage();
	stages["LoreStageBegin"]  = (Stage*) new LoreStageBegin(LoreStageBegin::INTRO);
	stages["GoodEndingStage"] = (Stage*) new LoreStageBegin(LoreStageBegin::GOODENDING);
	stages["BadEndingStage"]  = (Stage*) new LoreStageBegin(LoreStageBegin::BADENDING);

	transitioning = false;

	StageManager::instance = this;
	this->currStage = stages["LoreStageBegin"];
}

void StageManager::render() {
	currStage->render();
}

void StageManager::update(double seconds_elapsed) {
	if (transitioning)
	{
		if (currStage->nextStage == "IntroStage") {

		}
		else if (currStage->nextStage == "LoreStageBegin") {
			Audio::Stop(currStage->backgmusic);
			stages[currStage->nextStage]->switchstage(LoreStageBegin::INTRO);
		}
		else if (currStage->nextStage == "GoodEndingStage") {
			Audio::Stop(currStage->backgmusic);
			stages[currStage->nextStage]->switchstage(LoreStageBegin::GOODENDING);
		}
		else if (currStage->nextStage == "BadEndingStage") {
			Audio::Stop(currStage->backgmusic);
			stages[currStage->nextStage]->switchstage(LoreStageBegin::BADENDING);
		}
		else {

			stages[currStage->nextStage]->switchstage(0);
		}
		currStage = stages[currStage->nextStage];
		transitioning = false;
	}

	currStage->update(seconds_elapsed);
}