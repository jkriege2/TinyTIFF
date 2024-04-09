#ifdef TINYTIFF_TEST_LIBTIFF
#include <tiffio.h>
#include "libtiff_tools.h"
#endif
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include "tinytiffwriter.h"
#include "tinytiffreader.h"
#include "tinytiffhighrestimer.h"
#include "test_results.h"
#include "testimage_tools.h"
#include <sys/types.h>
#include <sys/stat.h>
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
#include <array>
#include "tinytiff_tools.hxx"



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

// save data (size=width*height*sizeof(TDATA)) into a file \a filename
template<class TDATA>
void SAVE_TIFF(const char* filename, const TDATA* data, size_t width, size_t height) {
    TinyTIFFWriterFile* tiff = TinyTIFFWriter_open(filename, sizeof(TDATA)*8, TinyTIFF_SampleFormatFromType<TDATA>().format, 1, width, height, TinyTIFFWriter_Greyscale);
    TinyTIFFWriter_writeImage(tiff, data);
    TinyTIFFWriter_close(tiff);
}

// save data (size=width*height*sizeof(TDATA)) into a file \a filename
template<class TDATA>
void SAVE_TIFF_libtiff(const char* filename, const TDATA* data, size_t width, size_t height, bool little_endian=true) {
#ifdef TINYTIFF_TEST_LIBTIFF
    TIFF* tifvideo;
    if (little_endian) {
        tifvideo=TIFFOpen(filename, "wl");
    } else {
        tifvideo=TIFFOpen(filename, "wb");
    }
    if (tifvideo) {
        TIFFWrite<TDATA>(tifvideo, data, width, height);
        TIFFWriteDirectory(tifvideo);
        TIFFClose(tifvideo);
    }
#endif
}


// try to open a TIFF file with TInyTIFFReader, if read successfully, the read frames are stored using SAVE_TIFF,
// does not check the contents for correctness!
template<class TIMAGESAMPLETYPE>
void TEST_SIMPLE(const char* filename, std::vector<TestResult>& test_results) {
    HighResTimer timer;
    bool ok=false;
    test_results.emplace_back();
    test_results.back().name=std::string("TEST_SIMPLE(")+std::string(filename)+std::string(")");
    std::cout<<"\n\nreading '"<<std::string(filename)<<"' and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n";
    test_results.back().success=ok=false;
    TinyTIFFReaderFile* tiffr=TinyTIFFReader_open(filename);
    if (!tiffr) {
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n";
    } else {
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n";
        timer.start();
        uint32_t frames=TinyTIFFReader_countFrames(tiffr);
        double duration=timer.get_time();
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
        timer.start();
        test_results.back().success=ok=true;
        uint32_t frame=0;
        do {
            uint32_t width=TinyTIFFReader_getWidth(tiffr);
            uint32_t height=TinyTIFFReader_getHeight(tiffr);
            if (width>0 && height>0) std::cout<<"    size of frame "<<frame<<": "<<width<<"x"<<height<<"\n";
            else { std::cout<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height<<"\n"; test_results.back().success=ok=false; }
            if (ok) {
                frame++;
                TIMAGESAMPLETYPE* tmp=(TIMAGESAMPLETYPE*)calloc(width*height, sizeof(TIMAGESAMPLETYPE));
                TinyTIFFReader_getSampleData(tiffr, tmp, 0);
                if (TinyTIFFReader_wasError(tiffr)) { test_results.back().success=ok=false; std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; }
                if (ok) {
                    char fn[1024];
                    sprintf(fn, "%s.%u.tif", filename, frame);
                    SAVE_TIFF(fn, tmp, width, height);
                }
                free(tmp);
            }
        } while (ok && TinyTIFFReader_readNext(tiffr));
        duration=timer.get_time();
        test_results.back().duration_ms=duration/1.0e3;
        test_results.back().numImages=frame;
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
        std::cout<<"    read "<<frame<<" frames\n";
    }
    TinyTIFFReader_close(tiffr);
    test_results.back().success=ok;
    if (ok) std::cout<<"  => SUCCESS\n";
    else std::cout<<"  => NOT CORRECTLY READ\n";
}


