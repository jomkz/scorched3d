////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#include <common/Defines.h>
#include <engine/ModFileEntry.h>
#include <engine/ModFiles.h>
#include <zlib.h>

ModFileEntry::ModFileEntry() : 
	compressedcrc_(0)
{
}

ModFileEntry::~ModFileEntry()
{
}

bool ModFileEntry::writeModFile(const std::string &fileName, 
								const std::string &modName)
{
	// Check that this file is allowed to be sent
	if (ModFiles::excludeFile(fileName)) return true;

	// Check the downloaded CRC matches the actual crc of the file
	unsigned int crc =  crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (unsigned char *) 
		compressedfile_.getBuffer(), compressedfile_.getBufferUsed());
	if (crc != compressedcrc_)
	{
		S3D::dialogMessage("WriteModFile",
			"File crc missmatch error");
		return false;
	}

	// Decompress the actual file contents	
	NetBuffer fileContents;
	if (compressedfile_.getBufferUsed() > 0)
	{
		unsigned long destLen = uncompressedSize_ + 10;
		fileContents.allocate(destLen);
		unsigned uncompressResult = 
			uncompress((unsigned char *) fileContents.getBuffer(), &destLen, 
			(unsigned char *) compressedfile_.getBuffer(), compressedfile_.getBufferUsed());
		fileContents.setBufferUsed(destLen);

		if (uncompressResult == Z_MEM_ERROR) S3D::dialogMessage(
			"WriteModFile", "Memory error");
		else if (uncompressResult == Z_DATA_ERROR) S3D::dialogMessage(
			"WriteModFile", "Data error");
		else if (uncompressResult == Z_BUF_ERROR) S3D::dialogMessage(
			"WriteModFile", "Buffer error");

		bool result = (Z_OK == uncompressResult);
		if (!result) return false;
	}
	else
	{
		fileContents.allocate(1);
		fileContents.setBufferUsed(0);
	}

	// Create any needed directories
	char *dir = (char *) fileName.c_str();
	while (dir = strchr(dir, '/'))
	{
		*dir = '\0';
		std::string needdir = S3D::getSettingsModFile(S3D::formatStringBuffer("%s/%s", 
			modName.c_str(), fileName.c_str()));
		if (!S3D::dirExists(needdir)) S3D::dirMake(needdir);
		*dir = '/';
		dir++;
	}

	// Write the file 
	std::string needfile = S3D::getSettingsModFile(S3D::formatStringBuffer("%s/%s", 
		modName.c_str(), fileName.c_str()));
	FILE *file = fopen(needfile.c_str(), "wb");
	if (!file)
	{
		S3D::dialogMessage("WriteModFile",
			"Create file error");
		return false;
	}
	if (fwrite(fileContents.getBuffer(), sizeof(unsigned char), 
		fileContents.getBufferUsed(), file) != fileContents.getBufferUsed())
	{
		S3D::dialogMessage("WriteModFile",
			"Write file error");
		fclose(file);
		return false;
	}
	fclose(file);

	return true;
}

bool ModFileEntry::removeModFile(const std::string &filename, 
								const std::string &modName)
{
	// Check that this file is allowed to be sent
	if (ModFiles::excludeSpecialFile(filename)) return true;

	std::string needfile = S3D::getSettingsModFile(S3D::formatStringBuffer("%s/%s", 
		modName.c_str(), filename.c_str()));
	S3D::removeFile(needfile);

	return true;
}

bool ModFileEntry::loadModFile(const std::string &filename)
{
	static NetBuffer fileContents;
	fileContents.reset();
	{
		// Do translation on ASCII files to prevent CVS
		// from being a biatch
		bool translate = false;
		if (ModFiles::fileEnding(filename, ".txt") ||
			ModFiles::fileEnding(filename, ".xml"))
		{
			translate = true;
		}

		// Load the file into a coms buffer
		FILE *file = fopen(filename.c_str(), "rb");
		if (!file) return false;
		int newSize = 0;
		unsigned char buffer[256];
		static NetBuffer tmpBuffer;
		tmpBuffer.reset();
		do
		{
			newSize = (int) fread(buffer, 
				sizeof(unsigned char), 
				sizeof(buffer), 
				file);
			tmpBuffer.addDataToBuffer(buffer, newSize);
		}
		while (newSize > 0);
		fclose(file);

		if (!translate)
		{
			fileContents.addDataToBuffer(
				tmpBuffer.getBuffer(),
				tmpBuffer.getBufferUsed());
		}
		else
		{
			for (unsigned i=0; i<tmpBuffer.getBufferUsed(); i++)
			{
				if (i >= tmpBuffer.getBufferUsed() - 1 ||
					tmpBuffer.getBuffer()[i] != 13 ||
					tmpBuffer.getBuffer()[i+1] != 10)
				{	
					fileContents.addDataToBuffer(
						&tmpBuffer.getBuffer()[i] , 1);
				}
			}
		}
	}

	uncompressedSize_ = fileContents.getBufferUsed();
	if (fileContents.getBufferUsed() > 0)
	{
		// Allocate the needed space for the compressed file
		unsigned long destLen = fileContents.getBufferUsed() + 100;
		compressedfile_.allocate(destLen);
		compressedfile_.reset();

		// Compress the file into the new buffer
		if (compress2(
				(unsigned char *) compressedfile_.getBuffer(), &destLen, 
				(unsigned char *) fileContents.getBuffer(), fileContents.getBufferUsed(), 
				6) != Z_OK)
		{
			return false;
		}
		compressedfile_.setBufferUsed(destLen);

		// Get the crc for the new file
		compressedcrc_ =  crc32(0L, Z_NULL, 0);
		compressedcrc_ = crc32(compressedcrc_, 
			(unsigned char *) compressedfile_.getBuffer(), compressedfile_.getBufferUsed());
	}
	else
	{
		compressedfile_.allocate(1);
		compressedfile_.setBufferUsed(0);
	}

	return true;
}
