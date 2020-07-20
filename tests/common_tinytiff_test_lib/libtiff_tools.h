/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-07-07 12:07:58 +0200 (Di, 07 Jul 2015) $  (revision $Rev: 4005 $)

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





/**
 * \defgroup libtiff_tools tool functions for libtiff
 * \ingroup tools
 *
 * Functions in this group add features to the <a href="http://www.libtiff.org">libtiff library for TIFF file I/O</a>.
 */

/** \file libtiff_tools.h
 *  \ingroup libtiff_tools
 *
 * This file contains a collection of tool routines to be used in conjunction with libtiff.
 */

#ifndef LIBTIFF_TOOLS_H_INCLUDED
#define LIBTIFF_TOOLS_H_INCLUDED

#define TIFFmin(a,b) (((a)<(b))?(a):(b))

// include libTIFF header
#include <tiffio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

/*! \brief write a float image to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteFloat(TIFF* tif, const float *image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);

/*! \brief write a float image to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteDouble(TIFF* tif, double* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);


/*! \brief write a double image as a float to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteDoubleAsFloat(TIFF* tif, double* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);

/*! \brief write a uint16 image to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteUint16(TIFF* tif, uint16* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);
/*! \brief write a uint32 image to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteUint32(TIFF* tif, uint32* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);


/*! \brief write a uint32 image to the TIFF file in uint16 format
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \param scaled if this is \c true, the image is scaled to 0..0xFFFF otherwise values>0xFFFF are simply cut to 0xFFFF
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim


    This function scales the contents of the array to the 16-bit range if and only if pixels are larger than 65535.
*/
bool TIFFTWriteUint16from32(TIFF* tif, uint32_t* image, uint16 width, uint16 height, bool scaled=true, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);

/*! \brief write a uint32 image to the TIFF file in float format
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim


*/
bool TIFFTWriteFloatfrom32(TIFF* tif, uint32_t* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);



/*! \brief write a uint8 image to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteUint8(TIFF* tif, uint8* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);

/*! \brief write an image of bools as uint8 image (false=0/true=1) to the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image points to the image data
    \param width width of the  image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim

*/
bool TIFFTWriteBoolAsUint8(TIFF* tif, bool* image, uint16 width, uint16 height, double pixel_width=0.0, double pixel_height=0.0, double deltaz=0.0, double frametime=0.0, uint32 compression=COMPRESSION_NONE);

/*! \brief return the number of directories in a TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
*/
uint32 TIFFCountDirectories(TIFF* tif);


/*! \brief read a uint16 image from the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param image image will be written here. This points to a pointer/array for the data. If the size
                 is not yet correct (as given by \a width/\a height), or \c *image==NULL a new array
                 will be allocated and the pointer returned in this variable. The old array will be
                 freed (internal call to \c realloc() !)
    \param width width of the image in pixels
    \param height height of the  image in pixels
    \param errormessage may be used to return error messages (if \c !=NULL )
    \param sample which sample to read in (default is 0, i.e. the first sample)
    \return \c true on success, \c false on failure
    \param pixel_width pixel width in nanometers
    \param pixel_height pixel width in nanometers
    \param deltaz in a multi-frame-TIFF distance between image planes in nanometers
    \param frametime in a multi-frame-TIFF frametime in seconds

    This functions writes some additional data into the ImageDescription field of the first frame, if it is proved (!=0!!!).
    It also writes the image count there. The ImageDescription finally has the form:
\verbatim
  TinyTIFFWriter_version=1.1
  images=1000
  pixel_width=100
  pixel_height=100
  deltaz=100
  frametime=1e-4
\endverbatim


    \note After a call to this function (even if it returns \c false ) there may be memory allocated in
    \c *image !!!
*/
bool TIFFTReadUInt16(TIFF* tif, uint16** image, uint16* width, uint16* height, char* errormessage=NULL, unsigned int sample=0);