// try to read the data in the TIFF file \a filename with TinyTIFFReader, compare the read data to the data in image and imagei, the file is expected to contain either
// a single frame of contents \a image, or a series of frames alternativ between image and imagei (i.e.  image,imagei,image,imagei,image,...)
template<class TIMAGESAMPLETYPE>
void TEST(const char* filename, const TIMAGESAMPLETYPE* image, const TIMAGESAMPLETYPE* imagei,size_t WIDTH_IN, size_t HEIGHT_IN, size_t SAMPLES_IN, size_t FRAMES_IN, std::vector<TestResult>& test_results) {
    HighResTimer timer, timer1;
    bool ok=false;
    std::cout<<"\n\nreading '"<<std::string(filename)<<"' and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n";
    test_results.emplace_back();
    const std::string desc=std::to_string(WIDTH_IN)+"x"+std::to_string(HEIGHT_IN)+"pix/"+std::to_string(sizeof(TIMAGESAMPLETYPE)*8)+"bit/"+std::to_string(SAMPLES_IN)+"ch/"+std::to_string(FRAMES_IN)+"frames";
    test_results.back().name=std::string("TEST(")+desc+", "+std::string(filename)+std::string(")");
    test_results.back().success=ok=false;
    try {
        TinyTIFFReaderFile* tiffr=TinyTIFFReader_open(filename);
        if (!tiffr) {
            std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n";
        } else {
            if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
            const char* desc=TinyTIFFReader_getImageDescription(tiffr);
            if (desc!=NULL && strlen(desc)>0) {
                std::cout<<"    ImageDescription:\n"<< desc <<"\n";
            } else {
                std::cout<<"    ImageDescription: EMPTY\n";
            }
            timer.start();
            uint32_t frames=TinyTIFFReader_countFrames(tiffr);
            double duration=timer.get_time();
            std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
            if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
            timer.start();
            test_results.back().success=ok=true;
            uint32_t frame=0;
            double duration_getdata=0, duration_nextframe=0;
            bool next;
            do {
                uint32_t width=TinyTIFFReader_getWidth(tiffr);
                uint32_t height=TinyTIFFReader_getHeight(tiffr);
                uint32_t samples=TinyTIFFReader_getSamplesPerPixel(tiffr);
                test_results.back().success=ok=(width==WIDTH_IN)&&(height==HEIGHT_IN)&&(samples==SAMPLES_IN);
                if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": size does not match, read "<<width<<"x"<<height<<"x"<<samples<<"    expected "<<WIDTH_IN<<"x"<<HEIGHT_IN<<"x"<<SAMPLES_IN<<"\n";;
                if (ok) {
                    for (size_t sample=0; sample<samples; sample++) {
                        TIMAGESAMPLETYPE* tmp=(TIMAGESAMPLETYPE*)calloc(width*height, TinyTIFFReader_getBitsPerSample(tiffr, sample)/8);
                        timer1.start();
                        TinyTIFFReader_getSampleData(tiffr, tmp, sample);
                        duration_getdata+=timer1.get_time();
                        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
                        uint32_t poserror=0xFFFFFFFF;
                        if (frame%2==0) {
                            for (uint32_t i=0; i<width*height; i++) {
                                test_results.back().success=ok=ok&&(tmp[i]==image[i*SAMPLES_IN+sample]);
                                if (!ok) { poserror=i; break; }
                            }
                        } else {
                            for (uint32_t i=0; i<width*height; i++) {
                                test_results.back().success=ok=ok&&(tmp[i]==imagei[i*SAMPLES_IN+sample]);
                                if (!ok) { poserror=i; break; }
                            }
                        }
                        free(tmp);
                        if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<"/ SAMPLE "<<sample<<": did not read correct contents @ pos="<<poserror<<"\n";;
                    }
                }
                if (ok) {
                    frame++;
                }
                timer1.start();
                next=TinyTIFFReader_readNext(tiffr);
                duration_nextframe+=timer1.get_time();
            } while (ok && next);
            duration=timer.get_time();
            test_results.back().duration_ms=duration/1.0e3;
            test_results.back().numImages=frame;
            std::cout<<"    read and checked "<<frame<<" frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
            std::cout<<"    getSampleData() consumed "<<floattounitstr(duration_getdata/1.0e6, "s")<<"\n";
            std::cout<<"    readNext() consumed "<<floattounitstr(duration_nextframe/1.0e6, "s")<<"\n";
            std::cout<<"    read "<<frame<<" frames\n";

            if (frame!=FRAMES_IN) {
                ok=false;
                std::cout<<"    ERROR: not enough frames red: expected: "<<FRAMES_IN<<", found: "<<frame<<"\n";
            }
        }
        TinyTIFFReader_close(tiffr);
    } catch(...) {
        ok=false;
        std::cout<<"       CRASH While reading file\n";
    }

    test_results.back().success=ok;
    if (ok) std::cout<<"  => SUCCESS\n";
    else std::cout<<"  => NOT CORRECTLY READ\n";
}


