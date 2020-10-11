#include "tinytiffwriter.h"
#include "tinytiffhighrestimer.h"
#include "test_results.h"
#include "testimage_tools.h"
#include <fstream>
#include <array>
#ifdef TINYTIFF_TEST_LIBTIFF
#include <tiffio.h>
#include "libtiff_tools.h"
#endif
#include "tinytiff_tools.hxx"



using namespace std;



template <class T>
bool libtiffTestRead(const char* filename, const T* writteneven, const T* writtenodd, uint32_t width, uint32_t height, uint16_t samples=1, uint32_t frames_expected=0, TinyTIFFSampleLayout inputOrg=TinyTIFF_Chunky)  {
    bool ok=true;
#ifdef TINYTIFF_TEST_LIBTIFF
    TIFF* tif = TIFFOpen(filename, "r");
    T* data=(T*)malloc(width*height*sizeof(T));
    if (tif) {
        uint32_t frame=0;
        do {
            uint32_t nx,ny;
            uint16_t ns,bs;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&ns);
            TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bs);
            char* val=NULL;
            TIFFGetField(tif,TIFFTAG_IMAGEDESCRIPTION,&val);
            if (val) std::cout<<"    ImageDescription("<<strlen(val)<<"):\n"<<val<<"\n";
            TIFFPrintDirectory(tif, stdout);
            if (nx==width && ny==height && ns==samples && bs==sizeof(T)*8) {
                size_t errcnt=0;
                size_t pixcnt=0;
                for (uint16_t samp=0; samp<samples; samp++) {
                    if (TIFFReadFrame(tif, data, samp)) {
                        ok=true;
                        const T* written=writteneven;
                        if (writtenodd && frame%2==1) written=writtenodd;
                        if (inputOrg==TinyTIFF_Chunky) {
                            for (uint32 i=0; i<width*height; i++) {
                                if (data[i]!=written[i*samples+samp]) {
                                    ok=false;
                                    errcnt++;
                                    if (errcnt<50) std::cout<<" -- READ-ERROR: pixel-value differs in frame "<<frame<<" for pixel ("<<i%width<<","<<i/width<<"): file: "<<static_cast<typename atleast_int<T>::TPrint>(data[i])<<" expected: "<<static_cast<typename atleast_int<T>::TPrint>(written[i*samples+samp])<<"\n";
                                    if (errcnt==50) std::cout<<" -- READ-ERROR: ...\n";
                                } else {
                                    pixcnt++;
                                }
                            }
                        } else {
                            for (uint32 i=0; i<width*height; i++) {
                                if (data[i]!=written[i+samp*width*height]) {
                                    ok=false;
                                    errcnt++;
                                    if (errcnt<50) std::cout<<" -- READ-ERROR: pixel-value differs in frame "<<frame<<" for pixel ("<<i%width<<","<<i/width<<"): file: "<<static_cast<typename atleast_int<T>::TPrint>(data[i])<<" expected: "<<static_cast<typename atleast_int<T>::TPrint>(written[i+samp*width*height])<<"\n";
                                    if (errcnt==50) std::cout<<" -- READ-ERROR: ...\n";
                                } else {
                                    pixcnt++;
                                }
                            }
                        }
                        if (!ok) {
                            std::cout<<" -- TEST READ WITH LIBTIFF: READ WRONG DATA for "<<errcnt<<" pixels in frame "<<frame<<"!!!\n";
                        }
                    } else {
                        std::cout<<" -- TEST READ WITH LIBTIFF: COULD NOT READ FRAME "<<frame<<"!\n";
                        ok=false;
                    }
                }
                if (ok) {
                    std::cout<<" -- TEST READ WITH LIBTIFF: SUCCESS FOR FRAME "<<frame<<"! All "<<pixcnt<<" pixels&samples as expected!\n";
                }
            } else {
                std::cout<<" -- TEST READ WITH LIBTIFF: FRAME SIZE OF FRAME "<<frame<<" DOES NOT MATCH (width: file:"<<nx<<"/expected:"<<width<<",   height: file:"<<ny<<"/expected:"<<height<<",   samples: file:"<<ns<<"/expected:"<<samples<<",   bitspersample: file:"<<bs<<"/expected:"<<(sizeof(T)*8)<<")!\n";
                ok=false;
            }
            frame++;
        } while (ok && TIFFReadDirectory(tif));
        if (frames_expected>0 && frames_expected!=frame) {
            std::cout<<" -- ERROR IN TEST READ WITH LIBTIFF: number of frames ("<<frame<<")does not match expected number of frames ("<<frames_expected<<")\n";
            ok=false;
        }
        TIFFClose(tif);
    } else {
        std::cout<<" -- TEST READ WITH LIBTIFF: COULD NOT OPEN FILE!\n";
        ok=false;
    }
    free(data);
