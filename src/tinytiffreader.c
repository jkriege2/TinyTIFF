/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/
 #include "tinytiffreader.h"

//#define DEBUG_IFDTIMING
#ifdef DEBUG_IFDTIMING
#include "highrestimer.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>



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

#define __USE_LIBC_FOR_TIFFReader__

#ifdef __WINDOWS__
#  ifndef __USE_LIBC_FOR_TIFFReader__
#    define TINYTIFF_USE_WINAPI_FOR_FILEIO
#  endif
#endif // __WINDOWS__

#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
#  include <windows.h>
#  warning COMPILING TinyTIFFReader with WinAPI
#  define TinyTIFFReader_POSTYPE DWORD
#else
#  define TinyTIFFReader_POSTYPE fpos_t
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO

#define TIFF_LAST_ERROR_SIZE 1024


#define TIFF_ORDER_UNKNOWN 0
#define TIFF_ORDER_BIGENDIAN 1
#define TIFF_ORDER_LITTLEENDIAN 2


#define TIFF_FIELD_IMAGEWIDTH 256
#define TIFF_FIELD_IMAGELENGTH 257
#define TIFF_FIELD_BITSPERSAMPLE 258
#define TIFF_FIELD_COMPRESSION 259
#define TIFF_FIELD_PHOTOMETRICINTERPRETATION 262
#define TIFF_FIELD_IMAGEDESCRIPTION 270
#define TIFF_FIELD_STRIPOFFSETS 273
#define TIFF_FIELD_SAMPLESPERPIXEL 277
#define TIFF_FIELD_ROWSPERSTRIP 278
#define TIFF_FIELD_STRIPBYTECOUNTS 279
#define TIFF_FIELD_XRESOLUTION 282
#define TIFF_FIELD_YRESOLUTION 283
#define TIFF_FIELD_PLANARCONFIG 284
#define TIFF_FIELD_RESOLUTIONUNIT 296
#define TIFF_FIELD_SAMPLEFORMAT 339

#define TIFF_TYPE_BYTE 1
#define TIFF_TYPE_ASCII 2
#define TIFF_TYPE_SHORT 3
#define TIFF_TYPE_LONG 4
#define TIFF_TYPE_RATIONAL 5

#define TIFF_COMPRESSION_NONE 1
#define TIFF_COMPRESSION_CCITT 2
#define TIFF_COMPRESSION_PACKBITS 32773

#define TIFF_PLANARCONFIG_CHUNKY 1
#define TIFF_PLANARCONFIG_PLANAR 2




#define TIFF_HEADER_SIZE 510
#define TIFF_HEADER_MAX_ENTRIES 16


int TIFFReader_get_byteorder() {
    union {
        long l;
        char c[4];
    } test;
    test.l = 1;
    if( test.c[3] && !test.c[2] && !test.c[1] && !test.c[0] )
        return TIFF_ORDER_BIGENDIAN;

    if( !test.c[3] && !test.c[2] && !test.c[1] && test.c[0] )
        return TIFF_ORDER_LITTLEENDIAN;

    return TIFF_ORDER_UNKNOWN;
}

typedef struct TinyTIFFReaderFrame {
    uint32_t width;
    uint32_t height;
    uint16_t compression;

    uint32_t rowsperstrip;
    uint32_t* stripoffsets;
    uint32_t* stripbytecounts;
    uint32_t stripcount;
    uint16_t samplesperpixel;
    uint16_t* bitspersample;
    uint16_t planarconfiguration;
    uint16_t sampleformat;
    uint32_t imagelength;
	
	char* description;
} TinyTIFFReaderFrame;

static TinyTIFFReaderFrame TinyTIFFReader_getEmptyFrame() {
    TinyTIFFReaderFrame d;
    d.width=0;
    d.height=0;
    d.stripcount=0;
    d.compression=TIFF_COMPRESSION_NONE;
    d.rowsperstrip=0;
    d.stripoffsets=0;
    d.stripbytecounts=0;
    d.samplesperpixel=1;
    d.bitspersample=0;
    d.planarconfiguration=TIFF_PLANARCONFIG_PLANAR;
    d.sampleformat=TINYTIFFREADER_SAMPLEFORMAT_UINT;
    d.imagelength=0;
	d.description=0;
    return d;
}

