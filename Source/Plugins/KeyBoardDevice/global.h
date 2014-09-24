//KeyBoardplugin
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

#ifndef KEYBOARD_GLOBAL_H
#define KEYBOARD_GLOBAL_H


/*! The namespace for the global Parallel Port definition */
namespace KeyBoardNameSpace
{
	/*! The enum (CaptureKeyMethod) can store a KeyBoard Capture thread method */
	enum CaptureKeyMethod
	{
		KeyPressed						= 0, //!< 0: The MaskedValueChanged method
		KeyReleased						= 1, //!< 1: The MaskedValueChangedHigh method
		KeyPressedReleased				= 2  //!< 2: The MaskedValueChangedLow method
	};
	Q_DECLARE_FLAGS(CaptureKeyMethods, CaptureKeyMethod)
	Q_DECLARE_OPERATORS_FOR_FLAGS(CaptureKeyMethods)
}

#endif //KEYBOARD_GLOBAL_H
