#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "utils.h"
#include <string>
#include <vector>

#define GRAPHICS ScreenBuffer::instance()


//informs ScreenBuffer of Class Texture
class Texture;

//
// Structures
//

struct Sprite{

	Texture* texture;
	std::string name;
	Sprite(std::string n, Texture* t) : name(n), texture(t) {}
};

//
// Singleton Class ScreenBuffer
//

class ScreenBuffer {
	public:

		static ScreenBuffer* instance();

		//init - initializes the values needed for screenBuffer class
		void init(int screen_width, int screen_height);

		//destroy - deletes the memory used in screenBuffer class
		void destroy();

		//clear_screen - clears the screen to an arbitrary colour
		void clear_screen(BYTE r, BYTE g, BYTE b);

		//set_pixel - sets a pixel on the screen to an arbitrary colour	
		void set_pixel(Coord2D position, BYTE r, BYTE g, BYTE b);

		//create_sprite - creates a sprite and stores it
		int create_sprite(const std::string& file_name, int frame_number = 1);

		//get_sprite_rect - returns the rectangle of a sprite
		Rect get_sprite_rect(uint id)const;

		//draw_sprite - draws a sprite into the screen at an arbitrary location 
		void draw_sprite(uint id, Coord2D position, int frame_id = 0);

		//get_screen - returns screen_
		Rect get_screen()const;

	private:
		
		//constructors and private functions

		ScreenBuffer();

		ScreenBuffer(ScreenBuffer& screen_buffer);

		//blit - draws a texture to an arbitrary destination which does not have transparency
		void blit(Texture* source, BYTE *destination, Rect r_destination);

		//blit_alpha - draws a texture to an arbitrary destination with transparency
		void blit_alpha(Texture* source, BYTE *destination, Rect r_destination);


		//Data members

		static ScreenBuffer* instance_;

		bool init_;

		unsigned int offset_;

		BYTE* screen_pointer_;
		BYTE* dest_pointer_;
		BYTE* source_pointer_;

		Rect screen_;

		HAPI_TColour colour_;

		std::vector <Sprite> sprite_vector_;

};

//
// Class Texture
//

class Texture {
	public:

		Texture(const std::string& file_name, int frame_number);

		~Texture();

		//clip - returns the rectangle of the texture that is contained by the destination/clips the texture to its destination
		Rect* clip(Rect destination);

		// out_of_bounds - returns true if the texture isn't contained by its destination, false otherwise
		bool out_of_bounds(Rect destination)const;

		// contained - returns true if the texture is completly contained by its destination, false otherwise
		bool contained(Rect destination)const;

		//get_frame - returns the number of frames
		int get_frame_number()const;

		//get_texture - returns the pointer to the texture
		BYTE* get_pointer()const;

		//get_rectangle - returns rectangle_ which has width, height and xy coordinates
		Rect* get_rectangle()const;

		//set_frame - sets the frame of a sprite 
		void set_frame(int state_id);

		//set_position - sets the xy coordinates of rectangle_
		void set_position(Coord2D position);

	private:

		int width_;

		int height_;

		int frame_number_;

		Rect* rectangle_;

		Rect* clip_rectangle_;

		BYTE* pointer_;

		BYTE* frame_pointer_;

		HAPI_TColour colour_;
		
};

#endif//GRAPHICS_H_