static void TinyTIFFReader_freeEmptyFrame(TinyTIFFReaderFrame f) {
    if (f.stripoffsets) free(f.stripoffsets);
    f.stripoffsets=NULL;
    if (f.stripbytecounts) free(f.stripbytecounts);
    f.stripbytecounts=NULL;
    if (f.bitspersample) free(f.bitspersample);
    f.bitspersample=NULL;
	if (f.description) free(f.description);
	f.description=NULL;
}


struct TinyTIFFReaderFile {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
    HANDLE hFile;
#else
    FILE* file;
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO

    char lastError[TIFF_LAST_ERROR_SIZE];
    int wasError;

    uint8_t systembyteorder;
    uint8_t filebyteorder;

    uint32_t firstrecord_offset;
    uint32_t nextifd_offset;

    uint64_t filesize;
	
    TinyTIFFReaderFrame currentFrame;
};


void TinyTIFFReader_fopen(TinyTIFFReaderFile* tiff, const char* filename) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
    tiff->hFile = CreateFile(filename,               // name of the write
                       GENERIC_READ,          // open for writing
                       FILE_SHARE_READ,
                       NULL,                   // default security
                       OPEN_EXISTING,             // create new file only
                       FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,  // normal file
                       NULL);                  // no attr. template
#else
#  ifdef HAVE_FOPEN_S
    fopen_s(&(tiff->file), filename, "rb");
#  else
    tiff->file=fopen(filename, "rb");
#  endif
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}

int TinyTIFFReader_fclose(TinyTIFFReaderFile* tiff) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
    CloseHandle(tiff->hFile);
    return 0;
#else
    int r=fclose(tiff->file);
    tiff->file=NULL;
    return r;
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}

int TinyTIFFReader_fOK(const TinyTIFFReaderFile* tiff)  {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
   if (tiff->hFile == INVALID_HANDLE_VALUE) return TINYTIFFREADER_FALSE;
   else return TINYTIFFREADER_TRUE;
#else
    if (tiff->file) {
        return TINYTIFFREADER_TRUE;
    }
    return TINYTIFFREADER_FALSE;
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}

int TinyTIFFReader_fseek_set(TinyTIFFReaderFile* tiff, size_t offset) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
   DWORD res = SetFilePointer (tiff->hFile,
                                offset,
                                NULL,
                                FILE_BEGIN);


   return res;
#else
    return fseek(tiff->file, (long)offset, SEEK_SET);
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}

int TinyTIFFReader_fseek_cur(TinyTIFFReaderFile* tiff, size_t offset) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
   DWORD res = SetFilePointer (tiff->hFile,
                                offset,
                                NULL,
                                FILE_CURRENT);


   return res;
#else
    return fseek(tiff->file, (long)offset, SEEK_CUR);
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}

size_t TinyTIFFReader_fread(void * ptr, size_t ptrsize, size_t size, size_t count, TinyTIFFReaderFile* tiff) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
    DWORD  dwBytesRead = 0;
    if(!ReadFile(tiff->hFile, ptr, size*count, &dwBytesRead, NULL)) {
        return 0;
    }
    return dwBytesRead;
#else
#ifdef  HAVE_FREAD_S
    return fread_s(ptr, ptrsize, size, count, tiff->file);
#  else
    return fread(ptr, size, count, tiff->file);
#  endif
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}


static long int TinyTIFFReader_ftell ( TinyTIFFReaderFile * tiff ) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
DWORD dwPtr = SetFilePointer( tiff->hFile,
                                0,
                                NULL,
                                FILE_CURRENT );
    return dwPtr;
#else
    return ftell(tiff->file);
#endif
}

int TinyTIFFReader_fgetpos(TinyTIFFReaderFile* tiff, TinyTIFFReader_POSTYPE* pos) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
    *pos= SetFilePointer( tiff->hFile,
                                0,
                                NULL,
                                FILE_CURRENT );
    return 0;
#else
    return fgetpos(tiff->file, pos);
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}

