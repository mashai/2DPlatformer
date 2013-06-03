#include "entities.h"
#include "graphics.h"
#include "input.h"

#include <sstream>


//
// Class Entity Implementation
//

Entity::Entity() {
}

Entity::~Entity() {
}

void Entity::check_collision(Entity* other){
}

void Entity::draw(float blend)const {

	if(is_alive())
	{
		//calculate interpolated position
		Coord2D position = last_local_pos_ + (local_position_ - last_local_pos_)*blend;

		//and use it to draw entity
		GRAPHICS->draw_sprite(graphic_id_, position, frame_);
	}

}

void Entity::hit(int damage) {

	if(life_ - damage < 11)
	{
		life_ -= damage;
	}
	else
	{
		life_ = 10;
	}
}

entityData Entity::has_fired() const {

	return entityData();  
}

bool Entity::is_alive() const {

	if(life_ > 0)
		return true;
	else
		return false;
}

Coord2D Entity::get_position() const {
	
	return world_position_;
}

int Entity::get_direction() const {

	return direction_;
}

Rect Entity::get_bounding_rect() const {

	return Rect(world_position_.x, world_position_.y, world_position_.x + width_, world_position_.y - height_).percent(25);
}

int Entity::get_layer() const {

	return layer_;
}

std::string Entity::get_type() const {

	return type_;
}

//
// Class PlayerEntity Implementation
//

PlayerEntity::PlayerEntity(int ground_level, std::string health_image) {
	
	type_ = "player";
	life_ = 10;
	speed_ = 16;
	layer_ = 1;
	frame_ = 0;
	ground_level_ = ground_level;
	health_gid_ = GRAPHICS->create_sprite(health_image);
}

PlayerEntity::~PlayerEntity() {
}

void PlayerEntity::init(std::string type, Coord2D position, std::string image) {

	subtype_ = type;
	life_ = 10;
	world_position_ = position;
	local_position_ = position;
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	direction_ = 1;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
	jumping_ = false;
	firing_ = false;
	level_ = ground_level_;
	score_ = 0;
	won_ = 0;
}

void PlayerEntity::update() {

	//update positions
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;

	firing_ = false;

	//if input state is right, move player to the right
	if (INPUT->is_state(right))
	{
		world_position_.x += speed_;
		direction_ = 1;
		frame_ = 0;

		//if player isn't near the right end of screen, keep offset constant
		if(local_position_.x < (GRAPHICS->get_screen().width - (256 + width_)))
		{
			local_position_.x += speed_;
		}
	}
	//else if input state is left, move player to the left		
	else if (INPUT->is_state(left))
	{
		world_position_.x -= speed_;
		direction_ = -1;
		frame_ = 1;

		//if player isn't near the left end of screen, keep offset constant
		if(local_position_.x > 256)
		{
			local_position_.x -= speed_;
		}
	}

	//if input state is shoot, set firing_ to true
	if (INPUT->is_state(shoot))
	{
		firing_ = true;
	}

	//if input state is jump, enable jumping
	if(!jumping_ && INPUT->is_state(jump))
	{
		jumping_ = true;
		velocity_ = -40;
	}
	//if player is jumping
	if(world_position_.y < level_ || jumping_)
	{
		if(world_position_.y + velocity_ <= level_)
		{
			world_position_.y += velocity_;
			local_position_.y += velocity_;
			velocity_ += 8;
		}
		else
		{
			jumping_ = false;
			world_position_.y = level_;
			local_position_.y = level_;
		}
	}

	level_ = ground_level_;
}

void PlayerEntity::check_collision(Entity* entity) {
		
	Rect entity_rect = entity->get_bounding_rect();
	Rect player_rect = get_bounding_rect();

	//if player is colliding with entity on the x-axis
	if(entity_rect.x <  player_rect.width && entity_rect.width >  player_rect.x)
	{	
		//if player is above entity
		if(entity_rect.height >  player_rect.y) 
		{
			level_ = entity_rect.height;
		}
		//else if collision is happening on player's left and player is moving or facing left or if collision is happening on player's right and player is moving or facing right
		else if (( player_rect.width > entity_rect.x && direction_ == 1) || ( player_rect.x < entity_rect.width && direction_ == -1))
		{
				local_position_.x = last_local_pos_.x;
				world_position_.x = last_world_pos_.x;
		}
	}	
}

void PlayerEntity::draw(float blend)const {


	//calculate interpolated position
	Coord2D position = last_local_pos_ + (local_position_ - last_local_pos_)*blend;

	//and use it to draw player
	GRAPHICS->draw_sprite(graphic_id_, position, frame_);

	//draw health bar
	GRAPHICS->draw_sprite(health_gid_, Coord2D(32, 505), life_);

	//change font and convert score to string
	HAPI->ChangeFont("Cooper Black", 25, 300);
	std::ostringstream stream;
	stream << score_;

	//then render player score
	HAPI->RenderText(600, 475, HAPI_TColour(0, 0, 0), "Score : " + stream.str());

}

