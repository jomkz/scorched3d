////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <graph/LandscapeBlender.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/LandscapeMaps.h>
#include <uistate/UIStatePlayingLand.h>
#include <scorched3dc/OgreSystem.h>
#include <common/simplexnoise.h>

LandscapeBlender::LandscapeBlender(ScorchedClient &scorchedClient, size_t blendMapWidth, size_t blendMapHeight)
{
	this->blendMapWidth = blendMapWidth;
	this->blendMapHeight = blendMapHeight;

	unsigned int seed = scorchedClient.getLandscapeMaps().getDescriptions().getSeed();
	LandscapeTex *tex = scorchedClient.getLandscapeMaps().getDescriptions().getTex();
	LandscapeTexLayers *texLayers = (LandscapeTexLayers*)tex->texture.getValue();

	/*
	// Try enabling a color map for color variation
	Ogre::Image colourMapImage;
	colourMapImage.load("colormap.jpg", terrain->getResourceGroup());
	if (tx % 2 == 0) colourMapImage.flipAroundY();
	if (ty % 2 == 0) colourMapImage.flipAroundX();
	terrain->setGlobalColourMapEnabled(true, colourMapImage.getWidth());
	Ogre::TexturePtr colourMap  = terrain->getGlobalColourMap();
	colourMap->loadImage(colourMapImage);
	*/

	// Constants loaded from file
	numberSources = 3;
	maxHeight = texLayers->layersheight.getValue().asFloat(); // Last texture starts at height 
	blendHeightFactor = new float[numberSources];
	blendHeightFactor[0] = texLayers->texturelayer1.blendheightfactor.getValue().asFloat(); // Ends blend when % into height band
	blendHeightFactor[1] = texLayers->texturelayer2.blendheightfactor.getValue().asFloat(); // Ends blend when % into height band
	blendHeightFactor[2] = texLayers->texturelayer3.blendheightfactor.getValue().asFloat(); // Ends blend when % into height band
	blendNormalSlopeStart = texLayers->texturelayerslope.blendslopestart.getValue().asFloat(); // Starts blending slope at %
	blendNormalSlopeLength = texLayers->texturelayerslope.blendslopefactor.getValue().asFloat(); // Blends when % more slope
	maxOffsetHeight = (maxHeight / numberSources) / 3.0f;
	blendDetailHeightStartFactor = texLayers->texturelayerdetail.blendheightstartfactor.getValue().asFloat();
	blendDetailHeightEndFactor = texLayers->texturelayerdetail.blendheightendfactor.getValue().asFloat();
	blendDetailHeightStartFactorFactor = texLayers->texturelayerdetail.blendheightstartfactor.getValue().asFloat() +
		texLayers->texturelayerdetail.blendheightfactor.getValue().asFloat();
	blendDetailHeightEndFactorFactor = texLayers->texturelayerdetail.blendheightendfactor.getValue().asFloat() +
		texLayers->texturelayerdetail.blendheightfactor.getValue().asFloat();
	blendDetailAmount = 1.0f / texLayers->texturelayerdetail.blendheightfactor.getValue().asFloat();
	blendDetailNoiseOctaves = texLayers->texturelayerdetail.blendnoiseoctaves.getValue();
	blendDetailNoisePersistence = texLayers->texturelayerdetail.blendnoisepersistence.getValue().asFloat();
	blendDetailNoiseScale = texLayers->texturelayerdetail.blendnoisescale.getValue().asFloat();
	layerOffsetNoiseOctaves = texLayers->layeroffsetoctaves.getValue();
	layerOffsetNoisePersistence = texLayers->layeroffsetpersistence.getValue().asFloat();
	layerOffsetNoiseScale = texLayers->layeroffsetscale.getValue().asFloat();
}

LandscapeBlender::~LandscapeBlender()
{
	delete [] blendHeightFactor;
}