int TinyTIFFReader_fsetpos(TinyTIFFReaderFile* tiff, const TinyTIFFReader_POSTYPE* pos) {
#ifdef TINYTIFF_USE_WINAPI_FOR_FILEIO
    SetFilePointer( tiff->hFile,
                                *pos,
                                NULL,
                                FILE_BEGIN );
    return 0;
#else
    return fsetpos(tiff->file, pos);
#endif // TINYTIFF_USE_WINAPI_FOR_FILEIO
}




const char* TinyTIFFReader_getLastError(TinyTIFFReaderFile* tiff) {
    if (tiff) return tiff->lastError;
    return NULL;
}

int TinyTIFFReader_wasError(TinyTIFFReaderFile* tiff) {
    if (tiff) return tiff->wasError;
    return TINYTIFFREADER_TRUE;
}

int TinyTIFFReader_success(TinyTIFFReaderFile* tiff) {
    if (tiff) return !tiff->wasError;
    return TINYTIFFREADER_FALSE;
}



static uint32_t TinyTIFFReader_Byteswap32(uint32_t nLongNumber)
{
   return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
   ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

static uint16_t TinyTIFFReader_Byteswap16(uint16_t nValue)
{
   return (((nValue>> 8)) | (nValue << 8));
}

static uint32_t TinyTIFFReader_readuint32(TinyTIFFReaderFile* tiff) {
    uint32_t res=0;
    //fread(&res, 4,1,tiff->file);
    TinyTIFFReader_fread(&res, sizeof(res), 4,1,tiff);
    if (tiff->systembyteorder!=tiff->filebyteorder) {
        res=TinyTIFFReader_Byteswap32(res);
    }
    return res;
}


static uint16_t TinyTIFFReader_readuint16(TinyTIFFReaderFile* tiff) {
    uint16_t res=0;
    //fread(&res, 2,1,tiff->file);
    TinyTIFFReader_fread(&res, sizeof(res), 2,1,tiff);
    if (tiff->systembyteorder!=tiff->filebyteorder) {
        res=TinyTIFFReader_Byteswap16(res);
    }
    return res;
}

static uint8_t TinyTIFFReader_readuint8(TinyTIFFReaderFile* tiff) {
    uint8_t res=0;
    //fread(&res, 1,1,tiff->file);
    TinyTIFFReader_fread(&res, sizeof(res), 1,1,tiff);
    return res;
}


typedef struct TinyTIFFReader_IFD {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint32_t value;
    uint32_t value2;

    uint32_t* pvalue;
    uint32_t* pvalue2;
} TinyTIFFReader_IFD;

static void TinyTIFFReader_freeIFD(TinyTIFFReader_IFD d) {
    if (d.pvalue /*&& d.count>1*/) { free(d.pvalue);  d.pvalue=NULL; }
    if (d.pvalue2 /*&& d.count>1*/) { free(d.pvalue2);  d.pvalue2=NULL; }
}

static TinyTIFFReader_IFD TinyTIFFReader_readIFD(TinyTIFFReaderFile* tiff) {
    TinyTIFFReader_IFD d;

    d.value=0;
    d.value2=0;

    d.pvalue=0;
    d.pvalue2=0;

    d.count=1;
    d.tag=TinyTIFFReader_readuint16(tiff);
    d.type=TinyTIFFReader_readuint16(tiff);
    d.count=TinyTIFFReader_readuint32(tiff);
    //uint32_t val=TinyTIFFReader_readuint32(tiff);
    TinyTIFFReader_POSTYPE pos;
    //fgetpos(tiff->file, &pos);
    TinyTIFFReader_fgetpos(tiff, &pos);
    int changedpos=TINYTIFFREADER_FALSE;
    //printf("    - pos=0x%X   tag=%d type=%d count=%u \n",pos, d.tag, d.type, d.count);
    switch(d.type) {
        case TIFF_TYPE_BYTE:
        case TIFF_TYPE_ASCII:
	    if (d.count>0) {
	      d.pvalue=(uint32_t*)calloc(d.count, sizeof(uint32_t));
	        if (d.count<=4) {
            unsigned int i;
		    for (i=0; i<4; i++) {
		      uint32_t v=TinyTIFFReader_readuint8(tiff);
		      if (i<d.count) d.pvalue[i]=v;
		    }
	      } else {
            changedpos=TINYTIFFREADER_TRUE;
		    uint32_t offset=TinyTIFFReader_readuint32(tiff);
			
		    if (offset+d.count*1<=tiff->filesize) {
		      //fseek(tiff->file, offset, SEEK_SET);
		      TinyTIFFReader_fseek_set(tiff, offset);
              unsigned int i;
		      for (i=0; i<d.count; i++) {
			    d.pvalue[i]=TinyTIFFReader_readuint8(tiff);
		      }
		    }
	      }
	    }
            d.pvalue2=NULL;
            //printf("    - BYTE/CHAR: tag=%d count=%u   val[0]=%u\n",d.tag,d.count, d.pvalue[0]);
            break;
        case TIFF_TYPE_SHORT:
            d.pvalue=(uint32_t*)calloc(d.count, sizeof(uint32_t));
            if (d.count<=2) {
                unsigned int i;
                for (i=0; i<2; i++) {
                    uint32_t v=TinyTIFFReader_readuint16(tiff);
                    if (i<d.count) d.pvalue[i]=v;
                }
            } else {
                changedpos=TINYTIFFREADER_TRUE;
                uint32_t offset=TinyTIFFReader_readuint32(tiff);
                if (offset+d.count*2<tiff->filesize) {
                    //fseek(tiff->file, offset, SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, offset);
                    unsigned int i;
                    for (i=0; i<d.count; i++) {
                        d.pvalue[i]=TinyTIFFReader_readuint16(tiff);
                    }
                }
            }
            d.pvalue2=NULL;
            //printf("    - SHORT: tag=%d count=%u   val[0]=%u\n",d.tag,d.count, d.pvalue[0]);
            break;

        case TIFF_TYPE_LONG:
            d.pvalue=(uint32_t*)calloc(d.count, sizeof(uint32_t));
            if (d.count<=1) {
                d.pvalue[0]=TinyTIFFReader_readuint32(tiff);
            } else {
                changedpos=TINYTIFFREADER_TRUE;
                uint32_t offset=TinyTIFFReader_readuint32(tiff);
                if (offset+d.count*4<tiff->filesize) {
                    //fseek(tiff->file, offset, SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, offset);
                    uint32_t i;
                    for (i=0; i<d.count; i++) {
                        d.pvalue[i]=TinyTIFFReader_readuint32(tiff);
                    }
                }
                //printf("    - LONG: pos=0x%X   offset=0x%X   tag=%d count=%u   val[0]=%u\n",pos, offset,d.tag,d.count, d.pvalue[0]);
            }
            d.pvalue2=NULL;
            //printf("    - LONG: tag=%d count=%u   val[0]=%u\n",d.tag,d.count, d.pvalue[0]);
            break;
        case TIFF_TYPE_RATIONAL: {
            d.pvalue=(uint32_t*)calloc(d.count, sizeof(uint32_t));
            d.pvalue2=(uint32_t*)calloc(d.count, sizeof(uint32_t));

            changedpos=TINYTIFFREADER_TRUE;
            uint32_t offset=TinyTIFFReader_readuint32(tiff);
            if (offset+d.count*4<tiff->filesize) {
                //fseek(tiff->file, offset, SEEK_SET);
                TinyTIFFReader_fseek_set(tiff, offset);
                uint32_t i;
                for (i=0; i<d.count; i++) {
                    d.pvalue[i]=TinyTIFFReader_readuint32(tiff);
                    d.pvalue2[i]=TinyTIFFReader_readuint32(tiff);
                }
            }
            //printf("    - RATIONAL: pos=0x%X   offset=0x%X   tag=%d count=%u   val[0]=%u/%u\n",pos, offset,d.tag,d.count, d.pvalue[0], d.pvalue[1]);
            } break;

       default: d.value=TinyTIFFReader_readuint32(tiff); break;
    }
    if (d.pvalue) d.value=d.pvalue[0];
    if (d.pvalue2) d.value2=d.pvalue2[0];

    if (changedpos) {
        //fsetpos(tiff->file, &pos);
        TinyTIFFReader_fsetpos(tiff, &pos);
        //fseek(tiff->file, 4, SEEK_CUR);
        TinyTIFFReader_fseek_cur(tiff, 4);
    }
    return d;
}


static void TinyTIFFReader_readNextFrame(TinyTIFFReaderFile* tiff) {

    TinyTIFFReader_freeEmptyFrame(tiff->currentFrame);
    tiff->currentFrame=TinyTIFFReader_getEmptyFrame();
    #ifdef DEBUG_IFDTIMING
    HighResTimer timer;
    timer.start();
    #endif
    if (tiff->nextifd_offset!=0 && tiff->nextifd_offset+2<tiff->filesize) {
        //printf("    - seeking=0x%X\n", tiff->nextifd_offset);
        //fseek(tiff->file, tiff->nextifd_offset, SEEK_SET);
        TinyTIFFReader_fseek_set(tiff, tiff->nextifd_offset);
        uint16_t ifd_count=TinyTIFFReader_readuint16(tiff);
        //printf("    - tag_count=%u\n", ifd_count);
        uint16_t i;
        for ( i=0; i<ifd_count; i++) {
    #ifdef DEBUG_IFDTIMING
            timer.start();
    #endif
            TinyTIFFReader_IFD ifd=TinyTIFFReader_readIFD(tiff);
    #ifdef DEBUG_IFDTIMING
            //printf("    - readIFD %d (tag: %u, type: %u, count: %u): %lf us\n", i, ifd.tag, ifd.type, ifd.count, timer.get_time());
    #endif
	        //printf("    - readIFD %d (tag: %u, type: %u, count: %u)\n", i, ifd.tag, ifd.type, ifd.count);
            switch(ifd.tag) {
                case TIFF_FIELD_IMAGEWIDTH: tiff->currentFrame.width=ifd.value;  break;
                case TIFF_FIELD_IMAGELENGTH: tiff->currentFrame.imagelength=ifd.value;  break;
                case TIFF_FIELD_BITSPERSAMPLE: {
                        tiff->currentFrame.bitspersample=(uint16_t*)malloc(ifd.count*sizeof(uint16_t));
                        memcpy(tiff->currentFrame.bitspersample, ifd.pvalue, ifd.count*sizeof(uint16_t));
                     } break;
                case TIFF_FIELD_COMPRESSION: tiff->currentFrame.compression=ifd.value; break;
                case TIFF_FIELD_STRIPOFFSETS: {
                        tiff->currentFrame.stripcount=ifd.count;
                        tiff->currentFrame.stripoffsets=(uint32_t*)calloc(ifd.count, sizeof(uint32_t));
                        memcpy(tiff->currentFrame.stripoffsets, ifd.pvalue, ifd.count*sizeof(uint32_t));
                    } break;
                case TIFF_FIELD_SAMPLESPERPIXEL: tiff->currentFrame.samplesperpixel=ifd.value; break;
                case TIFF_FIELD_ROWSPERSTRIP: tiff->currentFrame.rowsperstrip=ifd.value; break;
                case TIFF_FIELD_SAMPLEFORMAT: tiff->currentFrame.sampleformat=ifd.value; break;
                case TIFF_FIELD_IMAGEDESCRIPTION: {
                        //printf("TIFF_FIELD_IMAGEDESCRIPTION: (tag: %u, type: %u, count: %u)\n", ifd.tag, ifd.type, ifd.count);
				        if (ifd.count>0) {
				            if (tiff->currentFrame.description) free(tiff->currentFrame.description);
							tiff->currentFrame.description=(char*)calloc(ifd.count+1, sizeof(char));
                            memset(tiff->currentFrame.description, 0, ifd.count+1);
							for (uint32_t ji=0; ji<ifd.count; ji++) {
							    tiff->currentFrame.description[ji]=(char)ifd.pvalue[ji];
								//printf(" %d[%d]", int(tiff->currentFrame.description[ji]), int(ifd.pvalue[ji]));
							}
							//printf("\n  %s\n", tiff->currentFrame.description);
					    }
                    } break;
                case TIFF_FIELD_STRIPBYTECOUNTS: {
                        tiff->currentFrame.stripcount=ifd.count;
                        tiff->currentFrame.stripbytecounts=(uint32_t*)calloc(ifd.count, sizeof(uint32_t));
                        memcpy(tiff->currentFrame.stripbytecounts, ifd.pvalue, ifd.count*sizeof(uint32_t));
                    } break;
                case TIFF_FIELD_PLANARCONFIG: tiff->currentFrame.planarconfiguration=ifd.value; break;
                default: break;
            }
            TinyTIFFReader_freeIFD(ifd);
            //printf("    - tag=%u\n", ifd.tag);
        }
        tiff->currentFrame.height=tiff->currentFrame.imagelength;
        //printf("      - width=%u\n", tiff->currentFrame.width);
        //printf("      - height=%u\n", tiff->currentFrame.height);
        //fseek(tiff->file, tiff->nextifd_offset+2+12*ifd_count, SEEK_SET);
        TinyTIFFReader_fseek_set(tiff, tiff->nextifd_offset+2+12*ifd_count);
        tiff->nextifd_offset=TinyTIFFReader_readuint32(tiff);
        //printf("      - nextifd_offset=%lu\n", tiff->nextifd_offset);
    } else {
        tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
        strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "no more images in TIF file\0");
#else
        strcpy(tiff->lastError, "no more images in TIF file\0");
#endif
    }
}

