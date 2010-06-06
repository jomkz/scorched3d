////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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
	enum ImageLocation
	{
		eAbsLocation,
		eDataLocation,
		eModLocation
	};

	ImageID();
	ImageID(ImageLocation imageLocation,
		const std::string &imageName,
		const std::string &alphaName = "",
		bool invert = false);
	virtual ~ImageID();

	bool initFromNode(XMLNode *imageNode);

	bool initFromString(
		ImageLocation imageLocation,
		const std::string &imageName,
		const std::string &alphaName = "",
		bool invert = false);

	ImageLocation getImageLocation() { return imageLocation_; }
	const std::string &getStringHash();
	const std::string &getImageName() { return imageName_; }
	const std::string &getAlphaName() { return alphaName_; }
	bool getInvert() { return invert_; }

	static const std::string getLocation(ImageLocation imageLocation, const std::string &filename);

protected:
	ImageLocation imageLocation_;
	std::string imageName_;
	std::string alphaName_;
	std::string hash_;
	bool invert_;
};

#endif // __INCLUDE_ImageIDh_INCLUDE__
