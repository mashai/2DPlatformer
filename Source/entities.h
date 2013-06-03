#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "utils.h"
#include <string>


//
// Structure
//

struct entityData {
	
	Coord2D position;
	bool on_screen;
	std::string type;
	std::string image;
	entityData() : type(""), position(Coord2D()), image(""), on_screen(0){}
	entityData(std::string t, Coord2D p, std::string i, bool s = false) : type(t), position(p), image(i), on_screen(s){}
};

//
// Base Class Entity
//

class Entity {
	public:

		Entity();

		virtual ~Entity();

		//init - initialises entity to the given values
		virtual void init(std::string type, Coord2D position, std::string image) = 0;

		//update - updates entity values
		virtual void update() = 0;

		//check_collision - checks for collision with another entity
		virtual void check_collision(Entity* other);

		//draw - draws the entity on the screen
		 virtual void draw(float blend) const;

		//hit - takes life away from entity
		virtual void hit(int damage = 1);

		//has_fired - if entity has fired returns data needed to spawn bullet
		virtual entityData has_fired() const;

		//is_alive - returns true if entity is alive, false otherwise
		bool is_alive() const;

		//get_direction - returns the direction the entity is facing
		int get_direction() const;

		//get_position - returns the (world) position of the entity
		Coord2D get_position() const;

		//get_bounding_rect - returns the bounding rectangle of an entity
		virtual Rect get_bounding_rect() const;

		//get_layer - returns the layer the entity is to be drawn on
		int get_layer() const;

		//get_type - returns the type of the entity
		std::string get_type() const;

	protected:

		//data members
		
		Coord2D world_position_;
		Coord2D local_position_;
		Coord2D last_local_pos_;
		Coord2D last_world_pos_;

		int width_;
		int height_;

		int life_;
		int speed_;
		int direction_;

		uint graphic_id_;
		int frame_;
		int layer_;

		std::string type_;
		std::string subtype_;

};

//
// Children Classes
//

class PlayerEntity: public Entity {
	public:

		PlayerEntity(int ground_level, std::string healthimage);

		~PlayerEntity();

		//init - initialises player to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates player values
		void update();

		//check_collision - checks for collision with another entity
		void check_collision(Entity* entity);

		//draw - draws the player and its objects on the screen
		void draw(float blend) const;

		//gain_points - gives points to player
		void gain_points(int points = 100);

		//won - sets bool won_ to true;
		void win();

		//has_fired - if player has fired returns data needed to spawn bullet
		entityData has_fired() const;

		//is_jumping - returns true if player is jumping, false otherwise
		bool is_jumping() const;

		//has_won - returns true if player reached goal, false otherwise
		bool has_won() const;

		//get_offset - returns the player offset
		Coord2D get_offset() const;

		//get_score - returns player's score
		int get_score() const;


	private:
		
		int velocity_;
		bool firing_;
		bool jumping_;
		int ground_level_;
		int level_;
		int score_;
		bool won_;
		
		int health_gid_;
};

class EnemyEntity: public Entity {
	public:

		EnemyEntity(PlayerEntity* player);

		~EnemyEntity();

		//init - initialises enemy to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates enemy values
		void update();

		//check_collision - checks for collision with another entity
		void check_collision(Entity* entity);

	private:

		int anim_increment_;

		bool player_hurt_;

		DWORD last_update_time_;

		PlayerEntity* player_;
};

class FlyingEnemyEntity: public Entity {
	public:

		FlyingEnemyEntity(PlayerEntity* player);

		~FlyingEnemyEntity();

		//init - initialises flyingenemy to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates enemy values
		void update();

		//has_fired - if flyingenemy has fired returns data needed to spawn bullet
		entityData has_fired() const;

		//get_bounding_rect - returns the bounding rectangle of the flying enemy
		Rect get_bounding_rect() const;

	private:

		int distance_;

		bool firing_;
		bool has_bullet_;

		DWORD last_update_time_;

		PlayerEntity* player_;
};

class ObjectEntity: public Entity {
	public:

		ObjectEntity(PlayerEntity* player);

		~ObjectEntity();

		//init - initialises object to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates object values
		void update();

		//get_bounding_rect - returns the bounding rectangle of an object
		Rect get_bounding_rect() const;

	private:

		PlayerEntity* player_;
};

class PickUpEntity: public Entity {
	public:

		PickUpEntity(PlayerEntity* player);

		~PickUpEntity();

		//init - initialises object to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates object values
		void update();

	private:

		PlayerEntity* player_;
};

class BulletEntity: public Entity {
	public:

		BulletEntity(PlayerEntity* player, int ground_level);

		~BulletEntity();

		//init - initialises bullet to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates bullet values
		void update();

		//check_collision - checks for collision with another entity
		void check_collision(Entity* entity);

	private:

		int velocity_;
		int ground_level_;

		PlayerEntity* player_;
};

class BackgroundEntity: public Entity {
	public:

		BackgroundEntity(PlayerEntity* player);

		~BackgroundEntity();

		//init - initialises background to the given values
		void init(std::string type, Coord2D position, std::string image);

		//update - updates background
		void update();

		//draw - draws the background
		 void draw(float blend) const;

	private:

		PlayerEntity* player_;

		Coord2D world_pos_;
		Coord2D local_pos_;
		Coord2D last_local_;

		float offset_;

};

#endif//ENTITIES_H_
