#include "tinytiffwriter.h"
#include "tinytiffhighrestimer.h"

#ifdef TINYTIFF_TEST_LIBTIFF
#include <tiffio.h>
#include "libtiff_tools.h"
#endif
#include <iostream>
#include <vector>
#include <algorithm>


using namespace std;

#define WIDTH 32
#define HEIGHT 32
#define PATTERNSIZE 12
#define SPEEDTEST_SIZE 100000
#define SPEEDTEST_REMOVESLOWEST_PERCENT 0.1

struct Statistics {
    double mean;
    double std;
    double min;
    double max;
    size_t removed_records;
};

Statistics evaluateRuntimes(std::vector<double> runtimes, double remove_slowest_percent=SPEEDTEST_REMOVESLOWEST_PERCENT) {
    std::sort(runtimes.begin(), runtimes.end());
    size_t removed_records=0;
    if (remove_slowest_percent>0 && remove_slowest_percent/100.0*runtimes.size()>0) {
        removed_records=static_cast<size_t>(remove_slowest_percent/100.0*runtimes.size());
        runtimes.erase(runtimes.end()-removed_records, runtimes.end());
    }
    double sum=0, sum2=0, mmin=0, mmax=0;

    for (size_t i=0; i<runtimes.size(); i++) {

        sum+=runtimes[i];
        sum2+=(runtimes[i]*runtimes[i]);
        if (i==0) mmin=mmax=runtimes[i];
        else {
            if (runtimes[i]>mmax) mmax=runtimes[i];
            if (runtimes[i]<mmin) mmin=runtimes[i];
        }
    }
    Statistics stat;
    const double NN=static_cast<double>(runtimes.size());
    stat.mean=sum/NN;
    stat.std=sqrt((sum2-sum*sum/NN)/(NN-1.0));
    stat.min=mmin;
    stat.max=mmax;
    stat.removed_records=removed_records;
    return stat;
}

void reportRuntimes(const std::vector<double>& runtimes, double remove_slowest_percent=SPEEDTEST_REMOVESLOWEST_PERCENT) {
    Statistics stat=evaluateRuntimes(runtimes, 0);
    std::cout<<"  ALL:     average time to write one image: "<<stat.mean<<" usecs    std: "<<stat.std<<" usecs     range: ["<<stat.min<<".."<<stat.max<<"] usecs\n";
    std::cout<<"  ALL:     average image rate: "<<1.0/(stat.mean)*1000.0<<" kHz\n";
    stat=evaluateRuntimes(runtimes, remove_slowest_percent);
    std::cout<<"  CLEANED: removed slowest "<<stat.removed_records<<" records\n";
    std::cout<<"  CLEANED: average time to write one image: "<<stat.mean<<" usecs    std: "<<stat.std<<" usecs     range: ["<<stat.min<<".."<<stat.max<<"] usecs\n";
    std::cout<<"  CLEANED: average image rate: "<<1.0/(stat.mean)*1000.0<<" kHz\n";
}


template <class T>
void libtiffTestRead(const char* filename, T* written, int width, int height)  {
#ifdef TINYTIFF_TEST_LIBTIFF
    TIFF* tif = TIFFOpen(filename, "r");
    T* data=(T*)malloc(width*height*sizeof(T));
    if (tif) {
        uint32 nx,ny;
        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
		char* val=NULL; 
		TIFFGetField(tif,TIFFTAG_IMAGEDESCRIPTION,&val); 
		if (val) std::cout<<"    ImageDescription("<<strlen(val)<<"):\n"<<val<<"\n"; 
        TIFFPrintDirectory(tif, stdout, TIFFPRINT_STRIPS);
        if (nx==width && ny==height) {
            if (TIFFReadFrame(tif, data)) {
                bool ok=true;
                for (int i=0; i<width*height; i++) {
                    ok=ok&&(data[i]==written[i]);
                }
                if (ok) {
                    std::cout<<" -- TEST READ WITH LIBTIFF: TEST PASSED!!!\n";
                } else  {
                    std::cout<<" -- TEST READ WITH LIBTIFF: READ WRONG DATA!!!\n";
                }
            } else {
                std::cout<<" -- TEST READ WITH LIBTIFF: COULD NOT READ FRAME!\n";
            }
        } else {
            std::cout<<" -- TEST READ WITH LIBTIFF: FRAME SIZE DOES NOT MATCH ("<<nx<<"!="<<width<<",   "<<ny<<"!="<<height<<")!\n";
        }

        TIFFClose(tif);
    } else {
        std::cout<<" -- TEST READ WITH LIBTIFF: COULD NOT OPEN FILE!\n";
    }
    free(data);
#endif
}

