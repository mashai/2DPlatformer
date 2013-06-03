#include "graphics.h"
#include <assert.h>


//
//Singleton Class ScreenBuffer Implementation
//

ScreenBuffer* ScreenBuffer::instance_ = 0;

ScreenBuffer* ScreenBuffer::instance() {
	
	if(instance_ == 0) //if instance has never been called before
	{
		instance_ = new ScreenBuffer(); //call constructor.
	}
	return instance_; //always return pointer to the ScreenBuffer instance
}

void ScreenBuffer::init(int screen_width, int screen_height) {

	if(init_ == 0) //if screenBuffer has never been initialized
	{
		screen_ = Rect(0, 0, screen_width, screen_height);
		assert(screen_.height && screen_.width);

		screen_pointer_ = HAPI->GetScreenPointer();
		assert(screen_pointer_);

		init_ = 1; //screenbuffer has been initialized
	}
}

void ScreenBuffer::destroy(){

	//deletes sprites
	for(uint i = 0; i < sprite_vector_.size(); i++)
		delete sprite_vector_[i].texture;
	
	//deletes allocated memory for screenbuffer
	if(instance_)
	{
		delete instance_;
	}
	instance_ = 0;
}

ScreenBuffer::ScreenBuffer() : screen_pointer_(0), init_(0) {
}

ScreenBuffer::ScreenBuffer(ScreenBuffer& screen_buffer) {
}

void ScreenBuffer::clear_screen(BYTE r, BYTE g, BYTE b) {

	assert(init_);//checks that ScreenBuffer was properly initialized

	
	if( r == g && g == b ) // if colour is a shade of gray
	{
		memset(screen_pointer_, r, (screen_.height)*(screen_.width)*4);
	}
	else //if colour isn't a shade of grey
	{
		//passing rgb values to colour_
		colour_.red = r;
		colour_.green = g;
		colour_.blue = b;
		colour_.alpha = 255;

		offset_ = 0; //reset offset to zero

		for(int y = 0; y < (screen_.height); y++)
		{
			for(int x = 0; x < (screen_.width); x++)
			{
				memcpy(screen_pointer_ + offset_, &colour_, 4);
				offset_ += 4;
			}
		}

		//for(int y = 0; y < (screen_->height); y++)
		//{
		//	for(int x = 0; x < (screen_->width); x++)
		//	{
		//		screen_pointer_[offset_] = b;
		//		screen_pointer_[offset_ + 1] = g;
		//		screen_pointer_[offset_ + 2] = r;
		//		offset_ += 4;
		//	}
		//}
	}

}

void ScreenBuffer::set_pixel(Coord2D position, BYTE r, BYTE g, BYTE b) {

	assert(init_);//checks that ScreenBuffer was properly initialized


	if(position.x >= screen_.width || position.x < 0 || position.y >= screen_.height || position.y <0) //if out of bounds, return
	{
		HAPI->DebugText("ScreenBuffer::set_pixel returns without drawing to screen because position is out of boundaries");
		return;
	}
	else
	{
		//passing rgb values to colour_
		colour_.red = r;
		colour_.green = g;
		colour_.blue = b;
		colour_.alpha = 0;
		
		offset_ = (position.y * screen_.width + position.x)*4;

		memcpy(screen_pointer_ + offset_, &colour_, 4);

		//screen_pointer_[offset_] = b;
		//screen_pointer_[offset_ + 1] = g;
		//screen_pointer_[offset_ + 2] = r;
	}
}

int ScreenBuffer::create_sprite(const std::string& file_name, int frame_number) {

	//check if texture already exists
	for(uint i = 0; i < sprite_vector_.size(); i++)
	{
		//if it exists
		if(file_name == sprite_vector_[i].name)
		{
			return i; //return indice of the sprite at which the texture is stored
		}
	}
	Texture* sprite = new Texture(file_name, frame_number); //otherwise create a new texture
	
	sprite_vector_.push_back(Sprite(file_name, sprite));//and store it inside v_sprite_ vector
	
	return sprite_vector_.size() - 1;//return indice at which the sprite is stored (will never change since sprites cannot be deleted unless the whole vector is cleared)
}

Rect ScreenBuffer::get_sprite_rect(uint id) const {

	//assert id is valid
	assert(id < sprite_vector_.size());

	//and return corresponding sprite rectangle
	return *sprite_vector_[id].texture->get_rectangle();
}

