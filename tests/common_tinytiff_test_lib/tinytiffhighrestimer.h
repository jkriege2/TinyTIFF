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



#ifndef TINYTIFHIGHRESTIMER_H
#define TINYTIFHIGHRESTIMER_H

#include <memory>


class HighResTimer {
  protected:

    public:
        /** \brief class constructor. */
        HighResTimer();
        /** \brief class destructor */
        ~HighResTimer();
        /** \brief start the timer */
        void start();
        /** \brief get the time since the last call of start() in microseconds */
        double get_time();
    private:
        struct D;
        std::unique_ptr<D> d;

};





#endif // TINYTIFHIGHRESTIMER_H

