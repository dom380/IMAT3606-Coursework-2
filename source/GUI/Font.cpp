#include "..\..\include\GUI\Font.h"
#include <utils\GLSupport.h>

Font::Font(Font * font)
{
	ft = font->ft;
	fontFace = font->fontFace;
	graphics = font->graphics;
}

Font::Font(FT_Library ftLib, const char * passedFontPath, shared_ptr<Graphics>& graphics)
{
	fontPath = passedFontPath;
	ft = ftLib;
	FT_Error error = FT_New_Face(ftLib, passedFontPath, 0, &fontFace);
	FT_Set_Pixel_Sizes(fontFace, 0, 48);
	this->graphics = graphics;
}

void Font::compile()
{
	if (compiled) return;
	
	Character character;
	for (unsigned char charByte = 0; charByte < 128; ++charByte) {
		FT_Error error = FT_Load_Char(fontFace, charByte, FT_LOAD_RENDER);
		// Load character glyph 
		unsigned int texture = 0;
		graphics->buildFontTexture(fontFace, texture);
		//store character in map for use
		character = Character(
			texture,
			glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
			glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
			fontFace->glyph->advance.x
		);
	
		charMap.insert(std::pair<char,Character>(charByte, character));
	}
	//Free resources
	FT_Done_Face(fontFace);
	FT_Done_FreeType(ft);
	compiled = true;
}

Font::Character Font::getChar(char c)
{
	return charMap[c];
}

string Font::getFontPath()
{
	return fontPath;
}
