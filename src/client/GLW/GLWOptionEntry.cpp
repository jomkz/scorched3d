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

#include <GLW/GLWOptionEntry.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWCheckBox.h>
#include <GLW/GLWDropDownText.h>
#include <GLW/GLWTextBox.h>
#include <engine/ModDirs.h>
#include <common/Defines.h>
#include <common/ToolTip.h>

GLWOptionEntry::GLWOptionEntry(GLWidget *control, OptionEntry *entry) :
	control_(control), entry_(entry)
{
}

GLWOptionEntry::~GLWOptionEntry()
{
}

void GLWOptionEntry::createEntry(
	std::list<GLWOptionEntry> &controls, 
	GLWPanel *parent, OptionEntry &entry)
{
	GLWLabel *staticText = new GLWLabel(0.0f, 0.0f, LANG_RESOURCE(entry.getName()));
	staticText->setToolTip(new ToolTip(ToolTip::ToolTipHelp, entry.getName(), entry.getDescription()));
	parent->addWidget(staticText, 0, GLWPanel::AlignRight | 
		GLWPanel::SpaceLeft | GLWPanel::SpaceTop, 10.0f);

	GLWidget *control = 0;
	if (0 == strcmp(entry.getName(), "Mod"))
	{
		control = new GLWDropDownText(0.0f, 0.0f, 170.0f);
		ModDirs modDirs;
		modDirs.loadModDirs();
		std::list<ModInfo>::iterator itor;
		for (itor = modDirs.getDirs().begin();
			itor != modDirs.getDirs().end();
			itor++)
		{
			ModInfo &info = (*itor);
			((GLWDropDownText *) control)->addText(info.getName());
		}
	}
	else
	switch (entry.getEntryType())
	{
	case OptionEntry::OptionEntryStringType:
		{
			control = new GLWTextBox(0.0f, 0.0f, 170.0f);
		}
		break;
	case OptionEntry::OptionEntryBoundedIntType:
		{
			control = new GLWDropDownText(0.0f, 0.0f, 170.0f);
			OptionEntryBoundedInt &boundedInt = (OptionEntryBoundedInt &) entry;
			for (int i=boundedInt.getMinValue(); 
				i<=boundedInt.getMaxValue(); 
				i+=boundedInt.getStepValue())
			{
				((GLWDropDownText *) control)->addText(S3D::formatStringBuffer("%i", i));
			}
		}
		break;
	case OptionEntry::OptionEntryEnumType:
		{
			control = new GLWDropDownText(0.0f, 0.0f, 170.0f);

			OptionEntryEnum &optionEntryEnum = (OptionEntryEnum &) entry;
			OptionEntryEnum::EnumEntry *enums = optionEntryEnum.getEnums();
			for (OptionEntryEnum::EnumEntry *current = enums; current->description[0]; current++)
			{
				((GLWDropDownText *) control)->addEntry(
					GLWSelectorEntry(current->description, 0, false, 0, (void *) current->value));
			}
		}
		break;
	case OptionEntry::OptionEntryStringEnumType:
		{
			control = new GLWDropDownText(0.0f, 0.0f, 170.0f);

			OptionEntryStringEnum &optionEntryStringEnum = (OptionEntryStringEnum &) entry;
			OptionEntryStringEnum::EnumEntry *enums = optionEntryStringEnum.getEnums();
			for (OptionEntryStringEnum::EnumEntry *current = enums; current->value[0]; current++)
			{
				((GLWDropDownText *) control)->addText(current->value);
			}
		}
		break;
	case OptionEntry::OptionEntryBoolType:
		{
			control = new GLWCheckBox();
		}
		break;
	default:
		S3D::dialogExit("GLWOptionEntry::createEntry",
			S3D::formatStringBuffer("Unhandled OptionEntry type %s:%i",
			entry.getName(), entry.getEntryType()));
	}

	control->setToolTip(new ToolTip(ToolTip::ToolTipHelp, entry.getName(), entry.getDescription()));
	parent->addWidget(control, 0, 
		GLWPanel::SpaceRight | GLWPanel::SpaceLeft | GLWPanel::SpaceTop, 10.0f);

	controls.push_back(GLWOptionEntry(control, &entry));
}

void GLWOptionEntry::updateControls(
	std::list<GLWOptionEntry> &controls)
{
	std::list<GLWOptionEntry>::iterator itor;
	for (itor = controls.begin();
		itor != controls.end();
		itor++)
	{
		GLWOptionEntry &entrySetter = *itor;

		if (0 == strcmp(entrySetter.getEntry()->getName(), "Mod"))
		{
			GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
			control->setCurrentText(entrySetter.getEntry()->getValueAsString());			
		}
		else
		switch (entrySetter.getEntry()->getEntryType())
		{
		case OptionEntry::OptionEntryStringType:
			{
				GLWTextBox *control = (GLWTextBox *) entrySetter.getControl();
				control->setText(entrySetter.getEntry()->getValueAsString());
			}
			break;
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryStringEnumType:
			{
				GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
				control->setCurrentText(entrySetter.getEntry()->getValueAsString());
			}
			break;
		case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool *boolEntry = (OptionEntryBool *) entrySetter.getEntry();
				GLWCheckBox *control = (GLWCheckBox *) entrySetter.getControl();
				control->setState(boolEntry->getValue());
			}
			break;
		}
	}
}

void GLWOptionEntry::updateEntries(
	std::list<GLWOptionEntry> &controls)
{
	std::list<GLWOptionEntry>::iterator itor;
	for (itor = controls.begin();
		itor != controls.end();
		itor++)
	{
		GLWOptionEntry &entrySetter = *itor;

		if (0 == strcmp(entrySetter.getEntry()->getName(), "Mod"))
		{
			GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
			entrySetter.getEntry()->setValueFromString(control->getCurrentText());		
		}
		else
		switch (entrySetter.getEntry()->getEntryType())
		{
		case OptionEntry::OptionEntryStringType:
			{
				GLWTextBox *control = (GLWTextBox *) entrySetter.getControl();
				entrySetter.getEntry()->setValueFromString(control->getText().c_str());	
			}
			break;
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryStringEnumType:
			{
				GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
				entrySetter.getEntry()->setValueFromString(control->getCurrentText());
			}
			break;
		case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool *boolEntry = (OptionEntryBool *) entrySetter.getEntry();
				GLWCheckBox *control = (GLWCheckBox *) entrySetter.getControl();
				boolEntry->setValue(control->getState());
			}
			break;
		}
	}
}
