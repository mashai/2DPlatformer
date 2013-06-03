#ifndef WORLD_H_
#define WORLD_H_

#include "state.h"
#include "entities.h"

#include <vector>
#include <string>

//
// Class WorldState
//

class WorldState: public State {
	public:

		WorldState();

		~WorldState();

		//init - initializes the world state
		void init();

		//update - updates the world state
		void update();

		//draw - draws all the world components
		void draw(float blend);

		//load_level - loads level into data_vector_
		bool load_level();

		//load_screen - spawns on-screen entities
		void load_screen();

		//spawn_entity - creates an entity from data given
		bool spawn_entity(entityData data)const;

		//get_state - returns the next state
		int get_next_state() const;

		//get_score - returns the score of the player
		int get_score() const;

		//has_won - returns true if the player has won
		bool has_won() const;

	private:

		//Data members

		bool menu_;

		bool gameover_;

		std::string background_image_;

		PlayerEntity* player_;

		BackgroundEntity* background_;

		std::vector <Entity*> entity_vector_;

		std::vector <entityData> data_vector_;

};

#endif//WORLD_H_
