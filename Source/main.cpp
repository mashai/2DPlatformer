#include "utils.h"
#include "graphics.h"
#include "input.h"
#include "state.h"
#include "world.h"
#include "menu.h"
#include "gameover.h"

#define TPS 20  //ticks per seconds

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 512;

void HAPI_Main() {

	if(!HAPI->Initialise(&SCREEN_WIDTH, &SCREEN_HEIGHT))
		return;

	//show frames per seconds
	HAPI->SetShowFPS(true, 10, 10);

	//initialize components
	GRAPHICS->init(SCREEN_WIDTH, SCREEN_HEIGHT);
	INPUT->init();

	WorldState* world_state = new WorldState();
	MenuState* menu_state = new MenuState();
	GameOverState* game_over_state = new GameOverState(world_state);

	//add all states to state_list
	std::vector<State*> state_list;
	state_list.resize(states);
	state_list[play] = world_state;
	state_list[gameover] = game_over_state;
	state_list[menu] = menu_state;

	//initialize the first state to be the menu and time to 0
	int current_state = menu;
	DWORD last_update_time = 0;

	//initialize all states
	for(uint i  = 0; i < state_list.size(); i++)
	{
		state_list[i]->init();
	}

	while(HAPI->Update())
	{
		INPUT->update();

		//if the time since the last update is bigger than a tick
		if(HAPI->GetTime() - last_update_time >= 1000/TPS)
		{
			last_update_time = HAPI->GetTime();
			state_list[current_state]->update();
		}

		//calculate the amount to interpolate
		float blend = (HAPI->GetTime() - last_update_time) / (1000.f/TPS);

		state_list[current_state]->draw(blend);

		int next_state = state_list[current_state]->get_next_state();

		if(next_state != current_state)
		{
			state_list[next_state]->init();
			current_state = next_state;
		}
	}

	//delete all states
	for(uint i  = 0; i < state_list.size(); i++)
	{
		delete state_list[i];
	}

	GRAPHICS->destroy();

	INPUT->destroy();
}