#endif
    return ok;
}


template <class T>
void performWriteTest(const std::string& name, const char* filename, const T* imagedata, size_t WIDTH, size_t HEIGHT, size_t SAMPLES, TinyTIFFWriterSampleInterpretation interpret, std::vector<TestResult>& test_results, TinyTIFFSampleLayout inputOrg=TinyTIFF_Chunky, TinyTIFFSampleLayout outputOrg=TinyTIFF_Chunky) {
    const size_t bits=sizeof(T)*8;
    std::string desc=std::to_string(WIDTH)+"x"+std::to_string(HEIGHT)+"pix/"+std::to_string(bits)+"bit/"+std::to_string(SAMPLES)+"ch/1frame";
    if (inputOrg==TinyTIFF_Chunky && outputOrg==TinyTIFF_Chunky) desc+="/CHUNKY_FROM_CHUNKY";
    if (inputOrg==TinyTIFF_Chunky && outputOrg==TinyTIFF_Planar) desc+="/PLANAR_FROM_CHUNKY";
    if (inputOrg==TinyTIFF_Planar && outputOrg==TinyTIFF_Chunky) desc+="/CHUNKY_FROM_PLANAR";
    if (inputOrg==TinyTIFF_Planar && outputOrg==TinyTIFF_Planar) desc+="/PLANAR_FROM_PLANAR";
    test_results.emplace_back();
    test_results.back().name=name+" ["+desc+", "+std::string(filename)+"]";
    test_results.back().success=true;
    std::cout<<"\n\n*****************************************************************************\n";
    std::cout<<"* "<<test_results.back().name<<"\n";
    HighResTimer timer;
    timer.start();
    TinyTIFFWriterFile* tiff = TinyTIFFWriter_open(filename, bits, TinyTIFF_SampleFormatFromType<T>().format, SAMPLES, WIDTH,HEIGHT, interpret);
    if (tiff) {
        int res;
        res=TinyTIFFWriter_writeImageMultiSample(tiff, imagedata, inputOrg, outputOrg);
        if (res!=TINYTIFF_TRUE) {
            test_results.back().success=false;
            std::cout<<"ERROR: error writing image data into '"<<filename<<"'! MESSAGE: "<<TinyTIFFWriter_getLastError(tiff)<<"\n";
        }
        TinyTIFFWriter_close(tiff);
        test_results.back().duration_ms=timer.get_time()/1e3;
        test_results.back().numImages=1;
        if ((get_filesize(filename)<=0)) {
            test_results.back().success=false;
            std::cout<<"ERROR: file '"<<filename<<"' has no contents!\n";
        } else if (!libtiffTestRead<T>(filename, imagedata, nullptr, WIDTH, HEIGHT, SAMPLES, 1, inputOrg)) {
            test_results.back().success=false;
            std::cout<<"ERROR: reading '"<<filename<<"' with libTIFF failed!\n";
        }
    } else {
        std::cout<<"ERROR: could not open '"<<filename<<"' for writing!\n";
        test_results.back().success=false;
    }
    if (test_results.back().success) {
        std::cout<<"* ==> SUCCESSFUL,   duration="<<test_results.back().duration_ms<<"ms\n";
    } else {
        std::cout<<"* ==> FAILED\n";
    }
}

