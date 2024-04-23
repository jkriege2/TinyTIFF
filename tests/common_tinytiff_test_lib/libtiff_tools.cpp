/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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



#include "libtiff_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <float.h>
#include <cmath>
#include <string.h>

static bool TIFFMakeImageDescription(char* description, double pixel_width, double pixel_height, double deltaz, double timestep) {
    bool ok=false;
    char spw[96];
    sprintf(description, "JKLIBTIFF_version=1.1");
    if (fabs(pixel_width)>10.0*DBL_MIN) {
        sprintf(spw, "\npixel_width=%lf ", pixel_width);
        strcat(description, spw);
        ok=true;
    }
    if (fabs(pixel_height)>10.0*DBL_MIN) {
        sprintf(spw, "\npixel_height=%lf ", pixel_height);
        strcat(description,spw);
        ok=true;
    }
    if (fabs(deltaz)>10.0*DBL_MIN) {
        sprintf(spw, "\ndeltaz=%lf ", deltaz);
        strcat(description,spw);
        ok=true;
    }
    if (fabs(timestep)>10.0*DBL_MIN) {
        sprintf(spw, "\nframetime=%lg ", timestep);
        strcat(description,spw);
        ok=true;
    }
    return ok;
}


bool TIFFTWriteFloat(TIFF* tif, const float* image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(float)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);    

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    float* const buf = (float*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (float)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(float))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}


