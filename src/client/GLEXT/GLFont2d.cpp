////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <ft2build.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLFont2d.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLTextureBase.h>
#include <common/Defines.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

GLFont2d::GLFont2d() : textures_(0), list_base_(0), characters_(0)
{

}

GLFont2d::~GLFont2d()
{
	glDeleteLists(list_base_,128);
	glDeleteTextures(128, textures_);
	delete [] textures_;
	textures_ = 0;
	delete [] characters_;
	characters_ = 0;
}

bool GLFont2d::getInit()
{
	return (textures_ != 0);
}

float GLFont2d::getWidth(float size, const LangString &text, int len)
{
	float width = 0.0f;
	if (len == 0) len = (int) text.size();

	const unsigned int *a = text.c_str();
	for (int i=0; i<len; i++, a++)
	{
		width += float(characters_[*a].advances) * size / height_;
	}
	return width;
}

int GLFont2d::getChars(float size, const LangString &text, float len)
{
	int l = 0;
	float width = 0.0f;
	for (const unsigned int *a=text.c_str(); *a; a++)
	{
		width += float(characters_[*a].advances) * size / height_;
		if (width < len) l++;
	}
	return l;
}

void GLFont2d::draw(Vector &color, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString((GLsizei) text.size(), color, 1.0f, size, x, y, z, text.c_str(), false);
}

void GLFont2d::drawA(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString((GLsizei) text.size(), color, alpha, size, x, y, z, text.c_str(), false);
}

void GLFont2d::drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
	  float x, float y, float z, 
	  const LangString &text)
{
	drawStringHandler((GLsizei) text.size(), handler, color, alpha, size, x, y, z, text.c_str());
}

void GLFont2d::drawSubStr(int start, int len,
	 Vector &color, float size, 
	 float x, float y, float z, 
	 const LangString &text)
{
	drawSubStrA(start, len, color, 1.0f, size, x, y, z, text);
}

void GLFont2d::drawSubStrA(int start, int len,
	Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	int s = start;
	float width = 0.0f;
	for (const unsigned int *a=text.c_str(); *a; a++)
	{
		if (--s < 0) break;
		width += float(characters_[*a].advances) * size / height_;
	}

	if (len - start > 0)
	{
		drawString(len - start, color, alpha, size, x + width, y, z, text.c_str() + start, false);
	}
}

void GLFont2d::drawWidth(float len, Vector &color, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	int l = getChars(size, text, len);
	drawString(l, color, 1.0f, size, x, y, z, text.c_str(), false);
}

void GLFont2d::drawWidthRhs(float len, Vector &color, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	int slen = text.size();
	if (slen > 0)
	{
		int l = 0;
		float width = 0.0f;
		const unsigned int *a=& (text.c_str())[slen-1];
		for (; a >= text.c_str(); a--, l++)
		{
			width += float(characters_[*a].advances) * size / height_;
			if (width > len) break;
		}
		a++;

		drawString(l, color, 1.0f, size, x, y, z, a, false, 0.0f);
	}
}

void GLFont2d::drawBilboard(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString((GLsizei) text.size(), color, alpha, size, x, y, z, text.c_str(), true);
}

float GLFont2d::getWidth(float size, const std::string &text, int len)
{
	LangStringUtil::replace(langText_, text);
	return getWidth(size, langText_, len);
}

int GLFont2d::getChars(float size, const std::string &text, float len)
{
	LangStringUtil::replace(langText_, text);
	return getChars(size, langText_, len);
}

void GLFont2d::draw(Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	draw(color, size, x, y, z, langText_);
}

