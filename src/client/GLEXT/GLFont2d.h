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

#ifndef _GLFONT2D_H_
#define _GLFONT2D_H_

#include <GLEXT/GLState.h>
#include <common/Vector.h>
#include <common/Vector4.h>

class GLFont2dI;
class GLFont2d  
{
public:
	struct CharEntry 
	{
		float x, y;
		float width, height;
		int advances;
		int left, rows;
	};

	GLFont2d();
	virtual ~GLFont2d();

	bool createFont(const std::string &typeFace, unsigned int h, bool makeShadow = false);
	bool getInit();

	void draw(Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawA(Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawWidth(float width, 
		Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawWidthRhs(float width, 
		Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawSubStr(int start, int len,
		Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawSubStrA(int start, int len,
		Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawOutline(Vector &color, float size, float size2,
		float x, float y, float z,
		const std::string &text);
	void drawOutlineA(Vector &color, float alpha, float size, float size2,
		float x, float y, float z,
		const std::string &text);
	void drawOutlineWidthRhs(float len, Vector &color, float size, float size2,
		float x, float y, float z, 
		const std::string &text);
	void drawBilboard(Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);

	float getWidth(float size, const std::string &text, int len = 0);
	int getChars(float size, const std::string &text, float width);

protected:
	GLuint *textures_;
	GLuint list_base_;
	float height_;
	CharEntry *characters_;

	virtual bool drawString(unsigned len,
		Vector &color, float alpha, 
		float size, 
		float x, float y, float z, 
		const char *string,
		bool bilboard,
		float size2 = 0.0f);
	virtual bool drawStringHandler(unsigned length, 
		GLFont2dI *handler, 
		Vector &color, float alpha, 
		float size, 
		float x, float y, float z, 
		const char *string);
};

class GLFont2dI
{
public:
	virtual void drawCharacter(
		int charPosition, Vector &position, GLFont2d::CharEntry &charEntry, Vector4 &color) = 0;
};

#endif /* _GLFONT2D_H_ */
