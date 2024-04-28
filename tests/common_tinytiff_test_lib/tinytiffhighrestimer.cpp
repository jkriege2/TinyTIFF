/*
    Copyright (c) 2008-2024 Jan W. Krieger (<jan@jkrieger.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>

#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#if defined(__WINDOWS__)
#include<windows.h>
#endif


struct HighResTimer::D {
#ifdef __WINDOWS__
    /** \brief internal: time stamp of the last call of start() */
    LARGE_INTEGER last;

    /** \brief internal: timer frequency */
    double freq;
#else
    std::chrono::high_resolution_clock::time_point last;
#endif

};

HighResTimer::HighResTimer():
    d(new D)
{
  start();
}

HighResTimer::~HighResTimer() {
}

void HighResTimer::start(){
#ifdef __WINDOWS__
    LARGE_INTEGER fr;
    QueryPerformanceFrequency(&fr);
    d->freq=(double)(fr.QuadPart);
    QueryPerformanceCounter(&(d->last));
#else
    d->last=std::chrono::high_resolution_clock::now();
#endif
}

double HighResTimer::get_time(){
#ifdef __WINDOWS__
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return ((double)(now.QuadPart-d->last.QuadPart)/d->freq)*1e6;
#else
    const std::chrono::high_resolution_clock::time_point n=std::chrono::high_resolution_clock::now();
    return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(n-d->last).count())/1e3;
#endif
}