/*! \brief read a non-tiled one-channel image image from the TIFF file
    \ingroup libtiff_tools

    \param data_out array to write the image to
    \param tif the TIFF file to write to
    \param width width of the image in pixels
    \param height height of the  image in pixels
    \return \c true on success, \c false on failure
*/
template<typename t, typename tout>
inline bool TIFFLoadNontiled(tout* data_out, TIFF *const tif, const uint32 width, const uint32 height, const uint32 sample=0, const uint32 samplesperpixel=1, const uint16 planarconfig=PLANARCONFIG_SEPARATE) {
    if (planarconfig==PLANARCONFIG_SEPARATE) { // sample order RRR...GGG...BBB...
        t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
        if (buf && data_out) {
            uint32 row, rowsperstrip = (uint32)-1;
            TIFFGetField(tif,TIFFTAG_ROWSPERSTRIP,&rowsperstrip);
            for (row = 0; row<height; row+= rowsperstrip) {
                uint32 nrow = (row+rowsperstrip>height?height-row:rowsperstrip);
                tstrip_t strip = TIFFComputeStrip(tif, row, sample);
                if ((TIFFReadEncodedStrip(tif,strip,buf,-1))<0) {
                    _TIFFfree(buf);
                    return false;
                }
                const t *ptr = buf;
                for (unsigned int rr = 0; rr<nrow; ++rr) {
                    for (unsigned int cc = 0; cc<width; ++cc) {
                        data_out[cc+(row+rr)*width] = (tout)*(ptr++);
                    }
                }
            }
            _TIFFfree(buf);
        } else return false;
        return true;
    } else if (planarconfig==PLANARCONFIG_CONTIG) { // sample order RGBRGBRGB...
        t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
        if (buf && data_out) {
            uint32 row, rowsperstrip = (uint32)-1;
            TIFFGetField(tif,TIFFTAG_ROWSPERSTRIP,&rowsperstrip);
            for (row = 0; row<height; row+= rowsperstrip) {
                uint32 nrow = (row+rowsperstrip>height?height-row:rowsperstrip);
                tstrip_t strip = TIFFComputeStrip(tif, row, 0);
                if ((TIFFReadEncodedStrip(tif,strip,buf,-1))<0) {
                    _TIFFfree(buf);
                    return false;
                }
                const t *ptr = buf;
                for (unsigned int rr = 0; rr<nrow; ++rr) {
                    for (unsigned int cc = 0; cc<width; ++cc) {
                        for (unsigned int vv = 0; vv<samplesperpixel; ++vv) {
                            if (vv==sample) data_out[cc+(row+rr)*width] = (tout)*(ptr);
                            ptr++;
                        }
                    }
                }
            }
            _TIFFfree(buf);
        } else return false;
        return true;
    }
    return false;
}


