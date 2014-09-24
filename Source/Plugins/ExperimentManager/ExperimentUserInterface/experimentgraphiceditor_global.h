//ExperimentManagerplugin
//Copyright (C) 2014  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef EDITTREE_GLOBAL_H
#define EDITTREE_GLOBAL_H

#include "..\global.h"
#include "..\defines.h"

#define DEFAULT_DOCUMENTTYPEDEFINITION	"<!DOCTYPE EXML>"
#define DEFINITION_NAME_VALUE			"Value"
#define DEFINITION_TAG_COMMENT			"<COMMENT>"

enum TreeItemType
{	
	TreeItemType_Attribute	= 0, 
	TreeItemType_Comment	= 1 
};

struct TreeItemDefinition 
{
	TreeItemType type;
	QVariant value;
};

struct TreeFilterSettings
{
	bool bSkipEmptyAttributes;
	bool bSkipComments;
};

#endif //EDITTREE_GLOBAL_H