// try to read the data in the TIFF file \a filename with TinyTIFFReader and LIBTIFF and compare the result of the two
template<class TIMAGESAMPLETYPE>
void TEST_AGAINST_LIBTIFF(const char* filename, std::vector<TestResult>& test_results) {
#ifdef TINYTIFF_TEST_LIBTIFF
    HighResTimer timer, timer1;
    bool ok=false;
    std::cout<<"\n\nreading '"<<std::string(filename)<<"' with tinytiff and libtiff and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n";
    test_results.emplace_back();
    test_results.back().name=std::string("TEST_AGAINST_LIBTIFF(")+std::string(filename)+std::string(")");
    test_results.back().success=ok=false;
    test_results.back().numImages=0;
    try {
        timer.start();
        std::cout<<"    libTIFF: opening file with     [duration: "<<floattounitstr(double(timer.get_time())/1.0e6, "s")<<" ]\n";
        TIFF* ltiff=TIFFOpen(filename, "r");
        if (!ltiff) {
            std::cout<<"    libTIFF: ERROR reading (not existent, not accessible or no TIFF file)\n";
        } else {
            timer.start();
            uint32_t libTIFF_frames=TIFFCountDirectories(ltiff);
            double duration=timer.get_time();
            std::cout<<"    libTIFF: frames: "<<libTIFF_frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
            std::cout<<"    TinyTIFF: opening file with     [duration: "<<floattounitstr(double(timer.get_time())/1.0e6, "s")<<" ]\n";
            TinyTIFFReaderFile* tiffr=TinyTIFFReader_open(filename);
            if (!tiffr) {
                std::cout<<"    TinyTIFF: ERROR reading (not existent, not accessible or no TIFF file)\n";
                ok=false;
            } else if (TinyTIFFReader_wasError(tiffr)) {
                std::cout<<"   TinyTIFF: ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
                ok=false;
            } else {
                timer.start();
                uint32_t tinyTIFF_frames=TinyTIFFReader_countFrames(tiffr);
                duration=timer.get_time();
                std::cout<<"    TinyTIFF: frames: "<<tinyTIFF_frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
                if (TinyTIFFReader_wasError(tiffr)) {
                    std::cout<<"   TinyTIFF: ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
                    ok=false;
                } else {
                    if (libTIFF_frames!=tinyTIFF_frames) {
                        std::cout<<"    ERROR number of frames, read with TinyTIFF ("<<tinyTIFF_frames<<") and libTIFF ("<<libTIFF_frames<<") are not equal!\n";
                        ok=false;
                    } else {
                        timer.start();
                        size_t frame=0;
                        ok=true;
                        do {
                            std::cout<<"   frame: "<<frame<<std::endl;
                            uint32_t tinyTIFF_width=TinyTIFFReader_getWidth(tiffr);
                            uint32_t tinyTIFF_height=TinyTIFFReader_getHeight(tiffr);
                            uint16_t tinyTIFF_bitspersample=TinyTIFFReader_getBitsPerSample(tiffr, 0);
                            uint16_t tinyTIFF_samplesperpixel=TinyTIFFReader_getSamplesPerPixel(tiffr);
                            const char* tinyTIFF_imagedesc=TinyTIFFReader_getImageDescription(tiffr);
                            std::cout<<"     tinyTIFF: width="<<tinyTIFF_width<<", height="<<tinyTIFF_height<<", bitspersample="<<tinyTIFF_bitspersample<<", samplesperpixel="<<tinyTIFF_samplesperpixel<<"\n";
                            if (frame==0) {
                                const std::string desc=std::to_string(tinyTIFF_width)+"x"+std::to_string(tinyTIFF_height)+"pix/"+std::to_string(sizeof(TIMAGESAMPLETYPE)*8)+"bit/"+std::to_string(tinyTIFF_samplesperpixel)+"ch/"+std::to_string(tinyTIFF_frames)+"frames";
                                test_results.back().name=std::string("TEST_AGAINST_LIBTIFF(")+desc+", "+std::string(filename)+std::string(")");
                            }

                            uint32_t libTIFF_width=0;
                            uint32_t libTIFF_height=0;
                            uint32_t libTIFF_bitspersample=0;
                            uint32_t libTIFF_samplesperpixel=0;
                            char*libtiff_imagedesc;
                            TIFFGetField(ltiff,TIFFTAG_SAMPLESPERPIXEL,&libTIFF_samplesperpixel);
                            TIFFGetField(ltiff,TIFFTAG_BITSPERSAMPLE,&libTIFF_bitspersample);
                            TIFFGetField(ltiff,TIFFTAG_IMAGEWIDTH,&libTIFF_width);
                            TIFFGetField(ltiff,TIFFTAG_IMAGELENGTH,&libTIFF_height);
                            TIFFGetField(ltiff,TIFFTAG_IMAGEDESCRIPTION,&libtiff_imagedesc);
                            TIFFPrintDirectory(ltiff, stdout,TIFFPRINT_STRIPS|TIFFPRINT_COLORMAP);
                            std::cout<<"     libTIFF:  width="<<libTIFF_width<<", height="<<libTIFF_height<<", bitspersample="<<libTIFF_bitspersample<<", samplesperpixel="<<libTIFF_samplesperpixel<<"\n";

                            if (tinyTIFF_width!=libTIFF_width) {
                                std::cout<<"       ERROR in frame "<<frame<<": TinyTIFF and libTIFF read different widths (TinyTIFF: "<<tinyTIFF_width<<" != libTIFF: "<<libTIFF_width<<")\n";
                                ok=false;
                                break;
                            }
                            if (tinyTIFF_height!=libTIFF_height) {
                                std::cout<<"       ERROR in frame "<<frame<<": TinyTIFF and libTIFF read different heights (TinyTIFF: "<<tinyTIFF_height<<" != libTIFF: "<<libTIFF_height<<")\n";
                                ok=false;
                                break;
                            }
                            if (tinyTIFF_bitspersample!=libTIFF_bitspersample) {
                                std::cout<<"       ERROR in frame "<<frame<<": TinyTIFF and libTIFF read different bitspersamples (TinyTIFF: "<<tinyTIFF_bitspersample<<" != libTIFF: "<<libTIFF_bitspersample<<")\n";
                                ok=false;
                                break;
                            }
                            if (tinyTIFF_samplesperpixel!=libTIFF_samplesperpixel) {
                                std::cout<<"       ERROR in frame "<<frame<<": TinyTIFF and libTIFF read different samplesperpixels (TinyTIFF: "<<tinyTIFF_samplesperpixel<<" != libTIFF: "<<libTIFF_samplesperpixel<<")\n";
                                ok=false;
                                break;
                            }
                            if (tinyTIFF_imagedesc==nullptr || libtiff_imagedesc==nullptr || strcmp(tinyTIFF_imagedesc, libtiff_imagedesc)!=0) {
                                std::cout<<"       WARNING in frame "<<frame<<": TinyTIFF and libTIFF read different descriptions (TinyTIFF: '"<<tinyTIFF_imagedesc<<"' != libTIFF: '"<<libtiff_imagedesc<<"')\n";
                                //ok=false;
                                //break;
                            }
                            if (ok) {
                                for (size_t sample=0; sample<libTIFF_samplesperpixel; sample++) {
                                    std::vector<TIMAGESAMPLETYPE> tinyTIFF_data(tinyTIFF_width*tinyTIFF_height*sizeof(TIMAGESAMPLETYPE)*2, 0);
                                    TinyTIFFReader_getSampleData(tiffr,tinyTIFF_data.data(), sample);
                                    if (TinyTIFFReader_wasError(tiffr)) {
                                        std::cout<<"       TinyTIFF: ERROR reading frame "<<frame<<", sample "<<sample<<":"<<TinyTIFFReader_getLastError(tiffr)<<"\n";
                                        ok=false;
                                        break;
                                    }
                                    std::vector<TIMAGESAMPLETYPE> libTIFF_data(libTIFF_width*libTIFF_height*sizeof(TIMAGESAMPLETYPE)*2, 0);
                                    if (!TIFFReadFrame(ltiff, libTIFF_data.data(), sample)) {
                                        std::cout<<"       libTIFF: ERROR reading frame "<<frame<<", sample "<<sample<<"\n";
                                        ok=false;
                                        break;
                                    }
                                    for (size_t i=0; i<libTIFF_data.size(); i++) {
                                        if (i<=8) std::cout<<"          f"<<frame<<"s"<<sample<<"i"<<i<<": tiny="<<std::dec<<static_cast<typename atleast_int<TIMAGESAMPLETYPE>::TPrint>(tinyTIFF_data[i])<<" lib="<<static_cast<typename atleast_int<TIMAGESAMPLETYPE>::TPrint>(libTIFF_data[i])<<"\n";
                                        if (libTIFF_data[i]!=tinyTIFF_data[i]) {
                                            std::cout<<"       ERROR in frame "<<frame<<", sample "<<sample<<": TinyTIFF and libTIFF read different sample values (I="<<i<<": TinyTIFF: "<<std::dec<<static_cast<typename atleast_int<TIMAGESAMPLETYPE>::TPrint>(tinyTIFF_data[i])<<" != libTIFF: "<<std::dec<<static_cast<typename atleast_int<TIMAGESAMPLETYPE>::TPrint>(libTIFF_data[i])<<")\n";
                                            ok=false;
                                            break;
                                        }
                                    }
                                    if (!ok) {
                                        SAVE_TIFF(std::string(std::string(filename)+".tinytiff.error_sample"+std::to_string(sample)+".tif").c_str(), tinyTIFF_data.data(), tinyTIFF_width, tinyTIFF_height);
                                        SAVE_TIFF(std::string(std::string(filename)+".libtiff.error_sample"+std::to_string(sample)+".tif").c_str(), libTIFF_data.data(), tinyTIFF_width, tinyTIFF_height);
                                    }
                                }
                            }


                            frame++;
                        } while (ok && TIFFReadDirectory(ltiff) && TinyTIFFReader_readNext(tiffr));

                        test_results.back().success=ok;
                        test_results.back().numImages=frame;
                        duration=timer.get_time();
                        test_results.back().duration_ms=duration/1.0e3;
                        std::cout<<"    read and checked all "<<frame<<" frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n";
                    }
                }
            }
        }
    } catch(...) {
        ok=false;
        std::cout<<"       CRASH While reading file\n";
    }

    test_results.back().success=ok;
    if (ok) std::cout<<"  => SUCCESS\n";
    else std::cout<<"  => NOT CORRECTLY READ\n";
#endif
}



