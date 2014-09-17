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


#ifndef CurrentTime_h
#define CurrentTime_h

#include <time.h>

namespace WTF {

	// Returns the current UTC time in seconds, counted from January 1, 1970.
	// Precision varies depending on platform but is usually as good or better 
	// than a millisecond.
	double currentTime();

	// Same thing, in milliseconds.
	inline double currentTimeMS()
	{
		return currentTime() * 1000.0; 
	}

	inline void getLocalTime(const time_t* localTime, struct tm* localTM)
	{
//#if COMPILER(MSVC7) || COMPILER(MINGW) || OS(WINCE)
//		*localTM = *localtime(localTime);
//#elif COMPILER(MSVC)
		localtime_s(localTM, localTime);
//#else
//		localtime_r(localTime, localTM);
//#endif
	}

} // namespace WTF

using WTF::currentTime;

#endif // CurrentTime_h

