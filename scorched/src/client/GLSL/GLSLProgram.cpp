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

#include <GLSL/GLSLProgram.h>
#include <GLEXT/GLStateExtension.h>
#include <common/Logger.h>
#include <common/Defines.h>

const GLSLProgram* GLSLProgram::used_program_(0);

GLSLProgram::GLSLProgram() : 
	id_(0), linked_(false)
{
	DIALOG_ASSERT(GLStateExtension::hasShaders());

	id_ = glCreateProgram();

	DIALOG_ASSERT(id_);// ("can't create glsl program");
}

GLSLProgram::~GLSLProgram()
{
	if (used_program_ == this) 
	{
		// rather use some kind of "bug!"-exception here
		Logger::log("warning: deleting bound glsl program!");
		use_fixed();
	}
	// if shaders are still attached, it is rather a bug...
	std::list<GLSLShader*>::iterator it;
	for (it = attached_shaders_.begin(); 
		it != attached_shaders_.end(); 
		it++)
	{
		glDetachShader(id_, (*it)->getId());
	}

	glDeleteProgram(id_);
}

void GLSLProgram::attach(GLSLShader &s)
{
	glAttachShader(id_, s.getId());
	attached_shaders_.push_front(&s);
	linked_ = false;
}

void GLSLProgram::detach(GLSLShader &s)
{
	glDetachShader(id_, s.getId());

	std::list<GLSLShader*>::iterator it;
	for (it = attached_shaders_.begin(); 
		it != attached_shaders_.end(); )
	{
		if (*it == &s) 
		{
			glDetachShader(id_, (*it)->getId());
			it = attached_shaders_.erase(it);
		} else 
		{
			it++;
		}
	}
	linked_ = false;
}

void GLSLProgram::link()
{
	glLinkProgram(id_);
	GLint waslinked = GL_FALSE;
	glGetProgramiv(id_, GL_LINK_STATUS, &waslinked);

	if (!waslinked) 
	{
		GLint maxlength = 0;
		glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &maxlength);
		std::string log(maxlength+1, ' ');
		GLsizei length = 0;
		glGetProgramInfoLog(id_, maxlength, &length, &log[0]);
		S3D::dialogExit("GLSLProgram", 
			S3D::formatStringBuffer("linking of program failed : %s", log.c_str()));
	}

	linked_ = true;
}

void GLSLProgram::use() const
{
	DIALOG_ASSERT(linked_);
	glUseProgram(id_);
	used_program_ = this;
}

void GLSLProgram::set_gl_texture(GLTexture &tex, const char *texname, unsigned texunit) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint uniloc = glGetUniformLocation(id_, texname);
	glActiveTexture(GL_TEXTURE0 + texunit);
	tex.draw(true);
	glUniform1i(uniloc, texunit);
}

void GLSLProgram::set_gl_texture(GLShadowFrameBuffer &tex, const char *texname, unsigned texunit) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint uniloc = glGetUniformLocation(id_, texname);
	glActiveTexture(GL_TEXTURE0 + texunit);
	tex.bindDepthTexture();
	glUniform1i(uniloc, texunit);
}

void GLSLProgram::set_gl_texture_unit(const char *texname, unsigned texunit) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint uniloc = glGetUniformLocation(id_, texname);
	glActiveTexture(GL_TEXTURE0 + texunit);
	glUniform1i(uniloc, texunit);
}

void GLSLProgram::set_uniform(const char *name, const Vector& value) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint loc = glGetUniformLocation(id_, name);
	glUniform3f(loc, value[0], value[1], value[2]);
}

void GLSLProgram::set_uniform(const char *name, const float value) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint loc = glGetUniformLocation(id_, name);
	glUniform1f(loc, value);
}

unsigned GLSLProgram::get_vertex_attrib_index(const char *name) const
{
	DIALOG_ASSERT(used_program_ == this);

	return glGetAttribLocation(id_, name);
}

void GLSLProgram::use_fixed()
{
	glUseProgram(0);
	used_program_ = 0;
}