#ifdef TINYTIFF_TEST_LIBTIFF
static void errorhandler(const char* module, const char* fmt, va_list ap)
{
    static std::array<char, 1024> errorbuffer;
    vsnprintf(errorbuffer.data(), errorbuffer.size(), fmt, ap);

    std::cout<<"###LIBTIFF-ERROR: "<<module<<": "<<errorbuffer.data()<<"\n";
}
static void warninghandler(const char* module, const char* fmt, va_list ap)
{
    static std::array<char, 1024> errorbuffer;
    vsnprintf(errorbuffer.data(), errorbuffer.size(), fmt, ap);

    std::cout<<"###LIBTIFF-WARNING: "<<module<<": "<<errorbuffer.data()<<"\n";
}
#endif



int main(int argc, char *argv[]) {
    int quicktest=TINYTIFF_FALSE;
    if (argc>1 && std::string(argv[1])=="--simple")  quicktest=TINYTIFF_TRUE;

    std::cout<<"tinytiffreader_test:"<<std::endl;
    if (quicktest!=TINYTIFF_FALSE) std::cout<<"  - quick test with --simple"<<std::endl;
#ifdef TINYTIFF_TEST_LIBTIFF
    std::cout<<"  - checking against LibTIFF "<<TIFFGetVersion()<<std::endl;
    TIFFSetErrorHandler(errorhandler);
    TIFFSetWarningHandler(warninghandler);
#endif
    std::cout<<"creating some test TIFF files ..."<<std::endl;

    HighResTimer timer, timer1;

    const size_t WIDTH=(quicktest!=TINYTIFF_FALSE)?32:513;
    const size_t HEIGHT=(quicktest!=TINYTIFF_FALSE)?32:333;
    const size_t PATTERNSIZE=12;
    const size_t TEST_FRAMES=(quicktest!=TINYTIFF_FALSE)?6:100;


    vector<uint8_t> image8(WIDTH*HEIGHT, 0);
    vector<uint8_t> image8i(WIDTH*HEIGHT, 0);
    vector<uint16_t> image16(WIDTH*HEIGHT, 0);
    vector<uint16_t> image16i(WIDTH*HEIGHT, 0);
    vector<uint32_t> image32(WIDTH*HEIGHT, 0);
    vector<uint32_t> image32i(WIDTH*HEIGHT, 0);
    vector<uint64_t> image64(WIDTH*HEIGHT, 0);
    vector<uint64_t> image64i(WIDTH*HEIGHT, 0);
    vector<float> imagef(WIDTH*HEIGHT, 0);
    vector<float> imagefi(WIDTH*HEIGHT, 0);
    vector<double> imaged(WIDTH*HEIGHT, 0);
    vector<double> imagedi(WIDTH*HEIGHT, 0);
    vector<uint8_t> imagergb(WIDTH*HEIGHT*3, 0);
    vector<uint8_t> imagergbi(WIDTH*HEIGHT*3, 0);

    write1ChannelTestData(image8.data(), WIDTH, HEIGHT, PATTERNSIZE);
    write1ChannelTestData(image8i.data(), WIDTH, HEIGHT, PATTERNSIZE);
    invertTestImage(image8i.data(), WIDTH, HEIGHT);
    write1ChannelTestData(image16.data(), WIDTH, HEIGHT, PATTERNSIZE);
    write1ChannelTestData(image16i.data(), WIDTH, HEIGHT, PATTERNSIZE);
    invertTestImage(image16i.data(), WIDTH, HEIGHT);
    write1ChannelTestData(image32.data(), WIDTH, HEIGHT, PATTERNSIZE);
    write1ChannelTestData(image32i.data(), WIDTH, HEIGHT, PATTERNSIZE);
    invertTestImage(image32i.data(), WIDTH, HEIGHT);
    write1ChannelTestData(image64.data(), WIDTH, HEIGHT, PATTERNSIZE);
    write1ChannelTestData(image64i.data(), WIDTH, HEIGHT, PATTERNSIZE);
    invertTestImage(image64i.data(), WIDTH, HEIGHT);
    write1ChannelFloatTestData(imagef.data(), WIDTH, HEIGHT, PATTERNSIZE);
    write1ChannelFloatTestData(imagefi.data(), WIDTH, HEIGHT, PATTERNSIZE);
    invertFloatTestImage(imagefi.data(), WIDTH, HEIGHT);
    write1ChannelFloatTestData(imaged.data(), WIDTH, HEIGHT, PATTERNSIZE);
    write1ChannelFloatTestData(imagedi.data(), WIDTH, HEIGHT, PATTERNSIZE);
    invertFloatTestImage(imagedi.data(), WIDTH, HEIGHT);
    writeRGBTestDataChunky(imagergb.data(), WIDTH, HEIGHT, PATTERNSIZE,3);
    writeRGBTestDataChunky(imagergbi.data(), WIDTH, HEIGHT, PATTERNSIZE,3);
    invertTestImage(imagergbi.data(), WIDTH, HEIGHT,3);

    TinyTIFFWriterFile* tiff = TinyTIFFWriter_open("test8.tif", 8, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImage(tiff, image8.data());
    TinyTIFFWriter_close(tiff);
#ifdef TINYTIFF_TEST_LIBTIFF
    SAVE_TIFF_libtiff("test8_littleendian.tif", image8.data(), WIDTH, HEIGHT, true);
    SAVE_TIFF_libtiff("test8_bigendian.tif", image8.data(), WIDTH, HEIGHT, false);
#endif
    tiff = TinyTIFFWriter_open("test8m.tif", 8, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image8.data());
        TinyTIFFWriter_writeImage(tiff, image8i.data());
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("test16.tif", 16, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImage(tiff, image16.data());
    TinyTIFFWriter_close(tiff);
#ifdef TINYTIFF_TEST_LIBTIFF
    SAVE_TIFF_libtiff("test16_littleendian.tif", image16.data(), WIDTH, HEIGHT, true);
    SAVE_TIFF_libtiff("test16_bigendian.tif", image16.data(), WIDTH, HEIGHT, false);
#endif
    tiff = TinyTIFFWriter_open("test16m.tif", 16, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image16.data());
        TinyTIFFWriter_writeImage(tiff, image16i.data());
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("test32.tif", 32, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImage(tiff, image32.data());
    TinyTIFFWriter_close(tiff);
#ifdef TINYTIFF_TEST_LIBTIFF
    SAVE_TIFF_libtiff("test32_littleendian.tif", image32.data(), WIDTH, HEIGHT, true);
    SAVE_TIFF_libtiff("test32_bigendian.tif", image32.data(), WIDTH, HEIGHT, false);
#endif
    tiff = TinyTIFFWriter_open("test32m.tif", 32, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image32.data());
        TinyTIFFWriter_writeImage(tiff, image32i.data());
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("test64.tif", 64, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImage(tiff, image64.data());
    TinyTIFFWriter_close(tiff);
#ifdef TINYTIFF_TEST_LIBTIFF
    SAVE_TIFF_libtiff("test64_littleendian.tif", image64.data(), WIDTH, HEIGHT, true);
    SAVE_TIFF_libtiff("test64_bigendian.tif", image64.data(), WIDTH, HEIGHT, false);
#endif
    tiff = TinyTIFFWriter_open("test64m.tif", 64, TinyTIFFWriter_UInt, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image64.data());
        TinyTIFFWriter_writeImage(tiff, image64i.data());
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("testf.tif", 32, TinyTIFFWriter_Float, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImage(tiff, imagef.data());
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("testfm.tif", 32, TinyTIFFWriter_Float, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, imagef.data());
        TinyTIFFWriter_writeImage(tiff, imagefi.data());
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("testd.tif", 64, TinyTIFFWriter_Float, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImage(tiff, imaged.data());
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("testdm.tif", 64, TinyTIFFWriter_Float, 1, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, imaged.data());
        TinyTIFFWriter_writeImage(tiff, imagedi.data());
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("testrgb.tif", 8, TinyTIFFWriter_UInt, 3, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    TinyTIFFWriter_writeImagePlanarReorder(tiff, imagergb.data());
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("testrgbm.tif", 8, TinyTIFFWriter_UInt, 3, WIDTH,HEIGHT, TinyTIFFWriter_AutodetectSampleInterpetation);
    for (size_t i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImagePlanarReorder(tiff, imagergb.data());
        TinyTIFFWriter_writeImagePlanarReorder(tiff, imagergbi.data());
    }
    TinyTIFFWriter_close(tiff);

    std::cout<<" DONE!\n";

    std::vector<TestResult> test_results;



    TEST<uint8_t>("test8.tif", image8.data(), image8i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint8_t>("test8m.tif", image8.data(), image8i.data(), WIDTH, HEIGHT, 1, TEST_FRAMES, test_results);
    TEST<uint16_t>("test16.tif", image16.data(), image16i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint16_t>("test16m.tif", image16.data(), image16i.data(), WIDTH, HEIGHT, 1, TEST_FRAMES, test_results);
    TEST<uint32_t>("test32.tif", image32.data(), image32i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint32_t>("test32m.tif", image32.data(), image32i.data(), WIDTH, HEIGHT, 1, TEST_FRAMES, test_results);
    TEST<uint64_t>("test64.tif", image64.data(), image64i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint64_t>("test64m.tif", image64.data(), image64i.data(), WIDTH, HEIGHT, 1, TEST_FRAMES, test_results);
    TEST<float>("testf.tif", imagef.data(), imagefi.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<float>("testfm.tif", imagef.data(), imagefi.data(), WIDTH, HEIGHT, 1, TEST_FRAMES, test_results);
    TEST<double>("testd.tif", imaged.data(), imagedi.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<double>("testdm.tif", imaged.data(), imagedi.data(), WIDTH, HEIGHT, 1, TEST_FRAMES, test_results);
    TEST<uint8_t>("testrgb.tif", imagergb.data(), imagergbi.data(), WIDTH, HEIGHT, 3, 1, test_results);
    TEST<uint8_t>("testrgbm.tif", imagergb.data(), imagergbi.data(), WIDTH, HEIGHT, 3, TEST_FRAMES, test_results);

#ifdef TINYTIFF_TEST_LIBTIFF

    TEST<uint8_t>("test8_littleendian.tif", image8.data(), image8i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint8_t>("test8_bigendian.tif", image8.data(), image8i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint16_t>("test16_littleendian.tif", image16.data(), image16i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint16_t>("test16_bigendian.tif", image16.data(), image16i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint32_t>("test32_littleendian.tif", image32.data(), image32i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint32_t>("test32_bigendian.tif", image32.data(), image32i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint64_t>("test64_littleendian.tif", image64.data(), image64i.data(), WIDTH, HEIGHT, 1, 1, test_results);
    TEST<uint64_t>("test64_bigendian.tif", image64.data(), image64i.data(), WIDTH, HEIGHT, 1, 1, test_results);

    //TEST_AGAINST_LIBTIFF<uint16_t>("2K_source_Stack.tif",  test_results);
    //TEST_AGAINST_LIBTIFF<uint16_t>("2K_tiff_image.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("cell.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("circuit.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("galaxy.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("mri.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("multi-channel-time-series.ome.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint16_t>("test16m_imagej.tif",  test_results);
    TEST_AGAINST_LIBTIFF<float>("imagej_32bit_ramp.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("imagej_32bit_ramp_tiled.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("circuit_nocompression.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("mri_nocompression.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("galaxy_nocompression.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("corel_photopaint_grey.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint16_t>("corel_photopaint_grey16.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("corel_photopaint_greyalpha.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("corel_photopaint_rgb.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint16_t>("corel_photopaint_rgb48.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("corel_photopaint_rgba.tif",  test_results);
    TEST_AGAINST_LIBTIFF<uint8_t>("gh19-id8.tif",  test_results);
#else
    //TEST_SIMPLE<uint16_t>("2K_source_Stack.tif",  test_results);
    //TEST_SIMPLE<uint16_t>("2K_tiff_image.tif",  test_results);
    TEST_SIMPLE<uint8_t>("cell.tif",  test_results);
    TEST_SIMPLE<uint8_t>("circuit.tif",  test_results);
    TEST_SIMPLE<uint8_t>("galaxy.tif",  test_results);
    TEST_SIMPLE<uint8_t>("mri.tif",  test_results);
    TEST_SIMPLE<uint8_t>("multi-channel-time-series.ome.tif",  test_results);
    TEST_SIMPLE<uint16_t>("test16m_imagej.tif",  test_results);
    TEST_SIMPLE<float>("imagej_32bit_ramp.tif",  test_results);
    TEST_SIMPLE<uint8_t>("imagej_32bit_ramp_tiled.tif",  test_results);
    TEST_SIMPLE<uint8_t>("circuit_nocompression.tif",  test_results);
    TEST_SIMPLE<uint8_t>("mri_nocompression.tif",  test_results);
    TEST_SIMPLE<uint8_t>("galaxy_nocompression.tif",  test_results);
    TEST_SIMPLE<uint8_t>("corel_photopaint_grey.tif",  test_results);
    TEST_SIMPLE<uint16_t>("corel_photopaint_grey16.tif",  test_results);
    TEST_SIMPLE<uint8_t>("corel_photopaint_greyalpha.tif",  test_results);
    TEST_SIMPLE<uint8_t>("corel_photopaint_rgb.tif",  test_results);
    TEST_SIMPLE<uint16_t>("corel_photopaint_rgb48.tif",  test_results);
    TEST_SIMPLE<uint8_t>("corel_photopaint_rgba.tif",  test_results);

#endif
    TEST_SIMPLE<uint8_t>("gh19-id8.tif",  test_results);

    std::ostringstream testsum;
    testsum<<"\n\n\n\n";
    testsum<<"tinytiffreader_test:"<<std::endl;
    if (quicktest!=TINYTIFF_FALSE) testsum<<"  - quick test with --simple"<<std::endl;
#ifdef TINYTIFF_TEST_LIBTIFF
    testsum<<"  - checking against LibTIFF"<<std::endl;
#endif
    testsum<<"  - TinyTIFFReader Version: "<<TinyTIFFReader_getVersion()<<"\n  - TinyTIFFWriter Version: "<<TinyTIFFWriter_getVersion()<<"\n";
#ifdef TINYTIFF_TEST_LIBTIFF
    testsum<<"  - libTIFF Version: "<<TIFFGetVersion()<<"\n";
#endif
    testsum<<"\n"<<writeTestSummary(test_results)<<std::endl;
    std::cout<<testsum.str();
    std::ofstream file("tintytiffreader_test_result.txt", std::ofstream::out | std::ofstream::trunc);
    file<<testsum.str();
    file.close();
    return 0;
}