int TinyTIFFReader_getSampleData(TinyTIFFReaderFile* tiff, void* buffer, uint16_t sample) {
    if (tiff) {
        if (tiff->currentFrame.compression!=TIFF_COMPRESSION_NONE) {
            tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
            strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "the compression of the file is not supported by this library\0");
#else
            strcpy(tiff->lastError, "the compression of the file is not supported by this library\0");
#endif
            return TINYTIFFREADER_FALSE;
        }
        if (tiff->currentFrame.samplesperpixel>1 && tiff->currentFrame.planarconfiguration!=TIFF_PLANARCONFIG_PLANAR) {
            tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
            strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "only planar TIFF files are supported by this library\0");
#else
            strcpy(tiff->lastError, "only planar TIFF files are supported by this library\0");
#endif
            return TINYTIFFREADER_FALSE;
        }
        if (tiff->currentFrame.width==0 || tiff->currentFrame.height==0 ) {
            tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
            strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "the current frame does not contain images\0");
#else
            strcpy(tiff->lastError, "the current frame does not contain images\0");
#endif
            return TINYTIFFREADER_FALSE;
        }
        if (tiff->currentFrame.bitspersample[sample]!=8 && tiff->currentFrame.bitspersample[sample]!=16 && tiff->currentFrame.bitspersample[sample]!=32) {
            tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
            strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "this library only support 8,16 and 32 bits per sample\0");