template <class T>
void performMultiFrameWriteTest(const std::string& name, const char* filename, const T* imagedata, const T* imagedatai, size_t WIDTH, size_t HEIGHT, size_t SAMPLES, size_t FRAMES, TinyTIFFWriterSampleInterpretation interpret, std::vector<TestResult>& test_results, TinyTIFFSampleLayout inputOrg=TinyTIFF_Chunky, TinyTIFFSampleLayout outputOrg=TinyTIFF_Chunky) {
    const size_t bits=sizeof(T)*8;
    std::string desc=std::to_string(WIDTH)+"x"+std::to_string(HEIGHT)+"pix/"+std::to_string(bits)+"bit/"+std::to_string(SAMPLES)+"ch/"+std::to_string(FRAMES)+"frames";
    if (inputOrg==TinyTIFF_Chunky && outputOrg==TinyTIFF_Chunky) desc+="/CHUNKY_FROM_CHUNKY";
    if (inputOrg==TinyTIFF_Chunky && outputOrg==TinyTIFF_Planar) desc+="/PLANAR_FROM_CHUNKY";
    if (inputOrg==TinyTIFF_Planar && outputOrg==TinyTIFF_Chunky) desc+="/CHUNKY_FROM_PLANAR";
    if (inputOrg==TinyTIFF_Planar && outputOrg==TinyTIFF_Planar) desc+="/PLANAR_FROM_PLANAR";
    test_results.emplace_back();
    test_results.back().name=name+" ["+desc+", "+std::string(filename)+"]";
    test_results.back().success=true;
    std::cout<<"\n\n*****************************************************************************\n";
    std::cout<<"* "<<test_results.back().name<<"\n";
    HighResTimer timer;
    timer.start();
    TinyTIFFWriterFile* tiff = TinyTIFFWriter_open(filename, bits, TinyTIFF_SampleFormatFromType<T>().format, SAMPLES, WIDTH,HEIGHT, interpret);
    if (tiff) {
        for (size_t f=0; f<FRAMES; f++) {
            int res;
            if (f%2==0) res=TinyTIFFWriter_writeImageMultiSample(tiff, imagedata, inputOrg, outputOrg);
            else res=TinyTIFFWriter_writeImageMultiSample(tiff, imagedatai, inputOrg, outputOrg);
            if (res!=TINYTIFF_TRUE) {
                test_results.back().success=false;
                std::cout<<"ERROR: error writing image data into '"<<filename<<"'! MESSAGE: "<<TinyTIFFWriter_getLastError(tiff)<<"\n";
            }

        }
        TinyTIFFWriter_close(tiff);
        test_results.back().duration_ms=timer.get_time()/1e3;
        test_results.back().numImages=FRAMES;
        if ((get_filesize(filename)<=0)) {
            test_results.back().success=false;
            std::cout<<"ERROR: file '"<<filename<<"' has no contents!\n";
        } else if (!libtiffTestRead<T>(filename, imagedata, imagedatai, WIDTH, HEIGHT, SAMPLES, FRAMES, inputOrg)) {
            test_results.back().success=false;
            std::cout<<"ERROR: reading '"<<filename<<"' with libTIFF failes!\n";
        }
    } else {
        std::cout<<"ERROR: could not open '"<<filename<<"' for writing!\n";
        test_results.back().success=false;
    }
    if (test_results.back().success) {
        std::cout<<"* ==> SUCCESSFUL,   duration="<<test_results.back().duration_ms<<"ms\n";
    } else {
        std::cout<<"* ==> FAILED\n";
    }
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

    std::cout<<"tinytiffwriter_test:"<<std::endl;
    if (quicktest!=TINYTIFF_FALSE) std::cout<<"  - quick test with --simple"<<std::endl;
#ifdef TINYTIFF_TEST_LIBTIFF
    std::cout<<"  - cheching against LibTIFF"<<std::endl;
    TIFFSetErrorHandler(errorhandler);
    TIFFSetWarningHandler(warninghandler);
#endif
    std::vector<TestResult> test_results;

    const size_t WIDTH = (quicktest!=TINYTIFF_FALSE)?32:300;
    const size_t HEIGHT = (quicktest!=TINYTIFF_FALSE)?32:125;
    const size_t PATTERNSIZE = 12;


    std::vector<uint8_t> image8(WIDTH*HEIGHT, 0);
    std::vector<uint8_t> image8i(WIDTH*HEIGHT, 0);
    std::vector<uint16_t> image16(WIDTH*HEIGHT, 0);
    std::vector<uint16_t> image16i(WIDTH*HEIGHT, 0);
    std::vector<uint32_t> image32(WIDTH*HEIGHT, 0);
    std::vector<uint32_t> image32i(WIDTH*HEIGHT, 0);
    std::vector<uint64_t> image64(WIDTH*HEIGHT, 0);
    std::vector<uint64_t> image64i(WIDTH*HEIGHT, 0);
    std::vector<float> imagef(WIDTH*HEIGHT, 0);
    std::vector<float> imagefi(WIDTH*HEIGHT, 0);
    std::vector<double> imaged(WIDTH*HEIGHT, 0);
    std::vector<double> imagedi(WIDTH*HEIGHT, 0);
    std::vector<uint8_t> imagergb(WIDTH*HEIGHT*3, 0);
    std::vector<uint8_t> imagergbi(WIDTH*HEIGHT*3, 0);
    std::vector<uint8_t> imagergba(WIDTH*HEIGHT*4, 0);
    std::vector<uint8_t> imagergbai(WIDTH*HEIGHT*4, 0);
    std::vector<uint8_t> greyalpha(WIDTH*HEIGHT*2, 0);
    std::vector<uint8_t> greyalphai(WIDTH*HEIGHT*2, 0);
    std::vector<uint8_t> imagergbplan(WIDTH*HEIGHT*3, 0);
    std::vector<uint8_t> imagergbplani(WIDTH*HEIGHT*3, 0);


    write1ChannelTestData(image8.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    write1ChannelTestData(image8i.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    invertTestImage(image8i.data(), WIDTH, HEIGHT, 1);
    write1ChannelTestData(image16.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    write1ChannelTestData(image16i.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    invertTestImage(image16i.data(), WIDTH, HEIGHT, 1);
    write1ChannelTestData(image32.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    write1ChannelTestData(image32i.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    invertTestImage(image32i.data(), WIDTH, HEIGHT, 1);
    write1ChannelTestData(image64.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    write1ChannelTestData(image64i.data(), WIDTH, HEIGHT, PATTERNSIZE, 1);
    invertTestImage(image64i.data(), WIDTH, HEIGHT, 1);
    write1ChannelFloatTestData<float>(imagef.data(), WIDTH, HEIGHT, PATTERNSIZE, 1, 1.0);
    write1ChannelFloatTestData<float>(imagefi.data(), WIDTH, HEIGHT, PATTERNSIZE, 1, 1.0);
    invertFloatTestImage<float>(imagefi.data(), WIDTH, HEIGHT, 1, 1.0);
    write1ChannelFloatTestData<double>(imaged.data(), WIDTH, HEIGHT, PATTERNSIZE, 1, 1.0);
    write1ChannelFloatTestData<double>(imagedi.data(), WIDTH, HEIGHT, PATTERNSIZE, 1, 1.0);
    invertFloatTestImage<double>(imagedi.data(), WIDTH, HEIGHT, 1, 1.0);
    writeRGBTestDataChunky(imagergb.data(), WIDTH, HEIGHT, PATTERNSIZE,3);
    writeRGBTestDataChunky(imagergbi.data(), WIDTH, HEIGHT, PATTERNSIZE,3);
    invertTestImage(imagergbi.data(), WIDTH, HEIGHT,3);
    writeRGBTestDataPlanar(imagergbplan.data(), WIDTH, HEIGHT, PATTERNSIZE,3);
    writeRGBTestDataPlanar(imagergbplani.data(), WIDTH, HEIGHT, PATTERNSIZE,3);
    invertTestImage(imagergbplani.data(), WIDTH, HEIGHT,3);
    writeRGBTestDataChunky(imagergba.data(), WIDTH, HEIGHT, PATTERNSIZE,4);
    writeALPHATestData(imagergba.data(), 3, WIDTH, HEIGHT, 4);
    writeRGBTestDataChunky(imagergbai.data(), WIDTH, HEIGHT, PATTERNSIZE,4);
    writeALPHATestData(imagergbai.data(), 3, WIDTH, HEIGHT, 4);
    invertTestImage(imagergbai.data(), WIDTH, HEIGHT,4);
    write1ChannelTestData(greyalpha.data(), WIDTH, HEIGHT, PATTERNSIZE, 2);
    writeALPHATestData(greyalpha.data(), 1, WIDTH, HEIGHT, 2);
    write1ChannelTestData(greyalphai.data(), WIDTH, HEIGHT, PATTERNSIZE, 2);
    writeALPHATestData(greyalphai.data(), 1, WIDTH, HEIGHT, 2);
    invertTestImage(greyalphai.data(), WIDTH, HEIGHT, 2);


    performWriteTest("WRITING 8-Bit UINT GREY TIFF", "test8.tif", image8.data(), WIDTH, HEIGHT, 1, TinyTIFFWriter_Greyscale, test_results);
    performMultiFrameWriteTest("WRITING 8-Bit UINT GREY TIFF", "test8m.tif", image8.data(), image8i.data(), WIDTH, HEIGHT, 1, 6, TinyTIFFWriter_Greyscale, test_results);

    performWriteTest("WRITING 16-Bit UINT GREY TIFF", "test16.tif", image16.data(), WIDTH, HEIGHT, 1, TinyTIFFWriter_Greyscale, test_results);
    performMultiFrameWriteTest("WRITING 16-Bit UINT GREY TIFF", "test16m.tif", image16.data(), image16i.data(), WIDTH, HEIGHT, 1, 6, TinyTIFFWriter_Greyscale, test_results);

    performWriteTest("WRITING 32-Bit UINT GREY TIFF", "test32.tif", image32.data(), WIDTH, HEIGHT, 1, TinyTIFFWriter_Greyscale, test_results);
    performMultiFrameWriteTest("WRITING 32-Bit UINT GREY TIFF", "test32m.tif", image32.data(), image32i.data(), WIDTH, HEIGHT, 1, 6, TinyTIFFWriter_Greyscale, test_results);

    performWriteTest("WRITING 64-Bit UINT GREY TIFF", "test64.tif", image64.data(), WIDTH, HEIGHT, 1, TinyTIFFWriter_Greyscale, test_results);
    performMultiFrameWriteTest("WRITING 64-Bit UINT GREY TIFF", "test64m.tif", image64.data(), image64i.data(), WIDTH, HEIGHT, 1, 6, TinyTIFFWriter_Greyscale, test_results);

    performWriteTest("WRITING 32-Bit FLOAT GREY TIFF", "testf.tif", imagef.data(), WIDTH, HEIGHT, 1, TinyTIFFWriter_Greyscale, test_results);
    performMultiFrameWriteTest("WRITING 32-Bit FLOAT GREY TIFF", "testfm.tif", imagef.data(), imagefi.data(), WIDTH, HEIGHT, 1, 6, TinyTIFFWriter_Greyscale, test_results);

    performWriteTest("WRITING 64-Bit FLOAT GREY TIFF", "testd.tif", imaged.data(), WIDTH, HEIGHT, 1, TinyTIFFWriter_Greyscale, test_results);
    performMultiFrameWriteTest("WRITING 64-Bit FLOAT GREY TIFF", "testdm.tif", imaged.data(), imagedi.data(), WIDTH, HEIGHT, 1, 6, TinyTIFFWriter_Greyscale, test_results);

    performWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgb.tif", imagergb.data(), WIDTH, HEIGHT, 3, TinyTIFFWriter_RGB, test_results);
    performMultiFrameWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgbm.tif", imagergb.data(), imagergbi.data(), WIDTH, HEIGHT, 3, 6, TinyTIFFWriter_RGB, test_results);

    performWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgb_chunkplan.tif", imagergb.data(), WIDTH, HEIGHT, 3, TinyTIFFWriter_RGB, test_results, TinyTIFF_Chunky, TinyTIFF_Planar);
    performMultiFrameWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgbm_chunkplan.tif", imagergb.data(), imagergbi.data(), WIDTH, HEIGHT, 3, 6, TinyTIFFWriter_RGB, test_results, TinyTIFF_Chunky, TinyTIFF_Planar);

    performWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgb_planchunk.tif", imagergbplan.data(), WIDTH, HEIGHT, 3, TinyTIFFWriter_RGB, test_results, TinyTIFF_Planar, TinyTIFF_Chunky);
    performMultiFrameWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgbm_planchunk.tif", imagergbplan.data(), imagergbplani.data(), WIDTH, HEIGHT, 3, 6, TinyTIFFWriter_RGB, test_results, TinyTIFF_Planar, TinyTIFF_Chunky);

    performWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgb_planplan.tif", imagergbplan.data(), WIDTH, HEIGHT, 3, TinyTIFFWriter_RGB, test_results, TinyTIFF_Planar, TinyTIFF_Planar);
    performMultiFrameWriteTest("WRITING 8-Bit UINT RGB TIFF", "testrgbm_planplan.tif", imagergbplan.data(), imagergbplani.data(), WIDTH, HEIGHT, 3, 6, TinyTIFFWriter_RGB, test_results, TinyTIFF_Planar, TinyTIFF_Planar);

    performWriteTest("WRITING 8-Bit UINT RGBA TIFF", "testrgba.tif", imagergba.data(), WIDTH, HEIGHT, 4, TinyTIFFWriter_RGBA, test_results);
    performMultiFrameWriteTest("WRITING 8-Bit UINT RGBA TIFF", "testrgbam.tif", imagergba.data(), imagergbai.data(), WIDTH, HEIGHT, 4, 6, TinyTIFFWriter_RGBA, test_results);

    performWriteTest("WRITING 8-Bit UINT GREY+ALPHA  TIFF", "test_ga.tif", greyalpha.data(), WIDTH, HEIGHT, 2, TinyTIFFWriter_GreyscaleAndAlpha, test_results);
    performMultiFrameWriteTest("WRITING 8-Bit UINT GREY+ALPHA TIFF", "test_gam.tif", greyalpha.data(), greyalphai.data(), WIDTH, HEIGHT, 2, 6, TinyTIFFWriter_GreyscaleAndAlpha, test_results);



    const std::string testsum=writeTestSummary(test_results);
    std::cout<<"\n\n\n\n";
    std::cout<<"tinytiffwriter_test:"<<std::endl;
    if (quicktest!=TINYTIFF_FALSE) std::cout<<"  - quick test with --simple"<<std::endl;
#ifdef TINYTIFF_TEST_LIBTIFF
    std::cout<<"  - cheching against LibTIFF"<<std::endl;
#endif
    std::cout<<"  - TinyTIFFReader Version: "<<TinyTIFFReader_getVersion()<<"\n  - TinyTIFFWriter Version: "<<TinyTIFFWriter_getVersion()<<"\n";
#ifdef TINYTIFF_TEST_LIBTIFF
    std::cout<<"  - libTIFF Version: "<<TIFFGetVersion()<<"\n";
#endif
    std::ofstream file;
    file.open("tintytiffwriter_test_result.txt");
    file<<testsum;

    return 0;
}