/*! \brief read a tiled one-channel image image from the TIFF file
    \ingroup libtiff_tools

    \param data_out array to write the image to
    \param tif the TIFF file to write to
    \param width width of the image in pixels
    \param height height of the  image in pixels
    \param tile_width width of a tile in pixels
    \param tile_height height of a tile in pixels
    \return \c true on success, \c false on failure
*/
template<typename t, typename tout>
inline bool TIFFLoadTiled(tout* data_out, TIFF *const tif, const uint32 width, const uint32 height, const uint32 tile_width, const uint32 tile_height, const uint32 sample=0, const uint32 samplesperpixel=1, const uint16 planarconfig=PLANARCONFIG_SEPARATE) {
    if (planarconfig==PLANARCONFIG_SEPARATE) { // sample order RRR...GGG...BBB...
        t *const buf = (t*)_TIFFmalloc(TIFFTileSize(tif));
        if (buf) {
            //unsigned int sample=0;
            for (unsigned int row = 0; row<height; row+=tile_height) {
                for (unsigned int col = 0; col<width; col+=tile_width) {
                    if (TIFFReadTile(tif,buf,col,row,0,sample)<0) {
                        _TIFFfree(buf);
                        return false;
                    }
                    const t *ptr = buf;
                    for (unsigned int rr = row; rr<TIFFmin((unsigned int)(row+tile_height),(unsigned int)height); ++rr) {
                        for (unsigned int cc = col; cc<TIFFmin((unsigned int)(col+tile_width),(unsigned int)width); ++cc) {
                            data_out[cc+rr*width] = (tout)*(ptr++);
                        }
                    }
                }
            }
            _TIFFfree(buf);
        } else return false;
        return true;
    } else if (planarconfig==PLANARCONFIG_CONTIG) { // sample order RGBRGBRGB...
        t *const buf = (t*)_TIFFmalloc(TIFFTileSize(tif));
        if (buf) {
            //unsigned int sample=0;
            for (unsigned int row = 0; row<height; row+=tile_height) {
                for (unsigned int col = 0; col<width; col+=tile_width) {
                    if (TIFFReadTile(tif,buf,col,row,0,0)<0) {
                        _TIFFfree(buf);
                        return false;
                    }
                    const t *ptr = buf;
                    for (unsigned int rr = row; rr<TIFFmin((unsigned int)(row+tile_height),(unsigned int)height); ++rr) {
                        for (unsigned int cc = col; cc<TIFFmin((unsigned int)(col+tile_width),(unsigned int)width); ++cc) {
                            for (unsigned int vv = 0; vv<samplesperpixel; ++vv) {
                                if (vv==sample) data_out[cc+rr*width] = (tout)*(ptr);
                                ptr++;
                            }
                        }
                    }
                }
            }
            _TIFFfree(buf);
        } else return false;
        return true;
    }
    return false;
}



/*! \brief read a tiled or non-tiled one-channel image image from the TIFF file
    \ingroup libtiff_tools

    \param tif the TIFF file to write to
    \param data array to write the image to, must be of a size large enough to read the frame
    \return \c true on success, \c false on failure
*/
template<typename T>
inline bool TIFFReadFrame(TIFF *const tif, T* data) {
    if (!tif) return false;

    uint16 samplesperpixel, bitspersample;
    uint16 sampleformat = SAMPLEFORMAT_UINT;
    uint32 nx,ny;
    TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
    TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
    TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
    TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
    TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);
    if (samplesperpixel>1) {
        return false;
    } else {
        uint16 photo, config;
        TIFFGetField(tif,TIFFTAG_PLANARCONFIG,&config);
        TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo);
        // we only read image with one sample per pixel, so we do nothave to care for TIFFTAG_PLANARCONFIG
        if (TIFFIsTiled(tif)) { // load a tiled frame
            uint32 tw, th;
            TIFFGetField(tif,TIFFTAG_TILEWIDTH,&tw);
            TIFFGetField(tif,TIFFTAG_TILELENGTH,&th);
            if (bitspersample==8 && sizeof(T)>=1) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadTiled<uint8_t,  T>(data,  tif, nx, ny, tw, th);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadTiled<int8_t,  T>(data,  tif, nx, ny, tw, th);
                else {
                    return false;
                }
            } else if (bitspersample==16 && sizeof(T)>=2) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadTiled<uint16_t,  T>(data,  tif, nx, ny, tw, th);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadTiled<int16_t,  T>(data,  tif, nx, ny, tw, th);
                else {
                    return false;
                }
            } else if (bitspersample==32 && sizeof(T)>=4) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadTiled<uint32_t,  T>(data,  tif, nx, ny, tw, th);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadTiled<int32_t,  T>(data,  tif, nx, ny, tw, th);
                else if (sampleformat==SAMPLEFORMAT_IEEEFP) TIFFLoadTiled<float,  T>(data,  tif, nx, ny, tw, th);
                else {
                    return false;
                }
            } else if (bitspersample==64 && sizeof(T)>=8) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadTiled<uint64_t,  T>(data,  tif, nx, ny, tw, th);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadTiled<int64_t,  T>(data,  tif, nx, ny, tw, th);
                else if (sampleformat==SAMPLEFORMAT_IEEEFP) TIFFLoadTiled<double,  T>(data,  tif, nx, ny, tw, th);
                else {
                    return false;
                }
            } else {
                return false;
            }

        } else { // load a non-tiled frame
            if (bitspersample==8 && sizeof(T)>=1) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadNontiled<uint8_t,  T>(data,  tif, nx, ny);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadNontiled<int8_t,  T>(data,  tif, nx, ny);
                else {
                    return false;
                }
            } else if (bitspersample==16 && sizeof(T)>=2) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadNontiled<uint16_t,  T>(data,  tif, nx, ny);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadNontiled<int16_t,  T>(data,  tif, nx, ny);
                else {
                    return false;
                }
            } else if (bitspersample==32 && sizeof(T)>=4) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadNontiled<uint32_t,  T>(data,  tif, nx, ny);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadNontiled<int32_t,  T>(data,  tif, nx, ny);
                else if (sampleformat==SAMPLEFORMAT_IEEEFP) TIFFLoadNontiled<float,  T>(data,  tif, nx, ny);
                else {
                    return false;
                }
            } else if (bitspersample==64 && sizeof(T)>=8) {
                if (sampleformat==SAMPLEFORMAT_UINT) TIFFLoadNontiled<uint64_t,  T>(data,  tif, nx, ny);
                else if (sampleformat==SAMPLEFORMAT_INT) TIFFLoadNontiled<int64_t,  T>(data,  tif, nx, ny);
                else if (sampleformat==SAMPLEFORMAT_IEEEFP) TIFFLoadNontiled<double,  T>(data,  tif, nx, ny);
                else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}




