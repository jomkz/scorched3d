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

#if !defined(__INCLUDE_GLSLShaderh_INCLUDE__)
#define __INCLUDE_GLSLShaderh_INCLUDE__

#include <list>
#include <string>

class GLSLShader
{
public:
	/// a list of strings with shader preprocessor defines
	typedef std::list<std::string> defines_list;

	/// type of shader (vertex or fragment, later maybe geometry shader with GF8800+)
	enum Type 
	{
		VERTEX,
		FRAGMENT
	};

	GLSLShader(const char *filename, Type stype,
		const defines_list& dl = defines_list());
	~GLSLShader();

	unsigned int getId() { return id_; }

protected:
	unsigned int id_;

private:
	GLSLShader(const GLSLShader&);
	GLSLShader& operator= (const GLSLShader&);
};

#endif

