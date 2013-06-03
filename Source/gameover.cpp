#include "gameover.h"
#include "utils.h"
#include "input.h"
#include "graphics.h"

#include <fstream>

//
// Class GameOverState Implementation
//

GameOverState::GameOverState(WorldState* world):menu_(0), score_loaded_(0), new_high_score_(0), world_state_(world){

	//load images needed
	background_id_ = GRAPHICS->create_sprite("assets\\background.png");
	score_id_ = GRAPHICS->create_sprite("assets\\smallblank.png");
}

GameOverState::~GameOverState(){
}

void GameOverState::init() {

	menu_ = false;
	name_ = " ";
	score_list_.clear();
	player_list_.clear();
	score_loaded_ = load_scores();

	//if player score is biggest than the smallest score in high_scores
	if(world_state_->get_score() > score_list_[score_list_.size() - 1])
	{
		//set new_high_score to true
		new_high_score_ = true;
	}
	HAPI->ChangeFont("Cooper Black", 30, 200);
}

void GameOverState::update(){

	if(new_high_score_)
	{
		//get character from input
		char c = INPUT->get_char();

		//if character is valid and the string doesn't exceed its max length
		if(c != -1  && name_.length() < 10)
		{
			//if name is empty, set it to character
			if(name_ == " ")
			{
				name_ = c;
			}
			//add character to string
			else
			{
				name_ +=c;
			}
		}

		//if backspace occured and string is valid
		if(INPUT->is_state(backspace) && name_.length() != 0)
		{
			//delete last character from string
			name_.resize(name_.length() - 1);
		}

		//if select occured
		if(INPUT->is_state(select))
		{
			//add new score and name to file and set new_high_score to false
			new_high_score_ = false;
			update_scores(world_state_->get_score());
			write_scores();
		}
	}
	else if(INPUT->any_key_pressed())
	{
		//if any key is pressed, return to menu
		menu_ = true;
	}
}

void GameOverState::draw(float blend){

	const std::string win = "CONGRATULATIONS!";
	const std::string loose = "YOU LOST...";
	const std::string anykey = "press any key to return to menu";
	const std::string highscore = "NEW HIGH SCORE";
	const std::string entername = "enter your name:";
	const std::string score = "score:";

	const HAPI_TColour black_colour(0,0,0);
	const HAPI_TColour green_colour(107,142,35);

	GRAPHICS->clear_screen(255,255, 255);
	GRAPHICS->draw_sprite(background_id_, Coord2D(0,GRAPHICS->get_screen().height + 85));

	if(world_state_->has_won())
	{
		HAPI->RenderText(250, 100, green_colour, win);
	}
	else
	{
		HAPI->RenderText(320, 100, green_colour, loose);
	}

	if(new_high_score_)
	{
		char player_score[5];
		_itoa_s(world_state_->get_score(), player_score, 10);

		GRAPHICS->draw_sprite(score_id_, Coord2D(170,450));
		HAPI->RenderText(280, 220, black_colour, highscore);
		HAPI->RenderText(200, 280, black_colour, entername);
		HAPI->RenderText(450, 280, black_colour, name_);
		HAPI->RenderText(200, 320, black_colour, score);
		HAPI->RenderText(300, 320, black_colour, player_score);

	}
	else
	{
		HAPI->RenderText(200, 200, black_colour, anykey);
	}
}

void GameOverState::update_scores(int player_score){

	std::vector<std::string>::iterator name_it = player_list_.begin();
	std::vector<int>::iterator score_it = score_list_.begin();

	//iterate through player and score list and insert player score and name if bigger then current score
	for(uint i = 0; i < score_list_.size(); i++)
	{
		if(*score_it < player_score)
		{
			score_list_.insert(score_it, player_score);
			player_list_.insert(name_it, name_);
			score_list_.pop_back();
			player_list_.pop_back();
			break;
		}
		name_it++;
		score_it++;
	}
}

bool GameOverState::load_scores() {

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
		player_list_.push_back(temp);
		std::getline(data, temp);
		score_list_.push_back(atoi(temp.c_str()));
	}

	data.close();

	return true;

}

bool GameOverState::write_scores(){

	//open file score.txt
	std::fstream data("high_score.txt");

	//if file didn't open
	if(!data.is_open())
	{
		return false;
	}

	const std::string format_begin = "HIGH SCORES\n\n@scores\n";
	const std::string format_end = "\n@end";

	//write beginning format
	data.write(format_begin.c_str(), format_begin.length());

	//write data
	for(uint i = 0; i < score_list_.size(); i++)
	{
		data.write(player_list_[i].c_str(), player_list_[i].length());
		data.put(' ');

		char score[5];
		_itoa_s(score_list_[i], score, 10);

		data.write(score, 5);
		data.put('\n');
	}

	//write ending format
	data.write(format_end.c_str(), format_end.length());
	data.close();

	return true;
}

int GameOverState::get_next_state() const{

	if(menu_)
	{
		return menu;
	}
	return gameover;
}