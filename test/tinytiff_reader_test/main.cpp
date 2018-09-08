#ifdef TEST_LIBTIFF
#include <tiffio.h>
#include "../../libtiff_tools/libtiff_tools.h"
#endif
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../../tinytiffwriter.h"
#include "../../tinytiffreader.h"
#include "../../tinytiffhighrestimer.h"
//#include "../../tools.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdexcept>
#include <cctype>



 std::string tolower(const std::string& s){
  std::string d;
  d="";
  if (s.length()>0) {
    for (unsigned long i=0; i<s.length(); i++) {
        d+=std::tolower(s[i]);
    }
  }
  return d;
};

 bool strtobool(std::string data){
    std::string d=tolower(data);
  if (d=="true") return true;
  if (d=="t") return true;
  if (d=="1") return true;
  if (d=="j") return true;
  if (d=="y") return true;
  if (d=="yes") return true;
  if (d=="ja") return true;
  return false;
}
 std::string toupper(const std::string& s){
  std::string d;
  d="";
  if (s.length()>0) {
    for (unsigned long i=0; i<s.length(); i++) {
        d+=toupper(s[i]);
    }
  }
  return d;
};

 std::string format(const std::string& templ, ...){
  va_list ap;
  char buffer[4096];
  va_start (ap, templ);
  vsprintf (buffer, templ.c_str(), ap);
  va_end (ap);
  std::string ret(buffer);
  return ret;
};

 long get_filesize(char *FileName) {
    struct stat file;
    if(!stat(FileName,&file)) {
         return file.st_size;
    }
    return 0;
}
 std::string bytestostr(double bytes){
    double data=bytes;
  std::string form="%.0lf";
  std::string res=format(form,data);
  form="%.3lf";
  if (fabs(data)>=1024.0) res=format(form,data/1024.0)+" k";
  if (fabs(data)>=1024.0*1024.0) res=format(form,data/(1024.0*1024.0))+" M";
  if (fabs(data)>=1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0))+" ";
  if (fabs(data)>=1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0))+" G";
  if (fabs(data)>=1024.0*1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0*1024.0))+" T";
  if (fabs(data)>=1024.0*1024.0*1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0*1024.0*1024.0))+" E";
  if (fabs(data)==0) res="0 ";
  return res+"Bytes";
}

 std::string inttostr(long data){
  return format("%ld", data);
};

 std::string inttohex(long data){
  return format("%lX", data);
};

 std::string uinttostr(unsigned long data){
  std::ostringstream ost;
  ost<<data;
  return ost.str();
};

 std::string floattostr(double data, int past_comma=-1, bool remove_trail0=false, double belowIsZero=1e-16){
  if (belowIsZero>0) {
      if (fabs(data)<belowIsZero) return std::string("0");
  }

  std::string form="%."+inttostr(past_comma)+"lf";
  //std::string form="%lf";
  if (past_comma<=0) form="%lf";
  std::string r=format(form,data);
  //std::cout<<r<<std::endl;
  if (remove_trail0 && (tolower(r).find('e')==std::string::npos)) {
      if (data==0) return "0";
      //size_t cp=r.find(".");
      //if (cp<r.size()) return r;
      std::string re;
      size_t dpos=r.find('.');
      if (dpos==std::string::npos) {
          return r;
      } else {
          long i=r.size()-1;
          bool nonz=false;
          while (i>=0) {
              //std::cout<<i<<"\n";
              if (r[i]!='0') {
                  nonz=true;
              }
              if (nonz || (i<long(dpos))) {
                  if (re.size()==0 && r[i]=='.') {
                      // swallow decimal dot, if only 0 folowed
                  } else {
                      re=r[i]+re;
                  }
              }
              i--;
          }
          return re;
      }

  }
  return r;
}

 std::string floattounitstr(double dataa, std::string unitname){
  if (dataa==0) return floattostr(dataa)+unitname;
  std::string u="";
  double factor=1;
  double data=fabs(dataa);
  if (data>=1e3) { u="k"; factor=1e3; }
  if (data>=1e6) { u="M"; factor=1e6; }
  if (data>=1e9) { u="G"; factor=1e9; }
  if (data>=1e12) { u="T"; factor=1e12; }
  if (data>=1e15) { u="P"; factor=1e15; }
  if (data>=1e18) { u="E"; factor=1e18; }
  if (data<1) {u="m"; factor=1e-3; }
  if (data<1e-3) {u="u"; factor=1e-6; }
  if (data<1e-6) {u="n"; factor=1e-9; }
  if (data<1e-9) {u="p"; factor=1e-12; }
  if (data<1e-12) {u="f"; factor=1e-15; }
  if (data<1e-15) {u="a"; factor=1e-18; }

  return floattostr(dataa/factor)+u+unitname;
};

using namespace std;

#define WIDTH 64
#define HEIGHT 64
#define PATTERNSIZE 12
#define SPEEDTEST_SIZE 1000
#define SPEEDTEST_OUTPUT 10
#define TEST_FRAMES 100000

