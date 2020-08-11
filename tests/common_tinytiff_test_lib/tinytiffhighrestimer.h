/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-07-07 12:07:58 +0200 (Di, 07 Jul 2015) $  (revision $Rev: 4005 $)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/



#ifndef TINYTIFHIGHRESTIMER_H
#define TINYTIFHIGHRESTIMER_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <chrono>


class HighResTimer {
  protected:
      std::chrono::high_resolution_clock::time_point last;
	public:
		/** \brief class constructor. */
		HighResTimer();
		/** \brief class destructor */
		~HighResTimer();
		/** \brief start the timer */
		void start();
		/** \brief get the time since the last call of start() in microseconds */
		double get_time();


};





#endif // TINYTIFHIGHRESTIMER_H

