#ifndef MENU_H_
#define MENU_H_

//#include <HAPI_lib.h>
#include "state.h"

#include <vector>
#include <string>

//
// Enumerators
//

enum MenuChoice
{
	menuPlay=0,
	menuInstructions,
	menuScore,
	menuExit,
	menuChoices
};

//
// Class MenuState
//

class MenuState: public State{
	public:

		MenuState();

		~MenuState();

		//init - initializes the menu
		void init();

		//update - updates the menu
		void update();

		//draws the menu
		void draw(float blend);

		//load_scores - loads high scores from file
		bool load_scores();

		//get_next_state - returns the next state
		int get_next_state() const;

	private:

		bool play_;
		bool score_loaded_;

		int selected_choice_;
		int background_id_;
		int selected_id_;
		int instructions_id_;
		int instructions1_id_;
		int instructions2_id_;
		int score_id_;

		std::vector<std::string> score_list_;

};

#endif//MENU_H_