#else
            strcpy(tiff->lastError, "this library only support 8,16 and 32 bits per sample\0");
#endif
            return TINYTIFFREADER_FALSE;
        }
        TinyTIFFReader_POSTYPE pos;
        //fgetpos(tiff->file, &pos);
        TinyTIFFReader_fgetpos(tiff, &pos);
        tiff->wasError=TINYTIFFREADER_FALSE;

        //printf("    - stripcount=%u\n", tiff->currentFrame.stripcount);
        if (tiff->currentFrame.stripcount>0 && tiff->currentFrame.stripbytecounts && tiff->currentFrame.stripoffsets) {
            uint32_t s;
            //printf("    - bitspersample[sample]=%u\n", tiff->currentFrame.bitspersample[sample]);
            if (tiff->currentFrame.bitspersample[sample]==8) {
                for (s=0; s<tiff->currentFrame.stripcount; s++) {
                    //printf("      - s=%u: stripoffset=0x%X stripbytecounts=%u\n", s, tiff->currentFrame.stripoffsets[s], tiff->currentFrame.stripbytecounts[s]);
                    const size_t tmpsize=tiff->currentFrame.stripbytecounts[s];
                    uint8_t* tmp=(uint8_t*)calloc(tmpsize, sizeof(uint8_t));
                    TinyTIFFReader_fseek_set(tiff, tiff->currentFrame.stripoffsets[s]);
                    TinyTIFFReader_fread(tmp, tmpsize, tmpsize, 1, tiff);
                    uint32_t offset=s*tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    //printf("          bufferoffset=%u\n", offset);
                    memcpy(&(((uint8_t*)buffer)[offset]), tmp, tmpsize);
                    free(tmp);
                }
            } else if (tiff->currentFrame.bitspersample[sample]==16) {
                for (s=0; s<tiff->currentFrame.stripcount; s++) {
                    //printf("      - s=%u: stripoffset=0x%X stripbytecounts=%u\n", s, tiff->currentFrame.stripoffsets[s], tiff->currentFrame.stripbytecounts[s]);
                    const size_t tmpsize=tiff->currentFrame.stripbytecounts[s];
                    uint16_t* tmp=(uint16_t*)calloc(tmpsize, sizeof(uint8_t));
                    TinyTIFFReader_fseek_set(tiff, tiff->currentFrame.stripoffsets[s]);
                    TinyTIFFReader_fread(tmp, tmpsize, tiff->currentFrame.stripbytecounts[s], 1, tiff);
                    uint32_t offset=s*tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    uint32_t pixels=tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    uint32_t imagesize=tiff->currentFrame.width*tiff->currentFrame.height;
                    if (offset+pixels>imagesize) pixels=imagesize-offset;
                    uint32_t x;
                    if (tiff->systembyteorder==tiff->filebyteorder) {
                        memcpy(&(((uint16_t*)buffer)[offset]), tmp, tiff->currentFrame.stripbytecounts[s]);
                    } else {
                        for (x=0; x<pixels; x++) {
                            ((uint16_t*)buffer)[offset+x]=TinyTIFFReader_Byteswap16(tmp[x]);
                        }
                    }
                    free(tmp);
                }
            } else if (tiff->currentFrame.bitspersample[sample]==32) {
                for (s=0; s<tiff->currentFrame.stripcount; s++) {
                    //printf("      - s=%u: stripoffset=0x%X stripbytecounts=%u\n", s, tiff->currentFrame.stripoffsets[s], tiff->currentFrame.stripbytecounts[s]);
                    uint32_t* tmp=(uint32_t*)calloc(tiff->currentFrame.stripbytecounts[s], sizeof(uint8_t));
                    //fseek(tiff->file, tiff->currentFrame.stripoffsets[s], SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, tiff->currentFrame.stripoffsets[s]);
                    //fread(tmp, tiff->currentFrame.stripbytecounts[s], 1, tiff->file);
                    uint32_t offset=s*tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    //memcpy(&(((uint8_t*)buffer)[offset*2]), tmp, tiff->currentFrame.stripbytecounts[s]);
                    uint32_t pixels=tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    uint32_t imagesize=tiff->currentFrame.width*tiff->currentFrame.height;
                    if (offset+pixels>imagesize) pixels=imagesize-offset;
                    uint32_t x;
                    for (x=0; x<pixels; x++) {
                        ((uint32_t*)buffer)[offset+x]=TinyTIFFReader_readuint32(tiff);
                    }
                    free(tmp);
                }
            } else {
                tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
                strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "TINYTIFFReader does not support the bitsPerSample, given in teh file (only 8,16,32bit are supported)\0");
#else
                strcpy(tiff->lastError, "TINYTIFFReader does not support the bitsPerSample, given in teh file (only 8,16,32bit are supported)\0");
#endif
            }

        } else {
            tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
            strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "TIFF format not recognized\0");