void GLFont2d::drawA(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawA(color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawA(handler, color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawWidth(float width, 
	Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawWidth(width, color, size, x, y, z, langText_);
}

void GLFont2d::drawWidthRhs(float width, 
	Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawWidthRhs(width, color, size, x, y, z, langText_);
}

void GLFont2d::drawSubStr(int start, int len,
	Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawSubStr(start, len, color, size, x, y, z, langText_);
}

void GLFont2d::drawSubStrA(int start, int len,
	Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawSubStrA(start, len, color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawBilboard(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replace(langText_, text);
	drawBilboard(color, alpha, size, x, y, z, langText_);
}

static void drawLetter(unsigned int ch, GLuint list_base, GLuint *tex_base, 
	GLFont2d::CharEntry *characters)
{
	Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX();
	Vector &bilY = GLCameraFrustum::instance()->getBilboardVectorY();

	glBindTexture(GL_TEXTURE_2D,tex_base[ch]);
	glPushMatrix();

	Vector trans = bilX * (float) (characters+ch)->left + 
		bilY * (float) (characters+ch)->rows;
	glTranslatef(trans[0], trans[1], trans[2]);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); 
		glVertex3fv(bilY * (characters+ch)->height);

		glTexCoord2f(0.0f, (characters+ch)->y); 
		glVertex3f(0.0f,0.0f,0.0f);

		glTexCoord2f((characters+ch)->x,(characters+ch)->y); 
		glVertex3fv(bilX * (characters+ch)->width);

		glTexCoord2f((characters+ch)->x,0.0f); 
		glVertex3fv(bilX * (characters+ch)->width + bilY * (characters+ch)->height);
	glEnd();
	glPopMatrix();

	bilX *= (float)((characters+ch)->advances);
	glTranslatef(bilX[0], bilX[1], bilX[2]);
}

bool GLFont2d::drawStringHandler(unsigned length, 
	GLFont2dI *handler, 
	Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const unsigned int *string)
{
	if (textures_)
	{
		GLTextureBase::setLastBind(0); // Clear so no texture is cached

		GLState currentState(GLState::BLEND_ON | GLState::TEXTURE_ON);
		Vector4 acolor;
		Vector position(x, y, z);

		glPushMatrix();
			glTranslatef(position[0], position[1], position[2]);
			glScalef(size / height_, size / height_, size / height_);

			int pos = 0;
			for (;*string; string++, pos++)
			{
				unsigned int list = *string;

				acolor[0] = color[0];
				acolor[1] = color[1];
				acolor[2] = color[2];
				acolor[3] = alpha;

				CharEntry &charEntry = *(characters_ + *string);
				handler->drawCharacter(pos, position, charEntry, acolor);

				glColor4fv(acolor);
				glCallList(list + list_base_);

				position[0] += float(charEntry.advances) * size / height_;
			}
		glPopMatrix();

		return false;
	}

	return true;
}

bool GLFont2d::drawString(unsigned length, Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const unsigned int *string,
	bool bilboard,
	float size2)
{
	if (textures_)
	{
		GLTextureBase::setLastBind(0); // Clear so no texture is cached

		GLState currentState(GLState::BLEND_ON | GLState::TEXTURE_ON);
		glColor4f(color[0], color[1], color[2], alpha);

		if (bilboard)
		{
			glPushMatrix();
				glTranslatef(x,y,z);
				glScalef(size / height_, size / height_, size / height_);

				for (const unsigned int *current = string; *current; current++)
				{
					drawLetter(*current, list_base_,textures_,characters_);
				}
			glPopMatrix();
		}
		else if (size2 == 0.0f)
		{
			glPushAttrib(GL_LIST_BIT);
				glListBase(list_base_);

				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(size / height_, size / height_, size / height_);
					glCallLists(length, GL_UNSIGNED_INT, string);
				glPopMatrix();
			glPopAttrib();
		}
		else
		{
			glPushMatrix();
				glTranslatef(x,y,z);
				glScalef(size / height_, size / height_, size / height_);
				float scale = size / height_;
				float outlineScale = size2 / height_;
				for (;*string; string++)
				{
					float advances = (float)((characters_+*string)->advances);
					unsigned int list = *string;
					glCallList(list + list_base_);
					glTranslatef(-advances + advances * outlineScale / scale,0.0f ,0.0f);
				}
			glPopMatrix();
		}

		return true;
	}
	return false;
}

// This Function Gets The First Power Of 2 >= The
// Int That We Pass It.
static inline int next_p2 (int a )
{
	int rval=1;
	// rval<<=1 Is A Prettier Way Of Writing rval*=2; 
	while(rval<a) rval<<=1;
	return rval;
}

static bool make_dlist(FT_Face face, char ch, GLuint list_base, 
	GLuint *tex_base, GLFont2d::CharEntry *characters, bool makeShadow) 
{
	// The First Thing We Do Is Get FreeType To Render Our Character
	// Into A Bitmap.  This Actually Requires A Couple Of FreeType Commands:

	// Load The Glyph For Our Character.
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT )) 
	{ 
		S3D::dialogMessage("GLFont", "FT_Load_Glyph failed"); return false; 
	}

	// Move The Face's Glyph Into A Glyph Object.
	FT_Glyph glyph;
	if(FT_Get_Glyph( face->glyph, &glyph ))
	{ 
		S3D::dialogMessage("GLFont", "FT_Get_Glyph failed"); return false; 
	}

	// Convert The Glyph To A Bitmap.
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	// This Reference Will Make Accessing The Bitmap Easier.
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	// Get the width and height
	int bitmap_width = bitmap.width;
	int bitmap_rows = bitmap.rows;
	unsigned char *bitmap_buffer = bitmap.buffer;

	if (makeShadow)
	{
		int shadow_width = bitmap_width + 4;
		int shadow_rows = bitmap_rows + 4;
		unsigned char *shadow_buffer = new unsigned char[
			shadow_width * shadow_rows];
		memset(shadow_buffer, 0, shadow_width * shadow_rows);
		
		for(int j=2; j<shadow_rows-2;j++) 
		{
			for(int i=2; i< shadow_width-2; i++)
			{
				int bi = i-2;
				int bj = j-2;
				unsigned char bitmapValue = bitmap_buffer[bi + bitmap_width*bj];
				
				for (int b=-2; b<=2; b++)
				{
					for (int a=-2; a<=2; a++)
					{
						int si = i+a;
						int sj = j+b;
						unsigned char &shadowValue = shadow_buffer[si + shadow_width*sj];

						int sv = int(shadowValue) + int(bitmapValue);
						if (sv > 255) sv = 255;
						shadowValue = (unsigned char) sv;
					}
				}
			}
		}

		bitmap_width = shadow_width;
		bitmap_rows = shadow_rows;
		bitmap_buffer = shadow_buffer;
	}

	// Use Our Helper Function To Get The Widths Of
	// The Bitmap Data That We Will Need In Order To Create
	// Our Texture.
	int width = next_p2( bitmap_width );
	int height = next_p2( bitmap_rows );

	// Allocate Memory For The Texture Data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height];

	// Here We Fill In The Data For The Expanded Bitmap.
	// Notice That We Are Using A Two Channel Bitmap (One For
	// Channel Luminosity And One For Alpha), But We Assign
	// Both Luminosity And Alpha To The Value That We
	// Find In The FreeType Bitmap. 
	// We Use The ?: Operator To Say That Value Which We Use
	// Will Be 0 If We Are In The Padding Zone, And Whatever
	// Is The FreeType Bitmap Otherwise.
	for(int j=0; j <height;j++) {
		for(int i=0; i < width; i++){
			expanded_data[2*(i+j*width)]= expanded_data[2*(i+j*width)+1] = 
				(i>=bitmap_width || j>=bitmap_rows) ?
				0 : bitmap_buffer[i + bitmap_width*j];
		}
	}

	if (makeShadow) delete [] bitmap_buffer;

	// Now We Just Setup Some Texture Parameters.
	glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	
	// Here We Actually Create The Texture Itself, Notice
	// That We Are Using GL_LUMINANCE_ALPHA To Indicate That
	// We Are Using 2 Channel Data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	// With The Texture Created, We Don't Need The Expanded Data Anymore.
	delete [] expanded_data;

	// Now We Create The Display List
	glNewList(list_base+ch, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D,tex_base[ch]);
	glPushMatrix();

	// First We Need To Move Over A Little So That
	// The Character Has The Right Amount Of Space
	// Between It And The One Before It.
	glTranslatef((float) bitmap_glyph->left, 0.0f, 0.0f);
	(characters+ch)->left = bitmap_glyph->left;

	// Now We Move Down A Little In The Case That The
	// Bitmap Extends Past The Bottom Of The Line 
	// This Is Only True For Characters Like 'g' Or 'y'.
	glTranslatef(0.0f,(float) bitmap_glyph->top-bitmap_rows,0.0f);
	(characters+ch)->rows = bitmap_glyph->top-bitmap_rows;

	// Now We Need To Account For The Fact That Many Of
	// Our Textures Are Filled With Empty Padding Space.
	// We Figure What Portion Of The Texture Is Used By 
	// The Actual Character And Store That Information In
	// The x And y Variables, Then When We Draw The
	// Quad, We Will Only Reference The Parts Of The Texture
	// That Contains The Character Itself.
	
	float x=(float)bitmap_width / (float)width;
	float y=(float)bitmap_rows / (float)height;
	(characters+ch)->x = x;
	(characters+ch)->y = y;

	// Here We Draw The Texturemapped Quads.
	// The Bitmap That We Got From FreeType Was Not 
	// Oriented Quite Like We Would Like It To Be,
	// But We Link The Texture To The Quad
	// In Such A Way That The Result Will Be Properly Aligned.
	(characters+ch)->width = (float)bitmap_width;
	(characters+ch)->height = (float)bitmap_rows;

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex2f(0.0f,(float)bitmap_rows);
	glTexCoord2f(0.0f,y); glVertex2f(0.0f,0.0f);
	glTexCoord2f(x,y); glVertex2f((float)bitmap_width,0.0f);
	glTexCoord2f(x,0.0f); glVertex2f((float)bitmap_width,(float)bitmap_rows);
	glEnd();
	glPopMatrix();
	glTranslatef((float)(face->glyph->advance.x >> 6) ,0.0f ,0.0f);

	(characters+ch)->advances = (face->glyph->advance.x >> 6);

	// Increment The Raster Position As If We Were A Bitmap Font.
	// (Only Needed If You Want To Calculate Text Length)
	// ImageBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

	// Finish The Display List
	glEndList();
	return true;
}

bool GLFont2d::createFont(const std::string &typeFace, unsigned int h, bool makeShadow)
{
	// Allocate Some Memory To Store The Texture Ids.
	textures_ = new GLuint[128];
	characters_ = new CharEntry[128];
	height_ = float(h);

	// Create And Initilize A FreeType Font Library.
	FT_Library library;
	if (FT_Init_FreeType( &library )) 
	{
		S3D::dialogMessage("GLFont2d", "FT_Init_FreeType failed"); return false;
	}

	// The Object In Which FreeType Holds Information On A Given
	// Font Is Called A "face".
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Face( library, typeFace.c_str(), 0, &face )) 
	{
		S3D::dialogMessage("GLFont2d", S3D::formatStringBuffer(
			"FT_New_Face failed (there is probably a problem with your font file \"%s\")",
			typeFace.c_str())); 
		return false;
	}

	// For Some Twisted Reason, FreeType Measures Font Size
	// In Terms Of 1/64ths Of Pixels.  Thus, To Make A Font
	// h Pixels High, We Need To Request A Size Of h*64.
	// (h << 6 Is Just A Prettier Way Of Writing h*64)
	FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);

	// Here We Ask OpenGL To Allocate Resources For
	// All The Textures And Display Lists Which We
	// Are About To Create.  
	list_base_=glGenLists(128);
	glGenTextures( 128, textures_ );

	// This Is Where We Actually Create Each Of The Fonts Display Lists.
	for(unsigned char i=0;i<128;i++)
		make_dlist(face,i,list_base_,textures_,characters_, makeShadow);

	// We Don't Need The Face Information Now That The Display
	// Lists Have Been Created, So We Free The Assosiated Resources.
	FT_Done_Face(face);

	// Ditto For The Font Library.
	FT_Done_FreeType(library);
	return true;
}
