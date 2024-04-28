/*
    Copyright (c) 2008-2024 Jan W. Krieger (<jan@jkrieger.de>)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TESTIMAGE_TOOLS_H
#define TESTIMAGE_TOOLS_H

#include <limits>
#include <string>
#include <cmath>
#include <sys/stat.h>

template <class T>
std::string format(const std::string& templ, const T& val){
    char buffer[4096];
    sprintf(buffer, templ.c_str(), val);
    std::string ret(buffer);
    return ret;
}

long get_filesize(const char *FileName);

std::string bytestostr(double bytes);



template <class T>
struct atleast_int {
    typedef T TPrint;
};

template <>
struct atleast_int<char> {
    typedef int TPrint;
};

template <>
struct atleast_int<unsigned char> {
    typedef unsigned int TPrint;
};


// write a test pattern into image
template<class TIMAGESAMPLETYPE>
void write1ChannelTestData(TIMAGESAMPLETYPE* image,size_t WIDTH, size_t HEIGHT, size_t PATTERNSIZE, size_t CHANNELSINDATA=1) {
    size_t i=0;
    for (size_t y=0; y<HEIGHT; y++) {
        for (size_t x=0; x<WIDTH; x++) {
            image[i*CHANNELSINDATA]=0;
            if (x==y) image[i*CHANNELSINDATA]=std::numeric_limits<TIMAGESAMPLETYPE>::max();
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) image[i*CHANNELSINDATA]=std::numeric_limits<TIMAGESAMPLETYPE>::max()-std::numeric_limits<TIMAGESAMPLETYPE>::max()/5;
            else if (x%PATTERNSIZE==0) image[i*CHANNELSINDATA]=std::numeric_limits<TIMAGESAMPLETYPE>::max()/4;
            else if (y%PATTERNSIZE==0) image[i*CHANNELSINDATA]=std::numeric_limits<TIMAGESAMPLETYPE>::max()/2;
            i++;
        }
    }
}

// write a test pattern into image
template<class TIMAGESAMPLETYPE>
void write1ChannelFloatTestData(TIMAGESAMPLETYPE* image,size_t WIDTH, size_t HEIGHT, size_t PATTERNSIZE, size_t CHANNELSINDATA=1, TIMAGESAMPLETYPE MAXVAL=1.0) {
    size_t i=0;
    for (size_t y=0; y<HEIGHT; y++) {
        for (size_t x=0; x<WIDTH; x++) {
            image[i*CHANNELSINDATA]=0.0*MAXVAL;
            if (x==y) image[i*CHANNELSINDATA]=0.99*MAXVAL;
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) image[i*CHANNELSINDATA]=0.8*MAXVAL;
            else if (x%PATTERNSIZE==0) image[i*CHANNELSINDATA]=0.25*MAXVAL;
            else if (y%PATTERNSIZE==0) image[i*CHANNELSINDATA]=0.2*MAXVAL;
            i++;
        }
    }
}

// invert the image in  image
template<class TIMAGESAMPLETYPE>
void invertTestImage(TIMAGESAMPLETYPE* image,size_t WIDTH, size_t HEIGHT, size_t SAMPLES=1) {
    for (size_t i=0; i<WIDTH*HEIGHT*SAMPLES; i++) {
        image[i]=std::numeric_limits<TIMAGESAMPLETYPE>::max()-image[i];
    }
}

// invert the image in  image
template<class TIMAGESAMPLETYPE>
void invertFloatTestImage(TIMAGESAMPLETYPE* image,size_t WIDTH, size_t HEIGHT, size_t SAMPLES=1, TIMAGESAMPLETYPE MAXVAL=1.0) {
    for (size_t i=0; i<WIDTH*HEIGHT*SAMPLES; i++) {
        image[i]=MAXVAL-image[i];
    }
}

// write a RGB test pattern into image, chunky organization
template<class TIMAGESAMPLETYPE>
void writeRGBTestDataChunky(TIMAGESAMPLETYPE* imagergb,size_t WIDTH, size_t HEIGHT, size_t PATTERNSIZE, size_t CHANNELSINDATA=3) {
    size_t i=0;
    for (size_t y=0; y<HEIGHT; y++) {
        for (size_t x=0; x<WIDTH; x++) {
            imagergb[i*CHANNELSINDATA+0]=x*std::numeric_limits<TIMAGESAMPLETYPE>::max()/WIDTH;
            imagergb[i*CHANNELSINDATA+1]=0;
            imagergb[i*CHANNELSINDATA+2]=y*std::numeric_limits<TIMAGESAMPLETYPE>::max()/HEIGHT;
            if (x==y) imagergb[i*CHANNELSINDATA+1]=std::numeric_limits<TIMAGESAMPLETYPE>::max()-1;
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) imagergb[i*CHANNELSINDATA+1]=std::numeric_limits<TIMAGESAMPLETYPE>::max()-std::numeric_limits<TIMAGESAMPLETYPE>::max()/5;
            else if (x%PATTERNSIZE==0) imagergb[i*CHANNELSINDATA+1]=std::numeric_limits<TIMAGESAMPLETYPE>::max()/4;
            else if (y%PATTERNSIZE==0) imagergb[i*CHANNELSINDATA+1]=std::numeric_limits<TIMAGESAMPLETYPE>::max()/2;
            i++;
        }
    }
}

// write a RGB test pattern into image, planar organization
template<class TIMAGESAMPLETYPE>
void writeRGBTestDataPlanar(TIMAGESAMPLETYPE* imagergb,size_t WIDTH, size_t HEIGHT, size_t PATTERNSIZE, size_t CHANNELSINDATA=3) {
    size_t i=0;
    for (size_t y=0; y<HEIGHT; y++) {
        for (size_t x=0; x<WIDTH; x++) {
            imagergb[i+0*WIDTH*HEIGHT]=x*std::numeric_limits<TIMAGESAMPLETYPE>::max()/WIDTH;
            imagergb[i+1*WIDTH*HEIGHT]=0;
            imagergb[i+2*WIDTH*HEIGHT]=y*std::numeric_limits<TIMAGESAMPLETYPE>::max()/HEIGHT;
            if (x==y) imagergb[i+1*WIDTH*HEIGHT]=std::numeric_limits<TIMAGESAMPLETYPE>::max()-1;
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) imagergb[i+1*WIDTH*HEIGHT]=std::numeric_limits<TIMAGESAMPLETYPE>::max()-std::numeric_limits<TIMAGESAMPLETYPE>::max()/5;
            else if (x%PATTERNSIZE==0) imagergb[i+1*WIDTH*HEIGHT]=std::numeric_limits<TIMAGESAMPLETYPE>::max()/4;
            else if (y%PATTERNSIZE==0) imagergb[i+1*WIDTH*HEIGHT]=std::numeric_limits<TIMAGESAMPLETYPE>::max()/2;
            i++;
        }
    }
}

// write a RGB test pattern into image
template<class TIMAGESAMPLETYPE>
void writeALPHATestData(TIMAGESAMPLETYPE* imagergba, size_t alpha_channel,size_t WIDTH, size_t HEIGHT, size_t CHANNELSINDATA=4) {
    size_t i=0;
    for (size_t y=0; y<HEIGHT; y++) {
        for (size_t x=0; x<WIDTH; x++) {
            const int r2=(x-WIDTH/2)*(x-WIDTH/2)+(y-HEIGHT/2)*(y-HEIGHT/2);
            const int r2max=WIDTH*WIDTH/4+HEIGHT*HEIGHT/4+2;

            imagergba[i*CHANNELSINDATA+alpha_channel]=TIMAGESAMPLETYPE((r2max-r2)*std::numeric_limits<TIMAGESAMPLETYPE>::max()/r2max);

            i++;
        }
    }
}



#endif // TESTIMAGE_TOOLS_H
