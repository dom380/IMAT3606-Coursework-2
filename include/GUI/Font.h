#pragma once
#ifndef FONT_H
#define FONT_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <map>
using std::map;
#include <gl\glm\glm\glm.hpp>
#include <Renderers\Graphics.h>
class Graphics;

/*
	Class that represents a Font.  Wraps Freetype Library.
*/
class Font {
public:
	//Constructors
	Font() {};
	/*
		Copy constructor
	*/
	Font(Font* font);
	/*
		Constructor.
		FT_Library ftLib, Instance of the Freetype Library. Assumes it has been successfully initialised.
		const char* fontPath, File path, relative to the 'fontLocation' property, to the TrueType font to load. 
		shared_ptr<Graphics>& graphics, Pointer to the graphics system.
	*/
	Font(FT_Library ftLib, const char* fontPath, shared_ptr<Graphics>& graphics);
	/*
		Constructs and buffers the glyph textures for the first 128 ASCII characters 
		of the the supplied TrueType font. 
		Must be called before font can be used.
	*/
	void compile();
	/*
		Inline class representing an individual character in the font.
	*/
	class Character {
	public:
		Character() {};
		Character(unsigned int textureId, glm::ivec2 sizeVec, glm::ivec2 bearingVec, unsigned int charOffset) {
			texture = textureId;
			size = sizeVec;
			bearing = bearingVec;
			offset = charOffset;
		};
		unsigned int texture;    // ID handle of the glyph texture
		glm::ivec2 size;       // Size of glyph
		glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
		unsigned int offset;     // Offset to advance to next glyph
	};
	Character getChar(char c);
private:
	FT_Library ft;
	FT_Face fontFace;
	map<char, Character> charMap; //Map of char to Character to allow quick access to glyph texture information.
	bool compiled = false;
	shared_ptr<Graphics> graphics;
};

#endif // !FONT_H