#ifdef TEST_LIBTIFF
#define SAVE_TIFF(filename, data, width,height) { \
    std::cout<<"SAVE_TIFF("<<filename<<")\n"; \
    TIFF* t=TIFFOpen(filename, "w"); \
    uint32_t* d=(uint32_t*)calloc(width*height,sizeof(uint32_t));\
    for (int kkt=0; kkt<width*height; kkt++) {\
        d[kkt]=data[kkt];\
    }\
    TIFFTWriteUint16from32(t, d, width, height);\
    TIFFClose(t); \
    free(d);\
}
#else
#define SAVE_TIFF(filename, data, width,height)
#endif

#define TEST_SIMPLE(filename, imagetype) \
    std::cout<<"\n\nreading '"<<filename<<"' and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n"; \
    ok=false;\
    tiffr=TinyTIFFReader_open(filename); \
    if (!tiffr) { \
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; \
    } else { \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n"; \
        timer.start(); \
        uint32_t frames=TinyTIFFReader_countFrames(tiffr); \
        double duration=timer.get_time(); \
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        timer.start(); \
        ok=true; \
        uint32_t frame=0; \
        do { \
            uint32_t width=TinyTIFFReader_getWidth(tiffr); \
            uint32_t height=TinyTIFFReader_getHeight(tiffr); \
            if (width>0 && height>0) std::cout<<"    size of frame "<<frame<<": "<<width<<"x"<<height<<"\n"; \
            else { std::cout<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height<<"\n"; ok=false; } \
            if (ok) { \
                frame++; \
                imagetype* tmp=(imagetype*)calloc(width*height, sizeof(imagetype));  \
                TinyTIFFReader_getSampleData(tiffr, tmp, 0); \
                if (TinyTIFFReader_wasError(tiffr)) { ok=false; std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; } \
                if (ok) { \
                    char fn[1024];\
                    sprintf(fn, "%s.%u.tif", filename, frame); \
                    SAVE_TIFF(fn, tmp, width, height); \
                }\
                free(tmp); \
            } \
        } while (ok && TinyTIFFReader_readNext(tiffr));\
        duration=timer.get_time(); \
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        std::cout<<"    read "<<frame<<" frames\n"; \
    } \
    TinyTIFFReader_close(tiffr); \
    if (ok) std::cout<<"  => SUCCESS\n"; \
    else std::cout<<"  => NOT CORRECTLY READ\n";



#define TEST(filename, image, imagei, imagetype) \
    std::cout<<"\n\nreading '"<<filename<<"' and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n"; \
    ok=false;\
    tiffr=TinyTIFFReader_open(filename); \
    if (!tiffr) { \
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; \
    } else { \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n"; \
        timer.start(); \
        uint32_t frames=TinyTIFFReader_countFrames(tiffr); \
        double duration=timer.get_time(); \
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        timer.start(); \
        ok=true; \
        uint32_t frame=0; \
        double duration_getdata=0, duration_nextframe=0; \
        bool next; \
        do { \
            uint32_t width=TinyTIFFReader_getWidth(tiffr); \
            uint32_t height=TinyTIFFReader_getHeight(tiffr); \
            ok=(width==WIDTH)&&(height==HEIGHT); \
            if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": size does not match, read "<<width<<"x"<<height<<"    expected "<<WIDTH<<"x"<<HEIGHT<<"\n";; \
            if (ok) { \
                imagetype* tmp=(imagetype*)calloc(width*height, TinyTIFFReader_getBitsPerSample(tiffr)/8);  \
                timer1.start(); \
                TinyTIFFReader_getSampleData(tiffr, tmp, 0); \
                duration_getdata+=timer1.get_time(); \
                if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
                uint32_t poserror=0xFFFFFFFF; \
                if (frame%2==0) {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==image[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                } else {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==imagei[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                }\
                free(tmp); \
                if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": did not read correct contents @ pos="<<poserror<<"\n";; \
            } \
            if (ok) { \
                frame++; \
            } \
            timer1.start(); \
            next=TinyTIFFReader_readNext(tiffr);\
            duration_nextframe+=timer1.get_time(); \
        } while (ok && next);\
        duration=timer.get_time(); \
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        std::cout<<"    getSampleData() consumed "<<floattounitstr(duration_getdata/1.0e6, "s")<<"\n"; \
        std::cout<<"    readNext() consumed "<<floattounitstr(duration_nextframe/1.0e6, "s")<<"\n"; \
        std::cout<<"    read "<<frame<<" frames\n"; \
    } \
    TinyTIFFReader_close(tiffr); \
    if (ok) std::cout<<"  => SUCCESS\n"; \
    else std::cout<<"  => NOT CORRECTLY READ\n";



#define TEST_LIBTIFF_FUNC(filename, image, imagei, imagetype) \
    std::cout<<"\n\nreading '"<<filename<<"' with libtiff and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n"; \
    ok=false;\
    timer.start(); \
    std::cout<<"    opening file     [duration: "<<floattounitstr(double(timer.get_time())/1.0e6, "s")<<" ]\n"; \
    ltiff=TIFFOpen(filename, "r"); \
    if (!ltiff) { \
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; \
    } else { \
        timer.start(); \
        uint32_t frames=TIFFCountDirectories(ltiff); \
        double duration=timer.get_time(); \
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        timer.start(); \
        ok=true; \
        uint32_t frame=0; \
        do { \
            uint32_t width; \
            uint32_t height; \
            uint32_t bitspersample; \
            TIFFGetField(ltiff,TIFFTAG_BITSPERSAMPLE,&bitspersample);\
            TIFFGetField(ltiff,TIFFTAG_IMAGEWIDTH,&width); \
            TIFFGetField(ltiff,TIFFTAG_IMAGELENGTH,&height); \
			char*val; \
			TIFFGetField(ltiff,TIFFTAG_IMAGEDESCRIPTION,&val); \
			std::cout<<"    ImageDescription:\n"<<val<<"\n"; \
            ok=(width==WIDTH)&&(height==HEIGHT); \
            if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": size does not match, read "<<width<<"x"<<height<<"    expected "<<WIDTH<<"x"<<HEIGHT<<"\n";; \
            if (ok && image && imagei) { \
                imagetype* tmp=(imagetype*)calloc(width*height, sizeof(imagetype));   \
                TIFFReadFrame(ltiff, tmp); \
                uint32_t poserror=0xFFFFFFFF; \
                if (frame%2==0) {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==image[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                } else {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==imagei[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                }\
                free(tmp); \
                if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": did not read correct contents @ pos="<<poserror<<"\n";; \
            } \
            if (ok) { \
                frame++; \
            } \
        } while (ok && TIFFReadDirectory(ltiff));\
        duration=timer.get_time(); \
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        std::cout<<"    read "<<frame<<" frames\n"; \
    } \
    TIFFClose(ltiff); \
    if (ok) std::cout<<"  => SUCCESS\n"; \
    else std::cout<<"  => NOT CORRECTLY READ\n";


int main() {
    std::cout<<"creating some test TIFF files ...";

    HighResTimer timer, timer1;


    uint8_t* image8=(uint8_t*)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    uint8_t* image8i=(uint8_t*)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    uint16_t* image16=(uint16_t*)calloc(WIDTH*HEIGHT, sizeof(uint16_t));
    uint16_t* image16i=(uint16_t*)calloc(WIDTH*HEIGHT, sizeof(uint16_t));

    int i=0;
    for (int x=0; x<WIDTH; x++) {
        for (int y=0; y<HEIGHT; y++) {
            if (x==y) image8[i]=254;
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) image8[i]=200;
            else if (x%PATTERNSIZE==0) image8[i]=75;
            else if (y%PATTERNSIZE==0) image8[i]=150;
            image8i[i]=255-image8[i];
            image16[i]=(255-image8[i])*255;
            image16i[i]=0xFFFF-image16[i];
            i++;
        }
    }

    TinyTIFFFile* tiff = TinyTIFFWriter_open("test8.tif", 8, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image8);
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("test8m.tif", 8, WIDTH,HEIGHT);
    for (int i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image8);
        TinyTIFFWriter_writeImage(tiff, image8i);
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("test16.tif", 16, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image16);
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("test16m.tif", 16, WIDTH,HEIGHT);
    for (int i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image16);
        TinyTIFFWriter_writeImage(tiff, image16i);
    }
    TinyTIFFWriter_close(tiff);
    std::cout<<" DONE!\n";

    bool ok=false;
    TinyTIFFReaderFile* tiffr=NULL;

    TEST_SIMPLE("cell.tif", uint8_t)
    TEST_SIMPLE("circuit.tif", uint8_t)
    TEST_SIMPLE("galaxy.tif", uint8_t)
    //TEST_SIMPLE("galaxy_nocompression.tif", uint8_t)
    TEST_SIMPLE("mri.tif", uint8_t)
    TEST_SIMPLE("multi-channel-time-series.ome.tif", uint8_t)


    TEST("test8.tif", image8, image8i, uint8_t)
    TEST("test8m.tif", image8, image8i, uint8_t)
    TEST("test16.tif", image16, image16i, uint16_t)
    TEST("test16m.tif", image16, image16i, uint16_t)

#ifdef TEST_LIBTIFF
    if (TEST_FRAMES<60000) {
        TIFF* ltiff=NULL;
        TEST_LIBTIFF_FUNC("test8.tif", image8, image8i, uint8_t)
        TEST_LIBTIFF_FUNC("test8m.tif", image8, image8i, uint8_t)
        TEST_LIBTIFF_FUNC("test16.tif", image16, image16i, uint16_t)
        TEST_LIBTIFF_FUNC("test16m.tif", image16, image16i, uint16_t)
        TEST_LIBTIFF_FUNC("test16m_imagej.tif", image16, image16i, uint16_t)
    }
	{
		TIFF* ltiff=NULL;
		char* nval=NULL;
		TEST_LIBTIFF_FUNC("multi-channel-time-series.ome.tif", nval, nval, uint16_t)
		TEST_SIMPLE("multi-channel-time-series.ome.tif", uint8_t)
	}
#endif

    free(image8);
    free(image16);
    free(image8i);
    free(image16i);
    return 0;
}
