#include "tinytiffwriter.h"
#include "tinytiff_tools.hxx"
#include "tinytiffhighrestimer.h"
#include "test_results.h"
#include "testimage_tools.h"
#include <array>
#include <map>
#include <fstream>
#include <sstream>
#ifdef TINYTIFF_TEST_LIBTIFF
#include <tiffio.h>
#include "libtiff_tools.h"
#endif



using namespace std;

int quicktest=TINYTIFF_FALSE;


void saveTestSummary(const std::string& filename, const std::vector<TestResult> &test_results, bool print) {
    std::ostringstream testsum;
    testsum<<"\n\n\n\n";
    testsum<<"tinytiffreader_speedtest:"<<std::endl;
    if (quicktest!=TINYTIFF_FALSE) testsum<<"  - quick test with --simple"<<std::endl;
#ifdef TINYTIFF_TEST_LIBTIFF
    testsum<<"  - cheching against LibTIFF"<<std::endl;
#endif
    testsum<<"  - TinyTIFFReader Version: "<<TinyTIFFReader_getVersion()<<"\n  - TinyTIFFWriter Version: "<<TinyTIFFWriter_getVersion()<<"\n";
#ifdef TINYTIFF_TEST_LIBTIFF
    testsum<<"  - libTIFF Version: "<<TIFFGetVersion()<<"\n";
#endif
    testsum<<"\n"<<writeTestSummary(test_results)<<std::endl;
    if (print)std::cout<<testsum.str();
    std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);
    file<<testsum.str();
    file.close();
}

