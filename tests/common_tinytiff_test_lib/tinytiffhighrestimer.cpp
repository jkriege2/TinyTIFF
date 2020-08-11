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



/*
  Name: highrestimer.cpp
  Copyright: (c) 2020
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "tinytiffhighrestimer.h" // class's header file


HighResTimer::HighResTimer() {
  start();
}

HighResTimer::~HighResTimer() {
}

void HighResTimer::start(){
  last=std::chrono::high_resolution_clock::now();
}

double HighResTimer::get_time(){
  const std::chrono::high_resolution_clock::time_point n=std::chrono::high_resolution_clock::now();
  return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(n-last).count())/1e3;
}