void ScreenBuffer::draw_sprite(uint id, Coord2D position, int frame_id){

	if(id < sprite_vector_.size())
	{
		//change y coordinate to represent the bottom the sprite
		position.y -= sprite_vector_[id].texture->get_rectangle()->height;

		//access sprite and assign it a position at which to draw on screen
		sprite_vector_[id].texture->set_position(position);

		//access sprite and set it to an arbitrary frame
		sprite_vector_[id].texture->set_frame(frame_id);

		//draw sprite to screen with transparency
		blit_alpha(sprite_vector_[id].texture, HAPI->GetScreenPointer(), get_screen());
	}

}

void ScreenBuffer::blit(Texture* t_source, BYTE *destination, Rect r_destination) {

	assert(init_);//checks that ScreenBuffer was properly initialized


	//if source is out of bounds, don't blit it
	if( t_source->out_of_bounds(r_destination) )
		return;
	
	Rect* r_source = t_source->clip(r_destination); //allows access to the xy coordinates, width and height of the rectangle to be blitted

	//assign pointers to data (destination is calculated with offset)
	dest_pointer_ = destination + (r_source->x + r_source->y * r_destination.width)* 4;
	source_pointer_ = t_source->get_pointer();

	//if x coordinate of source is smaller than x coordinate of destination, adjusts pointer offset
	if(t_source->get_rectangle()->x < r_destination.x)
		source_pointer_ -= (t_source->get_rectangle()->x * 4);

	//if y coordinate of source is smaller than y coordinate of destination, adjusts pointer offset
	if(t_source->get_rectangle()->y < r_destination.y)
		source_pointer_ -= (t_source->get_rectangle()->y * t_source->get_rectangle()->width * 4 * t_source->get_frame_number());


	for(int y = 0; y < r_source->height; y++)
	{
		//copy line from source to destination
		memcpy(dest_pointer_, source_pointer_, r_source->width *4);

		//move to next line
		source_pointer_ += t_source->get_rectangle()->width * 4 * t_source->get_frame_number();
		dest_pointer_ += r_destination.width *4;
	}
}

void ScreenBuffer::blit_alpha(Texture* t_source, BYTE *destination, Rect r_destination) {

	assert(init_);//checks that ScreenBuffer was properly initialized


	//if source is out of bounds, don't blit it
	if( t_source->out_of_bounds(r_destination) )
		return;

	Rect* r_source = t_source->clip(r_destination); //allows access to the xy coordinates, width and height of the rectangle to be blitted

	//assign pointers to data (destination is calculated with offset)
	dest_pointer_ = destination + (r_source->x + r_source->y * r_destination.width)* 4;
	source_pointer_ = t_source->get_pointer();
	
	//if x coordinate of source is smaller than x coordinate of destination, adjusts pointer offset
	if(t_source->get_rectangle()->x < r_destination.x)
		source_pointer_ -= (t_source->get_rectangle()->x * 4);

	//if y coordinate of source is smaller than y coordinate of destination, adjusts pointer offset
	if(t_source->get_rectangle()->y < r_destination.y)
		source_pointer_ -= (t_source->get_rectangle()->y * t_source->get_rectangle()->width * 4 * t_source->get_frame_number());


	for(int y = 0; y < (r_source->height); y++)
	{
		for(int x = 0; x < (r_source->width); x++)
		{
			//store rgba values from source
			BYTE blue = source_pointer_[0];
			BYTE green = source_pointer_[1];
			BYTE red = source_pointer_[2];
			BYTE alpha = source_pointer_[3];

			if(alpha == 255)//if pixel doesn't have transparency
			{
				//assign rgb value from source to destination
				dest_pointer_[0] = blue;  
				dest_pointer_[1] = green;
				dest_pointer_[2] = red;
			}
			else if (alpha != 0)//else, if it's not completly transparent
			{
				//assign rgb value that is a percentage of source and destination combined
				dest_pointer_[0] += ((alpha*(blue-dest_pointer_[0]))>>8);
				dest_pointer_[1] += ((alpha*(green-dest_pointer_[1]))>>8);
				dest_pointer_[2] += ((alpha*(red-dest_pointer_[2]))>>8);
			}

		//move to next pixel
		source_pointer_ += 4;
		dest_pointer_ += 4;

		}

	//move to next line
	dest_pointer_ += (r_destination.width - r_source->width)*4;
	source_pointer_ += ((t_source->get_rectangle()->width * t_source->get_frame_number()) - r_source->width)*4;

	}
}

Rect ScreenBuffer::get_screen()const {

	assert(init_);//checks that ScreenBuffer was properly initialized

	return screen_;
}