bool TIFFTWriteDoubleAsFloat(TIFF* tif, const double* image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(float)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    float* const buf = (float*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (float)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(float))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool TIFFTWriteFloatfrom32(TIFF* tif, const uint32_t *image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(float)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    float* const buf = (float*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (float)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(float))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool TIFFTWriteUint16(TIFF* tif, const uint16_t* image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint16_t* const buf = (uint16_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint16_t)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint16_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}


bool TIFFTWriteUint32(TIFF* tif, const uint32_t *image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 32);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint32_t* const buf = (uint32_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint32_t)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint32_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}



bool TIFFTWriteUint64(TIFF *tif, const uint64_t *image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double frametime, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 64);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, frametime)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint64_t* const buf = (uint64_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint64_t)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint64_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool TIFFTWriteUint16from32(TIFF* tif, const uint32_t* image, uint16_t width, uint16_t height, bool scaled, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    uint32_t imin=image[0];
    uint32_t imax=image[0];
    double factor=1;
    if (scaled) {
        for (int32_t i=0; i<width*height; i++) {
            if (image[i]>imax) imax=image[i];
            if (image[i]<imin) imin=image[i];
        }
        factor=65535.0/(double)imax;
        if (factor>=1.0) factor=1.0;
    }
    //std::cout<<"factor = "<<factor<<"   imax="<<imax<<"   imin="<<imin<<"\n";

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint16_t* const buf = (uint16_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        if (scaled) {
            for (unsigned int rr = 0; rr<nrow; ++rr) {
                for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                    uint32_t d=image[cc+(row + rr)*frame_width];
                    d=(uint32_t)round((double)d*factor);
                    if (d>65535) d=65535;
                    buf[bi++] = d;
                }
            }
        } else {
            for (unsigned int rr = 0; rr<nrow; ++rr) {
                for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                    uint32_t d=image[cc+(row + rr)*frame_width];
                    if (d>0xFFFF) d=0xFFFF;
                    buf[bi++] = d;
                }
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint16_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool TIFFTWriteUint8(TIFF* tif, const uint8_t* image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression) {
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint8_t* const buf = (uint8_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint8_t)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint8_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool TIFFTReadUInt16(TIFF* tif, uint16_t** image, uint16_t* width, uint16_t* height, char* errormessage, unsigned int /*sample*/) {
    uint16_t samplesperpixel=1;
    uint16_t bitspersample=16;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint32_t nx=0;
    uint32_t ny=0;
    uint16_t* tiffbuf = NULL;
    uint16_t* imagebuf=NULL;
    uint32_t row = 0;
    uint32_t rowsperstrip = 0;
    unsigned int rr=0;
    unsigned int cc=0;
    bool ok=true;

    TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
    TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
    TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
    TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
    TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);


    if (sampleformat!=SAMPLEFORMAT_UINT) {
        if (errormessage) sprintf(errormessage, "can not read sample format other than uint");
        return false;
    }

    if ((bitspersample!=16)) {
        if (errormessage) sprintf(errormessage, "can not read sample with %d bits depth", bitspersample);
        return false;
    }

    if ((*image==NULL)||(nx!=*width)||(ny!=*height)) {
        *image=(uint16_t*)realloc(*image, nx*ny*sizeof(uint16_t));
        *width=nx;
        *height=ny;
    }

    imagebuf=*image;
    if (imagebuf==NULL) {
        if (errormessage) sprintf(errormessage, "error allocating memory (%.1f kBytes)", (float)(nx*ny*sizeof(uint16_t))/1024.0);
        return false;
    }

    tiffbuf = (uint16_t*)_TIFFmalloc(TIFFStripSize(tif));
    if (tiffbuf) {
        row = 0;
        rowsperstrip = 0;
        TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
        for (row = 0; ok&&(row<ny); row+= rowsperstrip) {
            uint32_t nrow =  ( (row+rowsperstrip>ny)?(ny-row):rowsperstrip);
            tstrip_t strip = TIFFComputeStrip(tif, row, 0);
            if ((TIFFReadEncodedStrip(tif,strip,tiffbuf,-1))<0) {
                ok=false;
                if (errormessage) sprintf(errormessage, "invalid strip");
                break;
            }
            uint16_t* ptr = tiffbuf;
            int iii=0;
            for (rr = 0;rr<nrow; ++rr) {
                for (cc = 0; cc<nx; ++cc) {
                    uint16_t t=(uint16_t)*(ptr++);
                    iii++;
                    imagebuf[cc+(row+rr)*nx]=t;
                }
            }
        }
    } else {
        if (errormessage) sprintf(errormessage, "error allocating memory buffer for TIFF strip");
        return false;
    }
    if (tiffbuf) _TIFFfree(tiffbuf);

    return ok;
}

uint32_t TIFFCountDirectories(TIFF* tif) {
    TIFFSetDirectory(tif,0);
    uint32_t n_images =0;
    do ++n_images; while (TIFFReadDirectory(tif));
    TIFFSetDirectory(tif,0);
    return n_images;
}




















bool TIFFTWriteDouble(TIFF *tif, const double *image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression)
{
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(double)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    float* const buf = (float*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (float)image[cc+(row + rr)*frame_width];
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(float))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}


bool TIFFTWriteBoolAsUint8(TIFF *tif, const bool *image, uint16_t width, uint16_t height, double pixel_width, double pixel_height, double deltaz, double timestep, uint32_t compression)
{
    uint16_t frame_width=width;
    uint16_t frame_height=height;
    uint32_t rowsperstrip = (uint32_t)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    char imgd[1024];
    if (TIFFMakeImageDescription(imgd, pixel_width, pixel_height, deltaz, timestep)) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,imgd);


    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint8_t* const buf = (uint8_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32_t nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint8_t)(image[cc+(row + rr)*frame_width]?1:0);
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint8_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}


template <>
bool TIFFWrite<uint8_t>(TIFF* tif, const uint8_t* image, uint16_t width, uint16_t height) {
    return TIFFTWriteUint8(tif, image, width, height);
}
template <>
bool TIFFWrite<uint16_t>(TIFF* tif, const uint16_t* image, uint16_t width, uint16_t height) {
    return TIFFTWriteUint16(tif, image, width, height);
}
template <>
bool TIFFWrite<uint32_t>(TIFF* tif, const uint32_t* image, uint16_t width, uint16_t height) {
    return TIFFTWriteUint32(tif, image, width, height);
}
template <>
bool TIFFWrite<uint64_t>(TIFF* tif, const uint64_t* image, uint16_t width, uint16_t height) {
    return TIFFTWriteUint64(tif, image, width, height);
}