void PlayerEntity::gain_points(int points) {

	score_ += points;
}

void PlayerEntity::win() {
	
	won_ = true;
}

entityData PlayerEntity::has_fired() const {

	if(firing_)
	{
		return entityData("playerBullet", Coord2D(get_bounding_rect().x, world_position_.y), "assets\\bullet.png");
	}
	return entityData();
}

bool PlayerEntity::is_jumping() const {

	return jumping_;
}

bool PlayerEntity::has_won() const {

	return won_;
}

Coord2D PlayerEntity::get_offset() const {

	return world_position_ - local_position_;
}

int PlayerEntity::get_score() const {

	return score_;
}

//
// Class EnemyEntity Implementation
//

EnemyEntity::EnemyEntity(PlayerEntity* player) {

	type_ = "enemy";
	life_ = 0;
	speed_ = 8;
	layer_ = 1;
	frame_ = 0;
	player_ = player;
	last_update_time_ = 0;
}

EnemyEntity::~EnemyEntity() {
}

void EnemyEntity::init(std::string type, Coord2D position, std::string image) {

	subtype_ = type;
	life_ = 3;
	direction_ = -1;
	world_position_ = position;
	local_position_ = world_position_ - player_->get_offset();
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
	player_hurt_ = false;
}

void EnemyEntity::update() {

	//update positions
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	local_position_ = world_position_ - player_->get_offset();
		
	//if enemy can "see" player
	if(player_->get_position().x > world_position_.x -300 && player_->get_position().x < world_position_.x + 300)
	{
		//if player is on the left and enemy isn't colliding with anything, move left
		if(get_bounding_rect().width < player_->get_bounding_rect().x)
		{
			world_position_.x += speed_;
			anim_increment_ = 1;
			direction_ = 1;
			frame_ = 0;
		}
		//else if player is on the right and enemy isn't colliding with anything, move right
		else if(get_bounding_rect().x > player_->get_bounding_rect().width)
		{
			world_position_.x -= speed_;
			anim_increment_ = 1;
			direction_ = -1;
			frame_ = 5;
		}
	}
	
	//if enemy is colliding on the x-axis with player
	if(get_bounding_rect().width >= player_->get_bounding_rect().x && get_bounding_rect().x <= player_->get_bounding_rect().width)
	{	
		//if player is not above enemy, animate ennemy attaking
		if(get_bounding_rect().height < player_->get_bounding_rect().y && HAPI->GetTime() - last_update_time_ >= 50)
		{
			//if enemy hasn't started animation yet
			if(frame_ == 0 || frame_ == 5)
			{
				//and if player is on enemy's left, attack on the left
				if(get_bounding_rect().x > player_->get_bounding_rect().x)
				{
					direction_ = -1;
					frame_ = 6;
				}
				//else if player is on enemy's right, attack on the attack
				else if(get_bounding_rect().width < player_->get_bounding_rect().width)
				{
					direction_ = 1;
					frame_ = 1;
				}
			}
			if(frame_ == 1 || frame_ == 6)
			{
				anim_increment_ = 1;
			}
			else if(frame_ == 4 || frame_ == 9)
			{
				anim_increment_ = -1;
				player_->hit();
				player_->gain_points(-50);
			}

			frame_ += anim_increment_;
			last_update_time_ = HAPI->GetTime();
		}
		
		//else if enemy is armoured and player is above enemy
		else if(subtype_ == "armouredEnemy" && get_bounding_rect().height == player_->get_position().y && !player_hurt_)
		{
			player_->hit(2);
			player_hurt_ = true;
		}
	}
	
	//if player is jumping
	if(player_->is_jumping())
	{
		player_hurt_ = false;

	}
}

void EnemyEntity::check_collision(Entity* other){

	Rect other_rect = other->get_bounding_rect();
	Rect entity_rect = get_bounding_rect();
		
	//if collision is happening on entity's left and entity is moving or facing left
	if((other_rect.width > entity_rect.x && other_rect.x < entity_rect.x && direction_ == -1)

	//or if collision is happening on entity's right and entity is moving or facing right
	|| (other_rect.x < entity_rect.width && other_rect.width > entity_rect.width && direction_ == 1)

	//and if collision is happenning on the y-axis
	&& other_rect.y > entity_rect.height && other_rect.height < entity_rect.y)
	{	
		local_position_ = last_world_pos_ - player_->get_offset();
		world_position_ = last_world_pos_;
	}
}

//
// Class FlyingEnemyEntity Implementation
//

FlyingEnemyEntity::FlyingEnemyEntity(PlayerEntity* player) {

	type_ = "flyingenemy";
	life_ = 0;
	speed_ = 8;
	layer_ = 1;
	frame_ = 0;
	player_ = player;
	last_update_time_ = 0;
}

