#include "input.h"



//
//Singleton Class UserInput Implementation
//

UserInput* UserInput::instance_ = 0;

UserInput* UserInput::instance() {
	
	if(instance_ == 0) //if instance has never been called before
	{
		instance_ = new UserInput(); //call constructor
	}
	return instance_; //always return pointer to the UserInput instance
}

void UserInput::init() {

	if(init_ == 0) //if userInput has never been initialized
	{
		for(int i = 0; i < inputs; i++)
		{
			pressed_[i] = false;
		}

		init_ = 1; //userInput has been initialized
	}
}

void UserInput::destroy(){

	//deletes allocated memory for userIput
	if(instance_)
	{
		delete instance_;
	}
	instance_ = 0;
}

UserInput::UserInput() : init_(0), last_state_(0) {
}

UserInput::UserInput(UserInput& user_input) {
}

void UserInput::update() {

	//get updated values from HAPI
	HAPI->GetKeyboardData(&key_data_);
}

bool UserInput::is_state(int state) {

	switch (state)
	{
	case right:
		
			if(key_data_.scanCode[HK_RIGHT])
			{
				last_state_ = right;
				return true;
			}
			return false;
		
		case left:
		
			if(key_data_.scanCode[HK_LEFT])
			{
				last_state_ = left;
				return true;
			}
			return false;

		case up:
		
			if(key_data_.scanCode[HK_UP] && !pressed_[up])
			{
				pressed_[up] = true;
				return true;
			}

			if(!key_data_.scanCode[HK_UP])
			{
				pressed_[up] = false;
			}
			return false;
		
		case down:
		
			if(key_data_.scanCode[HK_DOWN] && !pressed_[down])
			{
				pressed_[down] = true;
				return true;
			}

			if(!key_data_.scanCode[HK_DOWN])
			{
				pressed_[down] = false;
			}
			return false;

	case jump:
		
			if(key_data_.scanCode[HK_UP] && !pressed_[jump])
			{
				pressed_[jump] = true;
				return true;
			}
			
			if(!key_data_.scanCode[HK_UP])
			{
				pressed_[jump] = false;
			}
			return false;
		
	case shoot:

			if(key_data_.scanCode[HK_SPACE] && !pressed_[shoot])
			{
				pressed_[shoot] = true;
				return true;
			}
			
			if(!key_data_.scanCode[HK_SPACE])
			{
				pressed_[shoot] = false;
			}
			return false;

	case select:

		if(key_data_.scanCode[HK_RETURN] && !pressed_[select])
		{
			pressed_[select] = true;
			return true;
		}

		if(!key_data_.scanCode[HK_RETURN])
		{	
			pressed_[select] = false;
		}
			return false;


	case backspace:

		if(key_data_.scanCode[HK_BACK] && !pressed_[backspace])
		{
			pressed_[backspace] = true;
			return true;
		}

	if(!key_data_.scanCode[HK_BACK])
		{
			pressed_[backspace] = false;
		}
		return false;

	default:
		
		last_state_ = null;
		return false;
	}
}

bool UserInput::any_key_pressed(){

	//iterate through all items in key_data
	for(int i = 0; i < 256; i++)
	{
		//and return true if one of them is true
		if(key_data_.scanCode[i])
		{
			return true;
		}
	}
	return false;
}

int UserInput::get_char(){

	
	//iterate z - A times to check keypress array
	for(int i = 0; i <= 'z'-'A'; i++)
	{
		//if the character is pressed and was released since last pressed
		if(key_data_.scanCode[i+ 'A']  && !pressed_[inputs+i])
		{
			//set character pressed as true
			pressed_[inputs+i] = true;
			//and return lower case value
			return i+'A'+ 32;
		}
		//if character isn't pressed
		if(!key_data_.scanCode[i+ 'A'])
		{
			//set character pressed as false
			pressed_[inputs+i] = false;
		}
	}

	return -1;
}

int UserInput::get_last_state(){
	
	return last_state_;
}