template<typename T>
inline bool TIFFReadFrame(char* filename, T** data, int* width, int* height) {
    bool ok=false;

    if (*data) free(*data);
    *data=NULL;
    *width=0;
    *height=0;

    TIFF* tif=TIFFOpen(filename, "r");
    if (tif) {
        uint32 nx,ny;
        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
        *width=nx;
        *height=ny;
        *data=(double*)malloc(nx*ny*sizeof(double));
        ok=TIFFReadFrame<T>(tif, *data);
        TIFFClose(tif);
    }

    return ok;
}

template<typename T>
inline bool TIFFReadRGB(TIFF* tif, T* data, uint32 nx, uint32 ny, uint16 channel) {
    uint16 photo=1;
    uint16 sampleformat=SAMPLEFORMAT_UINT;
    uint16 samplesperpixel=1;
    uint16 bitspersample=1;
    TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo);
    TIFFGetField(tif,TIFFTAG_SAMPLEFORMAT,&sampleformat);
    TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
    TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);
    if (photo>=3 && sampleformat==SAMPLEFORMAT_UINT && bitspersample==8 && (samplesperpixel==3 || samplesperpixel==4)) {
        uint32 *const raster = (uint32*)_TIFFmalloc(nx*ny*sizeof(uint32));
        if (!raster) {
            return false;
        }
        TIFFReadRGBAImage(tif,nx,ny,raster,0);
        switch (samplesperpixel) {
            case 1 : {
                if (channel==0) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetR(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else {
                    _TIFFfree(raster);
                    return false;
                }
            } break;
            case 3 : {
                if (channel==0) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetR(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else if (channel==1) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetG(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else if (channel==2) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetB(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else {
                    _TIFFfree(raster);
                    return false;
                }
            } break;
            case 4 : {
                if (channel==0) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetR(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else if (channel==1) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetG(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else if (channel==2) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetB(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else if (channel==3) {
                    for (uint32 y=0; y<ny; y++) {
                        for (uint32 x=0; x<nx; x++) {
                            data[y*nx+x]=TIFFGetA(raster[nx*(ny-1-y)+x]);
                        }
                    }
                } else {
                    _TIFFfree(raster);
                    return false;
                }
            } break;
        }
        _TIFFfree(raster);
        return true;
    }
    return false;
}


#endif // LIBTIFF_TOOLS_H_INCLUDED