#else
            strcpy(tiff->lastError, "TIFF format not recognized\0");
#endif
        }

        //fsetpos(tiff->file, &pos);
        TinyTIFFReader_fsetpos(tiff, &pos);
        return !tiff->wasError;
    }
    tiff->wasError=TINYTIFFREADER_TRUE;
#ifdef HAVE_STRCPY_S
    strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, "TIFF file not opened\0");
#else
    strcpy(tiff->lastError, "TIFF file not opened\0");
#endif
    return TINYTIFFREADER_FALSE;
}
















TinyTIFFReaderFile* TinyTIFFReader_open(const char* filename) {
    TinyTIFFReaderFile* tiff=(TinyTIFFReaderFile*)malloc(sizeof(TinyTIFFReaderFile));

    tiff->filesize=0;
    struct stat file;
    if(stat(filename,&file)==0) {
         tiff->filesize=file.st_size;
    }
    tiff->currentFrame=TinyTIFFReader_getEmptyFrame();


    //tiff->file=v(filename, "rb");
    TinyTIFFReader_fopen(tiff, filename);
    tiff->systembyteorder=TIFFReader_get_byteorder();
    memset(tiff->lastError, 0, TIFF_LAST_ERROR_SIZE);
    tiff->wasError=TINYTIFFREADER_FALSE;
    if (TinyTIFFReader_fOK(tiff) && tiff->filesize>0) {
        const size_t tiffidsize=3;
        uint8_t tiffid[3]={0,0,0};
        //fread(tiffid, 1,2,tiff->file);
        TinyTIFFReader_fread(tiffid, tiffidsize, 1,2,tiff);

        //printf("      - head=%s\n", tiffid);
        if (tiffid[0]=='I' && tiffid[1]=='I') tiff->filebyteorder=TIFF_ORDER_LITTLEENDIAN;
        else if (tiffid[0]=='M' && tiffid[1]=='M') tiff->filebyteorder=TIFF_ORDER_BIGENDIAN;
        else {
            free(tiff);
            return NULL;
        }
        uint16_t magic=TinyTIFFReader_readuint16(tiff);
        //printf("      - magic=%u\n", magic);
        if (magic!=42) {
            free(tiff);
            return NULL;
        }
        tiff->firstrecord_offset=TinyTIFFReader_readuint32(tiff);
        tiff->nextifd_offset=tiff->firstrecord_offset;
        //printf("      - filesize=%u\n", tiff->filesize);
        //printf("      - firstrecord_offset=%4X\n", tiff->firstrecord_offset);
        TinyTIFFReader_readNextFrame(tiff);
    } else {
        free(tiff);
        return NULL;
    }

    return tiff;
}