FlyingEnemyEntity::~FlyingEnemyEntity() {
}

void FlyingEnemyEntity::init(std::string type, Coord2D position, std::string image) {

	subtype_ = type;
	life_ = 3;
	direction_ = 1;
	distance_ = 0;
	world_position_ = position;
	local_position_ = world_position_ - player_->get_offset();
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
	firing_ = false;
}

void FlyingEnemyEntity::update() {

	//update positions
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	local_position_ = world_position_ - player_->get_offset();
	
	firing_ = false;

	world_position_.x -= speed_*direction_;
	distance_ ++;

	//if enemy has been moving in same direction for more than 30 ticks
	if(distance_ > 30)
	{
		//change direction
		direction_ *= -1;

		//and change animation direction
		if(direction_ == -1)
		{
			frame_ = 2;
		}
		else
		{
			frame_ = 0;
		}

		distance_ = 0;
		has_bullet_ = true;
	}

	//animate ennemy flying
	if(HAPI->GetTime() - last_update_time_ >= 200)
	{
		frame_ ++;

		if(frame_ == 2 || frame_ == 4)
		{
			frame_ -= 2;
		}
		last_update_time_ = HAPI->GetTime();
	}

	//if enemy is colliding on the x-axis with player and has a bullet
	if(get_bounding_rect().width >= player_->get_bounding_rect().x && get_bounding_rect().x <= player_->get_bounding_rect().width && has_bullet_)
	{
		firing_ = true;
		has_bullet_ = false;
	}
}

entityData FlyingEnemyEntity::has_fired()const{

	if(firing_)
	{	
		if(direction_ == 1)
		{
			return entityData("enemyBullet", Coord2D(world_position_.x - (width_/2), world_position_.y), "assets\\bullet.png");
		}
		else
		{
			return entityData("enemyBullet", Coord2D(world_position_.x + (width_/2), world_position_.y), "assets\\bullet.png");
		}
	}
	return entityData();
}

Rect FlyingEnemyEntity::get_bounding_rect() const {

	return Rect(world_position_.x, world_position_.y/2, world_position_.x + width_, world_position_.y - height_).percent(25);
}

//
// Class ObjectEntity Implementation
//

ObjectEntity::ObjectEntity(PlayerEntity* player) {

	type_ = "object";
	life_ = 0;
	speed_ = 0;
	layer_ = 0;
	frame_ = 0;
	player_ = player;
}

ObjectEntity::~ObjectEntity() {
}

void ObjectEntity::init(std::string type, Coord2D position, std::string image) {

	subtype_ = type;
	life_ = 1;
	world_position_ = position;
	local_position_ = world_position_ - player_->get_offset();
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
}

void ObjectEntity::update() {

	//update positions
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	local_position_ = world_position_ - player_->get_offset();
}

Rect ObjectEntity::get_bounding_rect() const {
		
	if(subtype_ == "startObject")
	{
		return Rect(world_position_.x, GRAPHICS->get_screen().height, world_position_.x + width_, GRAPHICS->get_screen().x);
	}
	else
	{
		return Rect(world_position_.x, world_position_.y, world_position_.x + width_, world_position_.y - height_);
	}
}

//
// Class PickUpEntity Implementation
//

PickUpEntity::PickUpEntity(PlayerEntity* player) {

	type_ = "pickup";
	life_ = 0;
	speed_ = 0;
	layer_ = 0;
	frame_ = 0;
	player_ = player;
}

PickUpEntity::~PickUpEntity() {
}

void PickUpEntity::init(std::string type, Coord2D position, std::string image) {

	subtype_ = type;
	life_ = 1;
	world_position_ = position;
	local_position_ = world_position_ - player_->get_offset();
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
}

void PickUpEntity::update() {

	//update positions
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	local_position_ = world_position_ - player_->get_offset();

	//if player is colliding with entity
	if(get_bounding_rect().width >= player_->get_bounding_rect().x && get_bounding_rect().x <= player_->get_bounding_rect().width
	&& get_bounding_rect().height <= player_->get_bounding_rect().y && get_bounding_rect().y >= player_->get_bounding_rect().height)
	{
		if(subtype_ == "healthPickUp")
		{
			player_->hit(-5);
		}
		else if(subtype_ == "bonusPickUp")
		{
			player_->gain_points(300);
		}
		else if(subtype_ == "goalPickUp")
		{
			player_->win();
		}
		
		life_ = 0;
	}
}

//
// Class BulletEntity Implementation
//

BulletEntity::BulletEntity(PlayerEntity* player, int ground_level) {

	type_ = "bullet";
	life_ = 0;
	layer_ = 1;
	frame_ = 0;
	player_ = player;
	ground_level_ = ground_level;
}

