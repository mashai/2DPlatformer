#include "menu.h"
#include "utils.h"
#include "input.h"
#include "graphics.h"

#include <fstream>

//
// Class MenuState Implementation
//

MenuState::MenuState():play_(0), selected_choice_(0), score_loaded_(0){

	//load images needed in the menu
	background_id_ = GRAPHICS->create_sprite("assets\\background.png");
	instructions1_id_ = GRAPHICS->create_sprite("assets\\instructions1.png");
	instructions2_id_ = GRAPHICS->create_sprite("assets\\instructions2.png");
	score_id_ = GRAPHICS->create_sprite("assets\\blank.png");
	instructions_id_ = instructions1_id_;
}

MenuState::~MenuState(){
}

void MenuState::init() {

	play_ = false;
	selected_choice_ = false;
	score_list_.clear();
	score_loaded_ = load_scores();
	HAPI->ChangeFont("Cooper Black", 30, 200);
}

void MenuState::update(){


	//check for input and change selection accordingly
	if(INPUT->is_state(up) && selected_choice_ > 0)
	{
		selected_choice_ --;
	}
	else if(INPUT->is_state(down) && selected_choice_ < menuChoices -1)
	{
		selected_choice_ ++;
	}
	else if(INPUT->is_state(left) && selected_choice_ == menuInstructions)
	{
		instructions_id_ = instructions1_id_;
	}
	else if(INPUT->is_state(right) && selected_choice_ == menuInstructions)
	{
		instructions_id_ = instructions2_id_;
	}

	//act on selection
	switch (selected_choice_)
	{
	case menuPlay:
		{
			if(INPUT->is_state(select))
			{
				play_ = true;
			}
			selected_id_ = 0;;
			break;
		}
	case menuInstructions:
		{
			selected_id_ = instructions_id_;
			break;
		}
	case menuScore:
		{
			selected_id_ = score_id_;
			break;
		}
	case menuExit:
		{
			if(INPUT->is_state(select))
			{
				HAPI->Close();
			}
			selected_id_ = 0;
			break;
		}
	default:
		{
			break;
		}

	}
}

void MenuState::draw(float blend){

	const char* menu_text[menuChoices]=
	{
		"PLAY",
		"INSTRUCTIONS",
		"HIGH SCORES",
		"EXIT"
	};

	HAPI_TColour menu_colour(0,0,0);
	HAPI_TColour selection_colour(107,142,35);

	GRAPHICS->clear_screen(255,255, 255);
	GRAPHICS->draw_sprite(background_id_, Coord2D(0,GRAPHICS->get_screen().height + 85));

	//draw the text for the menu
	int yPos = 20;
	for (int i = 0; i < menuChoices; i++)
	{
		if (i == selected_choice_)
			HAPI->RenderText(50 ,yPos,selection_colour,menu_text[i]);
		else
			HAPI->RenderText(50,yPos,menu_colour,menu_text[i]);

		yPos += 50;
	}

	//if selected_id is true, draw it
	if(selected_id_)
	{
		GRAPHICS->draw_sprite(selected_id_, Coord2D(210,450));
	}

	//and if user has selected high scores
	if(selected_choice_ == menuScore)
	{
		//draw the text if it was properly loaded
		if(score_loaded_)
		{
			HAPI->RenderText(320, 220, menu_colour, "NAME");
			HAPI->RenderText(500, 220, menu_colour, "SCORE");

			int yPos = 270;
			for(uint i = 0; i < score_list_.size(); i += 2)
			{
				HAPI->RenderText(320, yPos, menu_colour, score_list_[i]);
				HAPI->RenderText(500, yPos, menu_colour, score_list_[i+ 1]);
				yPos += 30;
			}
		}

		//otherwise draw different text
		else
		{
			HAPI->RenderText(230, 220, menu_colour, "High scores could not be loaded...");
		}
	}
}

bool MenuState::load_scores(){

	//open file score.txt
	std::fstream data("high_score.txt");

	//if file didn't open
	if(!data.is_open())
	{
		return false;
	}

	//move pointer to @scores
	std::string temp;
	while(temp != "scores" && "end")
	{
		data.ignore(100, '@');
		getline(data, temp);
	}

	//load data
	while(data.peek() != '\n')
	{
		std::getline(data, temp, ' ');
		score_list_.push_back(temp);
		std::getline(data, temp);
		score_list_.push_back(temp);
	}

	data.close();

	return true;
}

int MenuState::get_next_state() const{

	if(play_)
	{
		return play;
	}
	return menu;
}