void TinyTIFFReader_close(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        TinyTIFFReader_freeEmptyFrame(tiff->currentFrame);
        //fclose(tiff->file);
        TinyTIFFReader_fclose(tiff);
        free(tiff);
    }
}

int TinyTIFFReader_hasNext(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        if (tiff->nextifd_offset>0 && tiff->nextifd_offset<tiff->filesize) return TINYTIFFREADER_TRUE;
        else return TINYTIFFREADER_FALSE;
    } else {
        return TINYTIFFREADER_FALSE;
    }
}

int TinyTIFFReader_readNext(TinyTIFFReaderFile* tiff) {
    int hasNext=TinyTIFFReader_hasNext(tiff);
    if (hasNext) {
        TinyTIFFReader_readNextFrame(tiff);
    }
    return hasNext;
}

uint32_t TinyTIFFReader_getWidth(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.width;
    }
    return 0;
}

uint32_t TinyTIFFReader_getHeight(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.height;
    }
    return 0;
}

const char* TinyTIFFReader_getImageDescription(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        if (tiff->currentFrame.description) return tiff->currentFrame.description;
    }
    static const char* nothing = "";
    return nothing;
}

uint16_t TinyTIFFReader_getSampleFormat(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.sampleformat;
    }
    return 0;
}

uint16_t TinyTIFFReader_getBitsPerSample(TinyTIFFReaderFile* tiff, int sample) {
    if (tiff) {
        return tiff->currentFrame.bitspersample[sample];
    }
    return 0;
}

