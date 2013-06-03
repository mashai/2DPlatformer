#ifndef STATE_H_
#define STATE_H_

#include "utils.h"

//
// Abstract Class State
//

class State {
	public:

		virtual ~State(){};

		//init - initialises state
		virtual void init() = 0;

		//update - updates state
		virtual void update() = 0;

		//draw - draws all the state components
		 virtual void draw(float blend) = 0;

		//get_next_state - returns the next state
		virtual int get_next_state() const = 0;
};

#endif//STATE_H_