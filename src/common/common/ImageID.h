////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_ImageIDh_INCLUDE__)
#define __INCLUDE_ImageIDh_INCLUDE__

#include <string>

class XMLNode;
class ImageID
{
public:
	ImageID();
	virtual ~ImageID();

	bool initFromNode(const char *directory,
		XMLNode *imageNode);

	bool initFromString(
		const char *type,
		const char *imageName,
		const char *alphaName = "",
		bool invert = false);

	// Not very generic but it will do for now!!
	const char *getStringHash();
	const char *getImageName() { return imageName_.c_str(); }
	const char *getAlphaName() { return alphaName_.c_str(); }
	const char *getType() { return type_.c_str(); }
	bool getInvert() { return invert_; }
	bool imageValid() { return !type_.empty(); }

protected:
	std::string type_;
	std::string imageName_;
	std::string alphaName_;
	std::string hash_;
	bool invert_;
};

#endif // __INCLUDE_ImageIDh_INCLUDE__
