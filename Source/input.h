#ifndef INPUT_H_
#define INPUT_H_

#include "utils.h"

#define INPUT UserInput::instance()

//
// Singleton Class UserInput
//

class UserInput {
	public:

		static UserInput* instance();

		//init - initializes the values needed for userInput class
		void init();

		//destroy - deletes the memory used in userInput class
		void destroy();

		//update - updates the values
		void update();

		//is_state - checks if argument is true
		bool is_state(int state);

		//any_key_pressed - checks if any key was pressed
		bool any_key_pressed();

		//get_char - returns character pressed
		int get_char();

		//get_last_check - returns previous state
		int get_last_state();

	private:
		
		//constructors

		UserInput();

		UserInput(UserInput& user_input);


		//Data members

		bool init_;
		bool pressed_[inputs+58];
		int last_state_;

		HAPI_TKeyboardData key_data_;

		static UserInput* instance_;



		
};

#endif//USER_INPUT_H_