template <class T>
void performTinyTIFFWriteTest(const std::string& name, const char* filename, const T* imagedata, size_t WIDTH, size_t HEIGHT, size_t SAMPLES, size_t FRAMES, TinyTIFFWriterSampleInterpretation interpret, std::vector<TestResult>& test_results, const float SPEEDTEST_REMOVESLOWEST_PERCENT = 0.1, TinyTIFFSampleLayout inputOrg=TinyTIFF_Chunky, TinyTIFFSampleLayout outputOrg=TinyTIFF_Chunky) {
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
    HighResTimer timer, timer1;
    timer.start();
    TinyTIFFWriterFile* tiff = TinyTIFFWriter_open(filename, bits, TinyTIFF_SampleFormatFromType<T>().format, SAMPLES, WIDTH,HEIGHT, interpret);
    if (tiff) {
        std::vector<double> runtimes;
        runtimes.reserve(FRAMES);
        for (size_t f=0; f<FRAMES; f++) {
            timer1.start();
            TinyTIFFWriter_writeImageMultiSample(tiff, imagedata, inputOrg, outputOrg);
            runtimes.push_back(timer1.get_time());
        }
        TinyTIFFWriter_close(tiff);
        test_results.back().duration_ms=timer.get_time()/1e3;
        test_results.back().numImages=FRAMES;
        test_results.back().success=true;
        reportRuntimes(name, runtimes, SPEEDTEST_REMOVESLOWEST_PERCENT, &test_results);
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
void performRAWWriteTest(const std::string& name, const char* filename, const T* imagedata, size_t WIDTH, size_t HEIGHT, size_t SAMPLES, size_t FRAMES, std::vector<TestResult>& test_results, const float SPEEDTEST_REMOVESLOWEST_PERCENT = 0.1) {
    const size_t bits=sizeof(T)*8;
    std::string desc=std::to_string(WIDTH)+"x"+std::to_string(HEIGHT)+"pix/"+std::to_string(bits)+"bit/"+std::to_string(SAMPLES)+"ch/"+std::to_string(FRAMES)+"frames";
    test_results.emplace_back();
    test_results.back().name=name+" ["+desc+", "+std::string(filename)+"]";
    test_results.back().success=true;
    std::cout<<"\n\n*****************************************************************************\n";
    std::cout<<"* "<<test_results.back().name<<"\n";
    HighResTimer timer, timer1;
    timer.start();
    FILE* fraw=fopen(filename, "w");
    if (fraw) {
        std::vector<double> runtimes;
        runtimes.reserve(FRAMES);
        for (size_t f=0; f<FRAMES; f++) {
            timer1.start();
            fwrite(imagedata, 1, sizeof(T)*WIDTH*HEIGHT*SAMPLES, fraw);
            runtimes.push_back(timer1.get_time());
        }
        fclose(fraw);
        test_results.back().duration_ms=timer.get_time()/1e3;
        test_results.back().numImages=FRAMES;
        test_results.back().success=true;
        reportRuntimes(name, runtimes, SPEEDTEST_REMOVESLOWEST_PERCENT, &test_results);
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
void performLibTIFFWriteTest(const std::string& name, const char* filename, const T* imagedata, size_t WIDTH, size_t HEIGHT, size_t SAMPLES, size_t FRAMES, std::vector<TestResult>& test_results, const float SPEEDTEST_REMOVESLOWEST_PERCENT = 0.1) {
#ifdef TINYTIFF_TEST_LIBTIFF
    const size_t bits=sizeof(T)*8;
    std::string desc=std::to_string(WIDTH)+"x"+std::to_string(HEIGHT)+"pix/"+std::to_string(bits)+"bit/"+std::to_string(SAMPLES)+"ch/"+std::to_string(FRAMES)+"frames";
    test_results.emplace_back();
    test_results.back().name=name+" ["+desc+", "+std::string(filename)+"]";
    test_results.back().success=true;
    std::cout<<"\n\n*****************************************************************************\n";
    std::cout<<"* "<<test_results.back().name<<"\n";
    HighResTimer timer, timer1;
    timer.start();
    TIFF* tifvideo=TIFFOpen(filename, "w");
    if (tifvideo) {
        std::vector<double> runtimes;
        runtimes.reserve(FRAMES);
        for (size_t f=0; f<FRAMES; f++) {
            timer1.start();
            TIFFWrite<T>(tifvideo, imagedata, WIDTH, HEIGHT);
            TIFFWriteDirectory(tifvideo);
            runtimes.push_back(timer1.get_time());
        }
        TIFFClose(tifvideo);
        test_results.back().duration_ms=timer.get_time()/1e3;
        test_results.back().success=true;
        test_results.back().numImages=FRAMES;
        reportRuntimes(name, runtimes, SPEEDTEST_REMOVESLOWEST_PERCENT, &test_results);
    } else {
        std::cout<<"ERROR: could not open '"<<filename<<"' for writing!\n";
        test_results.back().success=false;
    }
    if (test_results.back().success) {
        std::cout<<"* ==> SUCCESSFUL,   duration="<<test_results.back().duration_ms<<"ms\n";
    } else {
        std::cout<<"* ==> FAILED\n";
    }
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

struct TestConfig {
    size_t WIDTH;
    size_t HEIGHT;
    size_t SPEEDTEST_SIZE;
    float SPEEDTEST_REMOVESLOWEST_PERCENT;
};

template <class T>
void performTest(const std::vector<TestConfig>& image_sizes, std::vector<TestResult>& test_results, const size_t PATTERNSIZE = 12) {
    std::map<int, std::vector<T>> images;
    int i=0;
    for (auto& c: image_sizes) {
        if (images.find(i)==images.end()) {
            images[i]=std::vector<T>(c.WIDTH*c.HEIGHT, 0);
            write1ChannelTestData(images[i].data(), c.WIDTH, c.HEIGHT, PATTERNSIZE, 1);
        }
        performTinyTIFFWriteTest("TinyTIFFWriter-Test", std::string("tinytiff_speedtest_"+std::to_string(c.WIDTH)+"x"+std::to_string(c.HEIGHT)+"_uint"+std::to_string(sizeof(T)*8)+"_tinytiffwriter.tif").c_str(), images.at(i).data(), c.WIDTH, c.HEIGHT, 1, c.SPEEDTEST_SIZE, TinyTIFFWriter_AutodetectSampleInterpetation, test_results, c.SPEEDTEST_REMOVESLOWEST_PERCENT);
        saveTestSummary("tintytiffwriter_speedtest_result.txt", test_results, false);
        i++;
    }
    i=0;
    for (auto& c: image_sizes) {
        performRAWWriteTest("RAW-Test", std::string("tinytiff_speedtest_"+std::to_string(c.WIDTH)+"x"+std::to_string(c.HEIGHT)+"_uint"+std::to_string(sizeof(T)*8)+"_raw.raw").c_str(), images.at(i).data(), c.WIDTH, c.HEIGHT, 1, c.SPEEDTEST_SIZE, test_results, c.SPEEDTEST_REMOVESLOWEST_PERCENT);
        saveTestSummary("tintytiffwriter_speedtest_result.txt", test_results, false);
        i++;
    }
    i=0;
    for (auto& c: image_sizes) {
        performLibTIFFWriteTest("LibTIFF-Test", std::string("tinytiff_speedtest_"+std::to_string(c.WIDTH)+"x"+std::to_string(c.HEIGHT)+"_uint"+std::to_string(sizeof(T)*8)+"_libtiff.tif").c_str(), images.at(i).data(), c.WIDTH, c.HEIGHT, 1, c.SPEEDTEST_SIZE, test_results, c.SPEEDTEST_REMOVESLOWEST_PERCENT);
        saveTestSummary("tintytiffwriter_speedtest_result.txt", test_results, false);
        i++;
    }
}



int main(int argc, char *argv[]) {
    if (argc>1 && std::string(argv[1])=="--simple")  quicktest=TINYTIFF_TRUE;

    std::cout<<"tinytiffwriter_speedtest:"<<std::endl;
    if (quicktest!=TINYTIFF_FALSE) std::cout<<"  - quick test with --simple"<<std::endl;
#ifdef TINYTIFF_TEST_LIBTIFF
    std::cout<<"  - cheching against LibTIFF "<<TIFFGetVersion()<<std::endl;
    TIFFSetErrorHandler(errorhandler);
    TIFFSetWarningHandler(warninghandler);
#endif
    std::cout<<"creating some test TIFF files ..."<<std::endl;
    std::vector<TestResult> test_results;


    {
        std::vector<TestConfig> image_sizes;
        if (quicktest==TINYTIFF_FALSE) {
            image_sizes = {
                {32,32,10000,1},
                {64,64,10000,1},
                {128,128,10000,1},
                {256,256,5000,1},
                {512,512,1000,1},
                {1024,1024,500,1.0},
                {2048,2048,200,1.0},
                {4096,4096,50,1.0}
            };
        } else {
            image_sizes = {
                {128,128,100,0.5},
                {256,256,100,0.2},
                {512,512,100,0.1},
                {1024,1024,5,1.0},
                {2048,2048,5,1.0},
                {4096,4096,2,1.0}
            };
        }


        const size_t PATTERNSIZE = 12;
        performTest<uint16_t>(image_sizes, test_results, PATTERNSIZE);
        performTest<uint32_t>(image_sizes, test_results, PATTERNSIZE);
    }

    {
        std::vector<TestConfig> image_sizes ;
        if (quicktest==TINYTIFF_FALSE) {
            image_sizes = {
                {64,64,1,0.0},
                {64,64,10,10},
                {64,64,100,1},
                {64,64,1000,1},
                {64,64,10000,1},
                {64,64,100000,1},
                {64,64,1000000,1},
                };
        } else {
            image_sizes = {
                {32,32,1,0.0},
                {32,32,10,1},
                {32,32,100,0.5},
                {32,32,1000,1},
                {32,32,10000,1},
                };
        }


        const size_t PATTERNSIZE = 12;
        performTest<uint16_t>(image_sizes, test_results, PATTERNSIZE);
        performTest<uint32_t>(image_sizes, test_results, PATTERNSIZE);
    }



    saveTestSummary("tintytiffwriter_speedtest_result.txt", test_results, true);


    return 0;
}
