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

#if !defined(__INCLUDE_GLStateExtensionh_INCLUDE__)
#define __INCLUDE_GLStateExtensionh_INCLUDE__

#include <GLEXT/GLState.h>

class GLStateExtension
{
public:
	static void setup(); // Setup and check for each extension

	// Use VBO
	static bool hasVBO() { return hasVBO_; }
	static int getMaxElementVertices() { return maxElementVertices_; }
	static int getMaxElementIndices() { return maxElementIndices_; }
	// Use MultiTex
	static bool hasMultiTex() { return hasMultiTex_; }
	// Use framebuffers
	static bool hasFBO() { return hasFBO_; }
	// User drawrangeelements
	static bool hasDrawRangeElements() { return hasDrawRangeElements_; }
	// Use tex sub image extension?
	static bool getNoTexSubImage() { return noTexSubImage_; }
	// Use cube map extension?
	static bool hasCubeMap() { return hasCubeMap_; }
	// Use sphere map extension?
	static bool hasSphereMap() { return hasSphereMap_; }
	// Use hardware mipmap extension?
	static bool hasHardwareMipmaps() { return hasHardwareMipmaps_; }
	// Use env combine
	static bool hasEnvCombine() { return envCombine_; }
	// How many texture units
	static int getTextureUnits() { return textureUnits_; }
	// Use FrameBuffer extension
	static bool hasHardwareShadows() { return hasHardwareShadows_; }
	// Use glBlendColorEXT
	static bool hasBlendColor() { return hasBlendColor_; }
	// Use shaders
	static bool hasShaders() { return hasShaders_; }

protected:
	static bool envCombine_;
	static bool multiTexDisabled_;
	static bool hasVBO_;
	static bool hasFBO_;
	static bool hasShaders_;
	static int textureUnits_;
	static int maxElementVertices_;
	static int maxElementIndices_;
	static bool hasCubeMap_;
	static bool hasSphereMap_;
	static bool hasHardwareMipmaps_;
	static bool noTexSubImage_;
	static bool hasHardwareShadows_;
	static bool hasBlendColor_;
	static bool hasMultiTex_;
	static bool hasDrawRangeElements_;

private:
	GLStateExtension();
	~GLStateExtension();
};


#endif