//
// Class Texture Implementation
//

Texture::Texture(const std::string& file_name, int frame_number): pointer_(0), rectangle_(0), clip_rectangle_(0), width_(156), height_(156), frame_number_(frame_number) {

	//dynamically creates rectangle which will store xy coordinates, width and height
	rectangle_ = new Rect;
	clip_rectangle_ = new Rect;

	if(!HAPI->LoadTexture(file_name, &pointer_, &width_, &height_))//load texture
	{
		//Displays an error message if image can not be loaded, asks user if he wishes to continue or not
		HAPI_UserResponse user_response;
		HAPI->UserMessage("Image could not be found, click Ok to terminate program, Cancel to continue", "Error:LoadTexture", eButtonTypeOkCancel, &user_response);

		if(user_response == eUserResponseOk)
			HAPI->Close();
		else
		{
			frame_number_ = 1;

			pointer_ = new BYTE[width_*height_*4];

			//passing rgb values to colour_ 
			colour_.red = 43;
			colour_.green = 98;
			colour_.blue = 65;
			colour_.alpha = 255;

			int offset = 0;

			for(int i = 0; i < width_*height_*4; i+= 4)
				{
					memcpy(pointer_ + offset, &colour_, 4);
					offset += 4;
				}
		}
	}

	//store frame width and height to rectangle
	rectangle_->width = width_ / frame_number_;
	rectangle_->height = height_;

	frame_pointer_ = pointer_;

}

Texture::~Texture(){

	if(pointer_)
	{
		delete pointer_;
	}
	pointer_ = 0;

	if(rectangle_)
	{
		delete rectangle_;
	}
	rectangle_ = 0;

	if(clip_rectangle_)
	{
		delete clip_rectangle_;
	}
	clip_rectangle_ = 0;
}

Rect* Texture::clip(Rect destination) {

	//If possible, find a way to overload + operator for Rect
	clip_rectangle_->x = rectangle_->x;
	clip_rectangle_->y = rectangle_->y;
	clip_rectangle_->width = rectangle_->width;
	clip_rectangle_->height = rectangle_->height;

	//if source is completly contained, don't clip it
	if( contained(destination))
	{
		return clip_rectangle_;
	}
	else
	{
		//converts rectangle into screen space
		clip_rectangle_->width += clip_rectangle_->x;
		clip_rectangle_->height += clip_rectangle_->y;

		//clip rectangle against destination
		if(clip_rectangle_->x < destination.x)
			clip_rectangle_->x = destination.x;

		if(clip_rectangle_->y < destination.y)
			clip_rectangle_->y = destination.y;

		if(clip_rectangle_->width > destination.width)
			clip_rectangle_->width = destination.width;

		if(clip_rectangle_->height > destination.height)
			clip_rectangle_->height = destination.height;

		//converts rectangle back to source space
		clip_rectangle_->width -= clip_rectangle_->x;
		clip_rectangle_->height -= clip_rectangle_->y;

		//clamps negative values to zero
		if (clip_rectangle_->width < 0)
			clip_rectangle_->width = 0;

		if (clip_rectangle_->height < 0)
			clip_rectangle_->height = 0;

		return clip_rectangle_;
	}
}

bool Texture::out_of_bounds(Rect destination)const {
	
	if( rectangle_->x + rectangle_->width < destination.x)
		return true;
	else if( rectangle_->y + rectangle_->height < destination.y)
		return true;
	 else if( rectangle_->x > destination.width)
		return true;
	else if(rectangle_->y > destination.height)
		return true;
	else
		return false;
}

bool Texture::contained(Rect destination)const {

	if( rectangle_->x + rectangle_->width > destination.width)
		return false;
	else if( rectangle_->y + rectangle_->height > destination.height)
		return false;
	 else if( rectangle_->x < destination.x)
		return false;
	else if(rectangle_->y < destination.y)
		return false;
	else
		return true;
}

int Texture::get_frame_number()const{

	return frame_number_;
}

BYTE* Texture::get_pointer()const{

	return frame_pointer_;
}

Rect* Texture::get_rectangle()const{

	return rectangle_;
}

void Texture::set_frame(int state_id){
	
	if(state_id < 0)
	{
		state_id = 0;
	}

	while(state_id >= frame_number_)
	{
		state_id -= frame_number_;
	}


	frame_pointer_ = pointer_ + (state_id * rectangle_->width * 4);
}

void Texture::set_position(Coord2D position){

	rectangle_->x = position.x;
	rectangle_->y = position.y;

}