//parallelportplugin
//Copyright (C) 2015  Sven Gijsen
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

#ifndef PARPORT_GLOBAL_H
#define PARPORT_GLOBAL_H


/*! The namespace for the global Parallel Port definition */
namespace ParallelPortNameSpace
{
	/*! The enum (GenerationMethods) can store a Parallel Port Generate thread method */
	enum GenerationMethod
	{
		Value				= 0, //!< 0: The Value method
		Pulse    			= 1, //!< 1: The Pulse method
		Periodical			= 2  //!< 2: The Periodical method
	};
	Q_DECLARE_FLAGS(GenerationMethods, GenerationMethod)
	Q_DECLARE_OPERATORS_FOR_FLAGS(GenerationMethods)

	/*! The enum (CaptureMethods) can store a Parallel Port Capture thread method */
	enum CaptureMethod
	{
		MaskedValueChanged				= 0, //!< 0: The MaskedValueChanged method
		MaskedValueChangedHigh			= 1, //!< 1: The MaskedValueChangedHigh method
		MaskedValueChangedLow			= 2  //!< 2: The MaskedValueChangedLow method
	};
	Q_DECLARE_FLAGS(CaptureMethods, CaptureMethod)
	Q_DECLARE_OPERATORS_FOR_FLAGS(CaptureMethods)
}

#endif //PARPORT_GLOBAL_H
