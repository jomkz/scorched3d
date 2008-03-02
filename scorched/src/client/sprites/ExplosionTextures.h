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

#if !defined(AFX_EXPLOSIONTEXTURES_H__F2BC42E7_B2FB_4C0D_ACF0_0B91D8DC84D3__INCLUDED_)
#define AFX_EXPLOSIONTEXTURES_H__F2BC42E7_B2FB_4C0D_ACF0_0B91D8DC84D3__INCLUDED_

#include <GLEXT/GLTextureSet.h>
#include <image/ImageBitmap.h>
#include <common/ProgressCounter.h>
#include <string>
#include <map>

class ExplosionTextures  
{
public:
	static ExplosionTextures *instance();
	bool createTextures(ProgressCounter *counter = 0);

	GLTexture smokeTexture;
	GLTexture smokeTexture2;
	GLTexture particleTexture;
	GLTexture talkTexture;
	GLTexture rainTexture;
	GLTexture snowTexture;

	Image &getScorchBitmap(const char *name);

	GLTextureSet *getTextureSetByName(const char *name);
	std::map<std::string, GLTextureSet*> textureSets;
	std::map<std::string, Image*> scorchedBitmaps;

protected:
	static ExplosionTextures *instance_;

	bool addTextureToSet(GLTextureSet &set,
						const char *texPath);
private:
	ExplosionTextures();
	virtual ~ExplosionTextures();

};

#endif // !defined(AFX_EXPLOSIONTEXTURES_H__F2BC42E7_B2FB_4C0D_ACF0_0B91D8DC84D3__INCLUDED_)