int main() {
    uint8_t* image8=(uint8_t*)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    uint8_t* image8i=(uint8_t*)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    uint16_t* image16=(uint16_t*)calloc(WIDTH*HEIGHT, sizeof(uint16_t));
    uint16_t* image16i=(uint16_t*)calloc(WIDTH*HEIGHT, sizeof(uint16_t));
    float* imagef=(float*)calloc(WIDTH*HEIGHT, sizeof(float));
    float* imagefi=(float*)calloc(WIDTH*HEIGHT, sizeof(float));
    uint8_t* imagergb=(uint8_t*)calloc(WIDTH*HEIGHT*3, sizeof(uint8_t));
    uint8_t* imagergbi=(uint8_t*)calloc(WIDTH*HEIGHT*3, sizeof(uint8_t));

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
            imagef[i]=float(image8[i])/255.0-0.5;
            imagefi[i]=1.0-float(image8[i])/255.0-0.5;
            imagergb[i*3+0]=x*255/WIDTH;
            imagergb[i*3+1]=0;
            imagergb[i*3+2]=y*255/WIDTH;
            if (x==y) imagergb[i*3+1]=254;
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) imagergb[i*3+1]=200;
            else if (x%PATTERNSIZE==0) imagergb[i*3+1]=75;
            else if (y%PATTERNSIZE==0) imagergb[i*3+1]=150;
            imagergbi[i*3+0]=255-imagergb[i*3+0];
            imagergbi[i*3+1]=255-imagergb[i*3+1];
            imagergbi[i*3+2]=255-imagergb[i*3+2];
            i++;
        }
    }

    std::cout<<"WRITING 8-Bit UINT TIFF\n";
    TinyTIFFWriterFile* tiff = TinyTIFFWriter_open("test8.tif", 8, 1, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image8);
    TinyTIFFWriter_close_withdescription(tiff, "");
    tiff = TinyTIFFWriter_open("test8m.tif", 8, 1, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image8);
    TinyTIFFWriter_writeImage(tiff, image8i);
    TinyTIFFWriter_writeImage(tiff, image8);
    TinyTIFFWriter_writeImage(tiff, image8i);
    TinyTIFFWriter_close(tiff);
    libtiffTestRead<uint8_t>("test8.tif", image8, WIDTH, HEIGHT);


    std::cout<<"WRITING 16-Bit UINT TIFF\n";
    tiff = TinyTIFFWriter_open("test16.tif", 16, 1, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image16);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);
    tiff = TinyTIFFWriter_open("test16m.tif", 16, 1, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image16);
    TinyTIFFWriter_writeImage(tiff, image16i);
    TinyTIFFWriter_writeImage(tiff, image16);
    TinyTIFFWriter_writeImage(tiff, image16i);
    TinyTIFFWriter_writeImage(tiff, image16);
    TinyTIFFWriter_writeImage(tiff, image16i);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);
    libtiffTestRead<uint16_t>("test16.tif", image16, WIDTH, HEIGHT);

    std::cout<<"WRITING 32-Bit FLOAT TIFF\n";
    tiff = TinyTIFFWriter_open("testf.tif", 32, 1, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, imagef);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);
    tiff = TinyTIFFWriter_open("testfm.tif", 32, 1, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, imagef);
    TinyTIFFWriter_writeImage(tiff, imagefi);
    TinyTIFFWriter_writeImage(tiff, imagef);
    TinyTIFFWriter_writeImage(tiff, imagefi);
    TinyTIFFWriter_writeImage(tiff, imagef);
    TinyTIFFWriter_writeImage(tiff, imagefi);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);
    libtiffTestRead<float>("testf.tif", imagef, WIDTH, HEIGHT);


    std::cout<<"WRITING 3-CHANNEL 8-Bit UINT RGB TIFF\n";
    tiff = TinyTIFFWriter_open("test_rgb.tif", 8, 3, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, imagergb);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);
    tiff = TinyTIFFWriter_open("test_rgbm.tif", 8, 3, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, imagergb);
    TinyTIFFWriter_writeImage(tiff, imagergbi);
    TinyTIFFWriter_writeImage(tiff, imagergb);
    TinyTIFFWriter_writeImage(tiff, imagergbi);
    TinyTIFFWriter_writeImage(tiff, imagergb);
    TinyTIFFWriter_writeImage(tiff, imagergbi);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);
    //libtiffTestReadRGB<float>("test_rgb.tif", imagergb, WIDTH, HEIGHT);

    std::cout<<"TIFF SPEED TEST, 8-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
    HighResTimer timer;

    tiff = TinyTIFFWriter_open("test8_speedtest.tif", 8, 1, WIDTH,HEIGHT);
    std::vector<double> runtimes;
    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        TinyTIFFWriter_writeImage(tiff, image8i);
        const double duration=timer.get_time();
        runtimes.push_back(duration);
    }
    reportRuntimes(runtimes);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);




    std::cout<<"RAW SPEED TEST, 8-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
    FILE* fraw=fopen("test8_speedtest.raw", "w");
    runtimes.clear();
    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        fwrite(image8i, WIDTH*HEIGHT, 1, fraw);
        const double duration=timer.get_time();
        runtimes.push_back(duration);
    }
    reportRuntimes(runtimes);



    std::cout<<"TIFF SPEED TEST, 16-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";

    tiff = TinyTIFFWriter_open("test16_speedtest.tif", 16, 1, WIDTH,HEIGHT);
    runtimes.clear();

    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        TinyTIFFWriter_writeImage(tiff, image16);
        const double duration=timer.get_time();
        runtimes.push_back(duration);
    }
    reportRuntimes(runtimes);

    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);





    std::cout<<"RAW SPEED TEST, 16-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
    fraw=fopen("test16_speedtest.raw", "w");
    runtimes.clear();

    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        fwrite(image16i, WIDTH*HEIGHT*2, 1, fraw);
        const double duration=timer.get_time();
        runtimes.push_back(duration);
    }
    reportRuntimes(runtimes);



