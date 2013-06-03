#ifndef GAMEOVER_H_
#define GAMEOVER_H_

#include "state.h"
#include "world.h"
#include <vector>
#include <string>

//
// Class GameOverState
//

class GameOverState: public State{
	public:

		GameOverState(WorldState* world_state);

		~GameOverState();

		//init - initializes the state
		void init();

		//update - updates the state
		void update();

		//draws the state
		void draw(float blend);

		//update_scores - update high scores
		void update_scores(int player_score);

		//load_scores - loads high scores from file
		bool load_scores();

		//write_scores - writes high scores to file
		bool write_scores();

		//get_next_state - returns the next state
		int get_next_state() const;

	private:

		bool menu_;
		bool score_loaded_;
		bool new_high_score_;
		int background_id_;
		int score_id_;

		std::string name_;

		std::vector<std::string> player_list_;
		std::vector<int> score_list_;

		WorldState* world_state_;
};

#endif//GAMEOVER_H_