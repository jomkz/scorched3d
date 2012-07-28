/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <ui/RocketRenderInterfaceOpenGL.h>
#include <Rocket/Core.h>
#include <image/ImageFactory.h>

#define GL_CLAMP_TO_EDGE 0x812F

RocketRenderInterfaceOpenGL::RocketRenderInterfaceOpenGL()
{
}

// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void RocketRenderInterfaceOpenGL::RenderGeometry(Rocket::Core::Vertex* vertices, int ROCKET_UNUSED(num_vertices), int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
	glPushMatrix();
	glTranslatef(translation.x, translation.y, 0);

	glVertexPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rocket::Core::Vertex), &vertices[0].colour);

	if (!texture)
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].tex_coord);
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

	glPopMatrix();
}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.		
Rocket::Core::CompiledGeometryHandle RocketRenderInterfaceOpenGL::CompileGeometry(Rocket::Core::Vertex* ROCKET_UNUSED(vertices), int ROCKET_UNUSED(num_vertices), int* ROCKET_UNUSED(indices), int ROCKET_UNUSED(num_indices), const Rocket::Core::TextureHandle ROCKET_UNUSED(texture))
{
	return (Rocket::Core::CompiledGeometryHandle) NULL;
}

// Called by Rocket when it wants to render application-compiled geometry.		
void RocketRenderInterfaceOpenGL::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry), const Rocket::Core::Vector2f& ROCKET_UNUSED(translation))
{
}

// Called by Rocket when it wants to release application-compiled geometry.		
void RocketRenderInterfaceOpenGL::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry))
{
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.		
void RocketRenderInterfaceOpenGL::EnableScissorRegion(bool enable)
{
	if (enable)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

// Called by Rocket when it wants to change the scissor region.		
void RocketRenderInterfaceOpenGL::SetScissorRegion(int x, int y, int width, int height)
{
	glScissor(x, 768 - (y + height), width, height);
}

// Called by Rocket when a texture is required by the library.		
bool RocketRenderInterfaceOpenGL::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
	Image image = ImageFactory::loadAlphaImage(S3D::eAbsLocation, source.CString());
	if (!image.getBits()) return false;

	texture_dimensions.x = image.getWidth();
	texture_dimensions.y = image.getHeight();
	bool success = GenerateTexture(texture_handle, image.getBits(), texture_dimensions);	
	return success;
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool RocketRenderInterfaceOpenGL::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	if (texture_id == 0)
	{
		printf("Failed to generate textures\n");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	texture_handle = (Rocket::Core::TextureHandle) texture_id;

	return true;
}

// Called by Rocket when a loaded texture is no longer required.		
void RocketRenderInterfaceOpenGL::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
	glDeleteTextures(1, (GLuint*) &texture_handle);
}