#ifdef TINYTIFF_TEST_LIBTIFF
    if (SPEEDTEST_SIZE<60000) {
        std::cout<<"LIBTIFF SPEED TEST, 8-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
        TIFF* tifvideo=TIFFOpen("libtifftest8_speedtest.tif", "w");
        runtimes.clear();

        for (int i=0; i<SPEEDTEST_SIZE; i++) {
            timer.start();
            TIFFTWriteUint8(tifvideo, image8i, WIDTH, HEIGHT);
            TIFFWriteDirectory(tifvideo);
            const double duration=timer.get_time();
            runtimes.push_back(duration);

        }
        TIFFClose(tifvideo);
        reportRuntimes(runtimes);






        std::cout<<"LIBTIFF SPEED TEST, 16-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
        tifvideo=TIFFOpen("libtifftest16_speedtest.tif", "w");
        runtimes.clear();

        for (int i=0; i<SPEEDTEST_SIZE; i++) {
            timer.start();
            TIFFTWriteUint16(tifvideo, image16i, WIDTH, HEIGHT);
            TIFFWriteDirectory(tifvideo);
            const double duration=timer.get_time();
            runtimes.push_back(duration);

        }
        TIFFClose(tifvideo);
        reportRuntimes(runtimes);

    }
#endif




    free(image8);
    return 0;
}
