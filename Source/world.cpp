#include "world.h"
#include "utils.h"
#include "graphics.h"
#include "input.h"

#include <fstream> 

//
// Class WorldState Implementation
//

WorldState::WorldState(): menu_(0), gameover_(0){

	if(!load_level()) //if level doesn't load
	{
		//Displays an error message saying that file is missing or corrupted
		HAPI->UserMessage("Level loading file is missing or corrupted, game cannot load", "Error:LoadLevel");

		menu_ = true;
	}
}

WorldState::~WorldState() {

	//deletes entities
	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		delete entity_vector_[i];
	}
}

void WorldState::init() {

	gameover_ = false;

	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		entity_vector_[i]->hit(10);
	}

	for(uint i = 0; i < data_vector_.size(); i++)
	{
		data_vector_[i].on_screen = false;
	}
}

void WorldState::update() {

	load_screen(); //spawn on-screen entities

	if(!player_->is_alive() && !player_->is_jumping())
	{
		gameover_ = true;
	}
	else if(player_->has_won() && !player_->is_jumping())
	{
		gameover_ = true;
	}

	//check if any entities have fired
	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		if(entity_vector_[i]->is_alive() && !entity_vector_[i]->has_fired().on_screen)
		{
			//and spawn bullets if they have
			spawn_entity(entity_vector_[i]->has_fired());
		}
	}

	//search for living entities
	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		if(entity_vector_[i]->is_alive())
		{
			//and update them
			entity_vector_[i]->update();
		}
	}
		
	//check collisions for player and enemies
	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		for(uint j = 0; j < entity_vector_.size(); j++)
		{
			if(entity_vector_[i]->is_alive() && entity_vector_[j]->is_alive() && (entity_vector_[j]->get_type() == "enemy" || entity_vector_[j]->get_type() == "object") && i != j)
			{
				entity_vector_[i]->check_collision(entity_vector_[j]);
			}
		}
	}
}


void WorldState::draw(float blend) {

	GRAPHICS->clear_screen(0, 0, 0);

	//search for background elements (objects, pick-ups and bullets)
	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		if(entity_vector_[i]->get_layer() == 0)
		{
			//and draw them
			entity_vector_[i]->draw(blend);
		}
	}

	//then search for the rest of the elements (enemy and player)
	for(uint i = 0; i < entity_vector_.size(); i++)
	{
		if(entity_vector_[i]->get_layer() == 1)
		{
			//and draw them
			entity_vector_[i]->draw(blend);
		}
	}
}

bool WorldState::load_level() {

    //open file level.txt
	std::fstream data("level.txt");
	
	std::string temp;
	std::string image, entity;
	std::string number, posx, posy;
	std::string ground_level;
	std::string health_bar;

	//if file didn't open
	if(!data.is_open())
	{
		return false;
	}

	// *LOAD LEVEL DATA*

	//ignore text until @ symbol
    data.ignore(100, '@');

    //check data wasn't corrupted
	getline(data, temp);
	if(temp != "level data")
	{
		return false;
	}
	//load data
	std::getline(data, temp, ' ');
	std::getline(data, ground_level);
	std::getline(data, temp, ' ');
	std::getline(data, background_image_);
	std::getline(data, temp, ' ');
	std::getline(data, health_bar);


	// *LOAD IMAGE DATA*

	//ignore text until @ symbol
	data.ignore(100, '@');

	//check data wasn't corrupted
    getline(data, temp);
	if(temp != "image data")
	{
		return false;
	}
	//while end of image data hasn't been reached
	while(data.peek() != '\n')
	{
		//load texture data
		std::getline(data, image, ' ');
		std::getline(data, number);

		//load texture
		GRAPHICS->create_sprite(image, atoi(number.c_str()));
	}


	// *LOAD MAX ENTITIES*

	//ignore text until @ symbol
	data.ignore(100, '@');

	//check data wasn't corrupted
    getline(data, temp);
	if(temp != "max entities")
	{
		return false;
	}
	//dynamically allocate memory for player
	player_ = new PlayerEntity(atoi(ground_level.c_str()), health_bar);
	entity_vector_.push_back(player_);
	
	//dynamically allocate memory for background
	background_ = new BackgroundEntity(player_);
	entity_vector_.push_back(background_);

	//load max enemy data
	std::getline(data, entity, ' ');
	std::getline(data, number);
	//and dynamically allocate memory for enemy
	for(int i = 0; i < atoi(number.c_str()); i++)
	{
		EnemyEntity* enemy = new EnemyEntity(player_);
		entity_vector_.push_back(enemy);
	}

	//load max enemy data
	std::getline(data, entity, ' ');
	std::getline(data, number);
	//and dynamically allocate memory for enemy
	for(int i = 0; i < atoi(number.c_str()); i++)
	{
		FlyingEnemyEntity* flyingenemy = new FlyingEnemyEntity(player_);
		entity_vector_.push_back(flyingenemy);
	}

	//load max object data
	std::getline(data, entity, ' ');
	std::getline(data, number);
	//and dynamically allocate memory for object
	for(int i = 0; i < atoi(number.c_str()); i++)
	{
		ObjectEntity* object = new ObjectEntity(player_);
		entity_vector_.push_back(object);
	}

	//load max pickup data
	std::getline(data, entity, ' ');
	std::getline(data, number);
	//and dynamically allocate memory for pickup
	for(int i = 0; i < atoi(number.c_str()); i++)
	{
		PickUpEntity* pickup = new PickUpEntity(player_);
		entity_vector_.push_back(pickup);
	}
	
	//load max bullet data
	std::getline(data, entity, ' ');
	std::getline(data, number);
	//and dynamically allocate memory for bullet
	for(int i = 0; i < atoi(number.c_str()); i++)
	{
		BulletEntity* bullet = new BulletEntity(player_, atoi(ground_level.c_str()));
		entity_vector_.push_back(bullet);
	}


	// *LOAD ENTITY DATA*

	//ignore text until @ symbol
	data.ignore(100, '@');

	//check data wasn't corrupted
    getline(data, temp);
	if(temp != "entity data")
	{
		return false;
	}
	//load background data and store it in data_vector
	data_vector_.push_back(entityData("Background", Coord2D(0, GRAPHICS->get_screen().height), background_image_));

	//while end of entity data hasn't been reached
	while(data.peek() != '\n')
	{
		//load entity data
		std::getline(data, entity, ' ');
		std::getline(data, posx, ' ');
		std::getline(data, posy, ' ');
		std::getline(data, image);

		//and store it in data_vector
		data_vector_.push_back(entityData(entity, Coord2D(atoi(posx.c_str()), atoi(posy.c_str())), image));
	}

	//close stream
	data.close();

	return true;
}