BulletEntity::~BulletEntity() {
}

void BulletEntity::init(std::string type, Coord2D position, std::string image) {

	subtype_ = type;
	life_ = 1;
	velocity_ = 3;
	direction_ = player_->get_direction();
	world_position_ = position;
	local_position_ = world_position_ - player_->get_offset();
	speed_ = 32 * direction_;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
}

void BulletEntity::update() {

	//update positions
	last_local_pos_ = local_position_;
	last_world_pos_ = world_position_;
	local_position_ = world_position_ - player_->get_offset();

	if(subtype_ == "playerBullet")
	{
		world_position_.x += speed_;

		//if entity leaves screen, delete it
		if(local_position_.x < GRAPHICS->get_screen().x || local_position_.x > GRAPHICS->get_screen().width)
		{
			life_ = 0;
		}
	}
	if(subtype_ == "enemyBullet")
	{
		//if bullet is colliding with player
		if(get_bounding_rect().y >= player_->get_bounding_rect().y && get_bounding_rect().width > player_->get_bounding_rect().x && get_bounding_rect().x < player_->get_bounding_rect().width)
		{
			player_->hit();
			player_->gain_points(-100);
			life_ = 0;
		}
		//if bullet has reached ground level
		int y = get_bounding_rect().y;
		if( y >  ground_level_)
		{
			life_ = 0;
		}

		world_position_.y += velocity_;
		velocity_ ++;
	}
}


void BulletEntity::check_collision(Entity* entity) {
	
	//if bullet is colliding with entity
	if((entity->get_bounding_rect().x < get_bounding_rect().width && entity->get_bounding_rect().width > get_bounding_rect().x)
	&& (entity->get_bounding_rect().y > get_bounding_rect().height && entity->get_bounding_rect().height < get_bounding_rect().y))
	{
		//inflict damage to entity if not an object
		if(entity->get_type() != "object")
		{
			entity->hit();

			if(!entity->is_alive())
			{
				player_->gain_points();
			}
		}
		//and kill bullet
		life_ = 0;
	}
}

//
// Class BackgroundEntity Implementation
//

BackgroundEntity::BackgroundEntity(PlayerEntity* player) {

	type_ = "background";
	life_ = 1;
	layer_ = 0;
	frame_ = 0;
	player_ = player;
}

BackgroundEntity::~BackgroundEntity() {
}

void BackgroundEntity::init(std::string type, Coord2D position, std::string image) {

	type_ = type;
	life_ = 1;
	graphic_id_ = GRAPHICS->create_sprite(image);
	width_ = GRAPHICS->get_sprite_rect(graphic_id_).width;
	height_ = GRAPHICS->get_sprite_rect(graphic_id_).height;
	world_position_ = position;
	world_pos_ = Coord2D(position.x + width_, position.y);
	local_position_ = world_position_ - player_->get_offset();
	local_pos_ = world_pos_ - player_->get_offset();
}

void BackgroundEntity::update() {

	//update last positions
	last_local_ = local_position_;
	last_local_pos_ = local_pos_;

	//if the right end of the 1st background has reached the end of the screen, set the 2nd background to the 1st background's right
	if(world_position_.x + width_  <= GRAPHICS->get_screen().width + player_->get_offset().x)
	{
		world_pos_.x = world_position_.x + width_;
		last_local_pos_.x = last_local_.x + width_;
	}
	//else if the left end of the 1st background has reached the start of the screen, set it to the left
	else if(world_position_.x >= player_->get_offset().x)
	{
		world_pos_.x = world_position_.x - width_;
		last_local_pos_.x = last_local_.x - width_;
	}
	
	//if the right end of the 2nd background has reached the end of the screen, set the 2nd background to the 2nd background's right
	if(world_pos_ .x + width_  <= GRAPHICS->get_screen().width + player_->get_offset().x)
	{
		world_position_.x = world_pos_.x + width_;
		last_local_.x = last_local_pos_.x + width_;
	}
	//else if the left end of the 2nd background has reached the start of the screen, set it to the left
	else if(world_pos_ .x >= player_->get_offset().x)
	{
		world_position_.x = world_pos_.x - width_;
		last_local_.x = last_local_pos_.x - width_;
	}
	
	//update local positions
	local_position_ = world_position_ - player_->get_offset();
	local_pos_ = world_pos_ - player_->get_offset();
}

void BackgroundEntity::draw(float blend)const {

	//calculate interpolated position
	Coord2D position = last_local_ + (local_position_ - last_local_)*blend;
	Coord2D pos = last_local_pos_ + (local_pos_ - last_local_pos_)*blend;

	//draw background at interpolated positions
	GRAPHICS->draw_sprite(graphic_id_, position, frame_);
	GRAPHICS->draw_sprite(graphic_id_, pos, frame_);
}