void LandscapeBlender::subCalculate(bool useNoise)
{
	for (Ogre::uint16 y = 0; y < blendMapHeight; ++y)
	{
		for (Ogre::uint16 x = 0; x < blendMapWidth; ++x)
		{
			// Figure out the height map coords
			Ogre::Real tsx, tsy;
			float hxf, hyf;
			int ix, iy;
			convertImageToTerrainAndWorldAndImageSpace(x, y, &tsx, &tsy, &hxf, &hyf, &ix, &iy);

			// Get height and normal information
			float normal, height;
			getHeightNormal(tsx, tsy, hxf, hyf, ix, iy, &height, &normal);

			// This stops the height bands from being too uniform
			float offSetHeight = 0.0f;
			if (useNoise)
			{
				offSetHeight = octave_noise_2d(layerOffsetNoiseOctaves, layerOffsetNoisePersistence, layerOffsetNoiseScale, -hxf, -hyf)
					* maxOffsetHeight;
				height -= offSetHeight;
			}
			
			// Find the index of the current texture by deviding the height into strips
			float heightPer = (height / maxHeight) * (float) numberSources;
			int heightIndex = (int)heightPer;
			if (heightIndex >= numberSources)
			{
				heightIndex = numberSources - 1;
			}

			// Check if we are in a blending transition phase
			float blendFirstAmount = 1.0f;
			float blendSecondAmount = 0.0f;
			if (heightIndex < numberSources - 1)
			{
				float remainderIndex = heightPer - heightIndex;
				if (remainderIndex > blendHeightFactor[heightIndex])
				{
					// We need to do some blending, figure how much
					remainderIndex -= blendHeightFactor[heightIndex];
					blendSecondAmount = remainderIndex / (1.0f - blendHeightFactor[heightIndex]);
					blendFirstAmount = 1.0f - blendSecondAmount;
				}
			}

			// Check to see if we need to blend in the side texture
			float blendSideAmount = 0.0f;
			if (normal < blendNormalSlopeStart)
			{
				if (normal < blendNormalSlopeStart - blendNormalSlopeLength)
				{
					// Only use the side texture
					blendSideAmount = 1.0f;
					blendFirstAmount = 0.0f;
					blendSecondAmount = 0.0f;
				}
				else
				{
					// Blend in the side texture
					float remainderIndex = normal - (blendNormalSlopeStart - blendNormalSlopeLength);
					remainderIndex /= blendNormalSlopeLength;

					blendSideAmount = (1.0f - remainderIndex);
					blendFirstAmount *= remainderIndex;
					blendSecondAmount *= remainderIndex;
				}
			}

			// Add in the base value
			// Generate a noise map for the base map
			// This is like the detail map that adds texture variation across the terrain
			// Don't add it right at the lowest height, or at the max height
			if (useNoise && heightPer > blendDetailHeightStartFactor && heightPer < float(numberSources) - blendDetailHeightEndFactor)
			{
				float baseValue = octave_noise_2d(blendDetailNoiseOctaves, blendDetailNoisePersistence, blendDetailNoiseScale, hxf, hyf);
				if (baseValue > 0.0f)
				{
					if (heightPer < blendDetailHeightStartFactorFactor) baseValue *= (heightPer - blendDetailHeightStartFactor) * blendDetailAmount;
					else if (heightPer > float(numberSources) - blendDetailHeightEndFactorFactor) baseValue *= 1.0f -
						((heightPer - (float(numberSources) - blendDetailHeightEndFactorFactor)) * blendDetailAmount);
					blendFirstAmount *= Ogre::Math::Clamp(1.0f - baseValue, (Ogre::Real) 0, (Ogre::Real) 1);
					blendSecondAmount *= Ogre::Math::Clamp(1.0f - baseValue, (Ogre::Real) 0, (Ogre::Real) 1);
				}
			}

			// The texture color at this position
			float colorAmountFromLayers = 0.0f;
			Ogre::ColourValue textureColorValue(0.0f, 0.0f, 0.0f, 1.0f);

			// Update maps
			for (int i = 0; i<numberSources; i++)
			{
				// Update the blend map
				float blendAmount = 0.0f;
				if (i == heightIndex) blendAmount = blendFirstAmount;
				else if (i == heightIndex + 1) blendAmount = blendSecondAmount;

				blendAmount -= blendSideAmount;
				if (blendAmount < 0.0f) blendAmount = 0.0f;
				setLandscapeBlend(i, hxf, hyf, ix, iy, blendAmount, textureColorValue);
				colorAmountFromLayers += blendAmount;
			}

			// Add in the slope texture, the one when the gradient is larger
			setStoneBlend(hxf, hyf, ix, iy, blendSideAmount, textureColorValue);
			colorAmountFromLayers += blendSideAmount;

			// Add in the default texture, the detail texture
			if (colorAmountFromLayers < 1.0f)
			{
				float blendAmount = 1.0f - colorAmountFromLayers;
				setDefaultBlend(hxf, hyf, ix, iy, blendAmount, textureColorValue);
			}

			// Set the final texture color
			setFullColorBlend(ix, iy, textureColorValue);
		}
	}
}