void WorldState::load_screen() {
	
	for(uint i = 0; i < data_vector_.size(); i++)
	{
		//if entity should be on-screen but isn't yet, spawn it
		if(data_vector_[i].on_screen == false && data_vector_[i].position.x - player_->get_offset().x < GRAPHICS->get_screen().width*2 )
		{
			spawn_entity(data_vector_[i]);
			data_vector_[i].on_screen = true;
		}
	}
}

bool WorldState::spawn_entity(entityData data) const {

	
	if(data.type.find("Player") != std::string::npos)
	{
		//initialize player to the given values
		player_->init(data.type, data.position, data.image);
		return true;
	}
	else if(data.type.find("Background") != std::string::npos)
	{
		//initialize background to the given values
		background_->init(data.type, data.position, data.image);
		return true;
	}
	else if(data.type.find("FlyingEnemy") != std::string::npos)
	{
		//search for an unused enemy entity
		for(uint i = 0; i < entity_vector_.size(); i++)
		{
			if(entity_vector_[i]->get_type() ==  "flyingenemy" && !entity_vector_[i]->is_alive())
			{
				//and initialize it to the given values
				entity_vector_[i]->init(data.type, data.position, data.image);
				return true;
			}
		}
	}
	else if(data.type.find("Enemy") != std::string::npos)
	{
		//search for an unused enemy entity
		for(uint i = 0; i < entity_vector_.size(); i++)
		{
			if(entity_vector_[i]->get_type() ==  "enemy" && !entity_vector_[i]->is_alive())
			{
				//and initialize it to the given values
				entity_vector_[i]->init(data.type, data.position, data.image);
				return true;
			}
		}
	}
	else if(data.type.find("Object") != std::string::npos)
	{
		//search for an unused object entity
		for(uint i = 0; i < entity_vector_.size(); i++)
		{
			if(entity_vector_[i]->get_type() ==  "object" && !entity_vector_[i]->is_alive())
			{
				//and initialize it to the given values
				entity_vector_[i]->init(data.type, data.position, data.image);
				return true;
			}
		}
	}
	else if(data.type.find("PickUp") != std::string::npos)
	{
		//search for an unused object entity
		for(uint i = 0; i < entity_vector_.size(); i++)
		{
			if(entity_vector_[i]->get_type() ==  "pickup" && !entity_vector_[i]->is_alive())
			{
				//and initialize it to the given values
				entity_vector_[i]->init(data.type, data.position, data.image);
				return true;
			}
		}
	}
	else if(data.type.find("Bullet") != std::string::npos)
	{
		//search for an unused bullet entity
		for(uint i = 0; i < entity_vector_.size(); i++)
		{
			if(entity_vector_[i]->get_type() ==  "bullet" && !entity_vector_[i]->is_alive())
			{
				//and initialize it to the given values
				entity_vector_[i]->init(data.type, data.position, data.image);
				return true;
			}
		}
	}

	return false;
}

int WorldState::get_next_state() const{

	if(gameover_)
	{
		return gameover;
	}
	else if(menu_)
	{
		return menu;
	}
	return play;
}

int WorldState::get_score() const{

	return player_->get_score();
}

bool WorldState::has_won() const{

	return player_->has_won();
}