uint16_t TinyTIFFReader_getSamplesPerPixel(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.samplesperpixel;
    }
    return 0;
}


uint32_t TinyTIFFReader_countFrames(TinyTIFFReaderFile* tiff) {

    if (tiff) {
        uint32_t frames=0;
        TinyTIFFReader_POSTYPE pos;
        //printf("    -> countFrames: pos before %ld\n", ftell(tiff->file));
        //fgetpos(tiff->file, &pos);
        TinyTIFFReader_fgetpos(tiff, &pos);

        uint32_t nextOffset=tiff->firstrecord_offset;
        while (nextOffset>0) {
            //fseek(tiff->file, nextOffset, SEEK_SET);
            TinyTIFFReader_fseek_set(tiff, nextOffset);
            uint16_t count=TinyTIFFReader_readuint16(tiff);
            //fseek(tiff->file, count*12, SEEK_CUR);
            TinyTIFFReader_fseek_cur(tiff, count*12);
            nextOffset=TinyTIFFReader_readuint32(tiff);
            frames++;
        }


        //fsetpos(tiff->file, &pos);
        TinyTIFFReader_fsetpos(tiff, &pos);
        //printf("    -> countFrames: pos after %ld\n", ftell(tiff->file));
        return frames;
    }
    return 0;
}
