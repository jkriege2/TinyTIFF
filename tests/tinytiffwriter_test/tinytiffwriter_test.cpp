#include "tinytiffwriter.h"
#include "tinytiffhighrestimer.h"

#ifdef TINYTIFF_TEST_LIBTIFF
#include <tiffio.h>
#include "libtiff_tools.h"
#endif
#include <iostream>


using namespace std;

#define WIDTH 32
#define HEIGHT 32
#define PATTERNSIZE 12
#define SPEEDTEST_SIZE 100000
#define SPEEDTEST_OUTPUT 10

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

    FILE* fstat;
    tiff = TinyTIFFWriter_open("test8_speedtest.tif", 8, 1, WIDTH,HEIGHT);
    fstat=fopen("test8_speedtest.dat", "w");
    double sum[SPEEDTEST_SIZE/SPEEDTEST_OUTPUT+5];
    double sum2[SPEEDTEST_SIZE/SPEEDTEST_OUTPUT+5];
    double msum=0;
    double mmin=0;
    double mmax=0;
    int sumi=0;
    sum[0]=0;
    sum2[0]=0;
    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        TinyTIFFWriter_writeImage(tiff, image8i);
        double duration=timer.get_time();
        sum[sumi]+=duration;
        sum2[sumi]+=duration*duration;
        msum+=duration;
        if (i%SPEEDTEST_OUTPUT==0) {
            /*double NN=(double)SPEEDTEST_OUTPUT;
            double mean=sum[sumi]/NN;
            double std=sqrt((sum2[sumi]-sum[sumi]*sum[sumi]/NN)/(NN-1.0));
            std::cout<<"  "<<(double)i/(double)SPEEDTEST_SIZE*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
            fprintf(fstat, "%d, %lf, %lf\n", i, mean, std);
            */
            sumi++;
            sum[sumi]=0;
            sum2[sumi]=0;
        }
        if (i==0) mmin=mmax=duration;
        else {
            if (duration>mmax) mmax=duration;
            if (duration<mmin) mmin=duration;
        }
    }
    double min=0;
    double max=0;
    double mmean=0;
    for (int i=0; i<SPEEDTEST_SIZE/SPEEDTEST_OUTPUT; i++) {
        double NN=(double)SPEEDTEST_OUTPUT;
        double mean=sum[i]/NN;
        mmean+=mean;
        double std=sqrt((sum2[i]-sum[i]*sum[i]/NN)/(NN-1.0));
        if (i==0) min=max=mean;
        else {
            if (mean>max) max=mean;
            if (mean<min) min=mean;
        }
        //std::cout<<"  "<<(double)i/(double)((double)SPEEDTEST_SIZE/(double)SPEEDTEST_OUTPUT)*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
        fprintf(fstat, "%d, %lf, %lf\n", i*SPEEDTEST_OUTPUT, mean, std);
    }
    std::cout<<"  average time to write one image: "<<msum/(double)(SPEEDTEST_SIZE)<<" usecs    range: ["<<mmin<<".."<<mmax<<"] usecs\n";
    std::cout<<"  average image rate: "<<1/(msum/(double)(SPEEDTEST_SIZE))*1000.0<<" kHz\n";
    fclose(fstat);
    fstat=fopen("test8_speedtest.plt", "w");
    fprintf(fstat, "set title 'TinyTIFFWrite Speed Test, %d 8-bit %dx%d images, each point: average over %d images'\n", SPEEDTEST_SIZE, WIDTH, HEIGHT,
            SPEEDTEST_OUTPUT);
    fprintf(fstat, "set xlabel 'image number'\n");
    fprintf(fstat, "set ylabel 'time to write one image [microseconds]'\n");
    fprintf(fstat, "set logscale y\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'test8_speedtest.dat' with points\n", min, max);
    fprintf(fstat, "pause -1\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'test8_speedtest.dat' with yerrorbars\n", min, max);
    fprintf(fstat, "pause -1\n");
    fclose(fstat);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);




    std::cout<<"RAW SPEED TEST, 8-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
    FILE* fraw=fopen("test8_speedtest.raw", "w");
    fstat=fopen("rawtest8_speedtest.dat", "w");
    msum=0;
    mmin=0;
    mmax=0;
    sumi=0;
    sum[0]=0;
    sum2[0]=0;
    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        fwrite(image8i, WIDTH*HEIGHT, 1, fraw);
        double duration=timer.get_time();
        sum[sumi]+=duration;
        sum2[sumi]+=duration*duration;
        msum+=duration;
        if (i%SPEEDTEST_OUTPUT==0) {
            sumi++;
            sum[sumi]=0;
            sum2[sumi]=0;
        }
        if (i==0) mmin=mmax=duration;
        else {
            if (duration>mmax) mmax=duration;
            if (duration<mmin) mmin=duration;
        }
    }
    min=0;
    max=0;
    mmean=0;
    for (int i=0; i<SPEEDTEST_SIZE/SPEEDTEST_OUTPUT; i++) {
        double NN=(double)SPEEDTEST_OUTPUT;
        double mean=sum[i]/NN;
        mmean+=mean;
        double std=sqrt((sum2[i]-sum[i]*sum[i]/NN)/(NN-1.0));
        if (i==0) min=max=mean;
        else {
            if (mean>max) max=mean;
            if (mean<min) min=mean;
        }
        //std::cout<<"  "<<(double)i/(double)((double)SPEEDTEST_SIZE/(double)SPEEDTEST_OUTPUT)*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
        fprintf(fstat, "%d, %lf, %lf\n", i*SPEEDTEST_OUTPUT, mean, std);
    }
    std::cout<<"  average time to write one image: "<<msum/(double)(SPEEDTEST_SIZE)<<" usecs    range: ["<<mmin<<".."<<mmax<<"] usecs\n";
    std::cout<<"  average image rate: "<<1/(msum/(double)(SPEEDTEST_SIZE))*1000.0<<" kHz\n";
    fclose(fstat);
    fstat=fopen("rawtest8_speedtest.plt", "w");
    fprintf(fstat, "set title 'Raw Speed Test, %d 8-bit %dx%d images, each point: average over %d images'\n", SPEEDTEST_SIZE, WIDTH, HEIGHT,
            SPEEDTEST_OUTPUT);
    fprintf(fstat, "set xlabel 'image number'\n");
    fprintf(fstat, "set ylabel 'time to write one image [microseconds]'\n");
    fprintf(fstat, "set logscale y\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'rawtest8_speedtest.dat' with points\n", min, max);
    fprintf(fstat, "pause -1\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'rawtest8_speedtest.dat' with yerrorbars\n", min, max);
    fprintf(fstat, "pause -1\n");
    fclose(fstat);
    fclose(fraw);



    std::cout<<"TIFF SPEED TEST, 16-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";

    tiff = TinyTIFFWriter_open("test16_speedtest.tif", 16, 1, WIDTH,HEIGHT);
    fstat=fopen("test16_speedtest.dat", "w");
    sum[0]=0;
    sum2[0]=0;
    msum=0;
    mmin=0;
    mmax=0;
    sumi=0;
    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        TinyTIFFWriter_writeImage(tiff, image16);
        double duration=timer.get_time();
        sum[sumi]+=duration;
        sum2[sumi]+=duration*duration;
        msum+=duration;
        if (i%SPEEDTEST_OUTPUT==0) {
            /*double NN=(double)SPEEDTEST_OUTPUT;
            double mean=sum[sumi]/NN;
            double std=sqrt((sum2[sumi]-sum[sumi]*sum[sumi]/NN)/(NN-1.0));
            std::cout<<"  "<<(double)i/(double)SPEEDTEST_SIZE*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
            fprintf(fstat, "%d, %lf, %lf\n", i, mean, std);
            */
            sumi++;
            sum[sumi]=0;
            sum2[sumi]=0;
        }
        if (i==0) mmin=mmax=duration;
        else {
            if (duration>mmax) mmax=duration;
            if (duration<mmin) mmin=duration;
        }
    }
    min=0;
    max=0;
    mmean=0;
    for (int i=0; i<SPEEDTEST_SIZE/SPEEDTEST_OUTPUT; i++) {
        double NN=(double)SPEEDTEST_OUTPUT;
        double mean=sum[i]/NN;
        mmean+=mean;
        double std=sqrt((sum2[i]-sum[i]*sum[i]/NN)/(NN-1.0));
        if (i==0) min=max=mean;
        else {
            if (mean>max) max=mean;
            if (mean<min) min=mean;
        }
        //std::cout<<"  "<<(double)i/(double)((double)SPEEDTEST_SIZE/(double)SPEEDTEST_OUTPUT)*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
        fprintf(fstat, "%d, %lf, %lf\n", i*SPEEDTEST_OUTPUT, mean, std);
    }
    std::cout<<"  average time to write one image: "<<msum/(double)(SPEEDTEST_SIZE)<<" usecs    range: ["<<mmin<<".."<<mmax<<"] usecs\n";
    std::cout<<"  average image rate: "<<1/(msum/(double)(SPEEDTEST_SIZE))*1000.0<<" kHz\n";
    fclose(fstat);
    fstat=fopen("test16_speedtest.plt", "w");
    fprintf(fstat, "set title 'TinyTIFFWrite Speed Test, %d 16-bit %dx%d images, each point: average over %d images'\n", SPEEDTEST_SIZE, WIDTH, HEIGHT, SPEEDTEST_OUTPUT);
    fprintf(fstat, "set xlabel 'image number'\n");
    fprintf(fstat, "set ylabel 'time to write one image [microseconds]'\n");
    fprintf(fstat, "set logscale y\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'test16_speedtest.dat' with points\n", min, max);
    fprintf(fstat, "pause -1\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'test16_speedtest.dat' with yerrorbars\n", min, max);
    fprintf(fstat, "pause -1\n");
    fclose(fstat);
    TinyTIFFWriter_close_withmetadatadescription(tiff, 100, 200, 300, 1e-4);





    std::cout<<"RAW SPEED TEST, 16-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
    fraw=fopen("test16_speedtest.raw", "w");
    fstat=fopen("rawtest16_speedtest.dat", "w");
    msum=0;
    mmin=0;
    mmax=0;
    sumi=0;
    sum[0]=0;
    sum2[0]=0;
    for (int i=0; i<SPEEDTEST_SIZE; i++) {
        timer.start();
        fwrite(image16i, WIDTH*HEIGHT*2, 1, fraw);
        double duration=timer.get_time();
        sum[sumi]+=duration;
        sum2[sumi]+=duration*duration;
        msum+=duration;
        if (i%SPEEDTEST_OUTPUT==0) {
            sumi++;
            sum[sumi]=0;
            sum2[sumi]=0;
        }
        if (i==0) mmin=mmax=duration;
        else {
            if (duration>mmax) mmax=duration;
            if (duration<mmin) mmin=duration;
        }
    }
    min=0;
    max=0;
    mmean=0;
    for (int i=0; i<SPEEDTEST_SIZE/SPEEDTEST_OUTPUT; i++) {
        double NN=(double)SPEEDTEST_OUTPUT;
        double mean=sum[i]/NN;
        mmean+=mean;
        double std=sqrt((sum2[i]-sum[i]*sum[i]/NN)/(NN-1.0));
        if (i==0) min=max=mean;
        else {
            if (mean>max) max=mean;
            if (mean<min) min=mean;
        }
        //std::cout<<"  "<<(double)i/(double)((double)SPEEDTEST_SIZE/(double)SPEEDTEST_OUTPUT)*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
        fprintf(fstat, "%d, %lf, %lf\n", i*SPEEDTEST_OUTPUT, mean, std);
    }
    std::cout<<"  average time to write one image: "<<msum/(double)(SPEEDTEST_SIZE)<<" usecs    range: ["<<mmin<<".."<<mmax<<"] usecs\n";
    std::cout<<"  average image rate: "<<1/(msum/(double)(SPEEDTEST_SIZE))*1000.0<<" kHz\n";
    fclose(fstat);
    fstat=fopen("rawtest16_speedtest.plt", "w");
    fprintf(fstat, "set title 'Raw Speed Test, %d 16-bit %dx%d images, each point: average over %d images'\n", SPEEDTEST_SIZE, WIDTH, HEIGHT,
            SPEEDTEST_OUTPUT);
    fprintf(fstat, "set xlabel 'image number'\n");
    fprintf(fstat, "set ylabel 'time to write one image [microseconds]'\n");
    fprintf(fstat, "set logscale y\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'rawtest16_speedtest.dat' with points\n", min, max);
    fprintf(fstat, "pause -1\n");
    fprintf(fstat, "plot [*:*] [%lf:%lf] 'rawtest16_speedtest.dat' with yerrorbars\n", min, max);
    fprintf(fstat, "pause -1\n");
    fclose(fstat);
    fclose(fraw);


#ifdef TINYTIFF_TEST_LIBTIFF
    if (SPEEDTEST_SIZE<60000) {
        std::cout<<"LIBTIFF SPEED TEST, 8-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
        TIFF* tifvideo=TIFFOpen("libtifftest8_speedtest.tif", "w");
        fstat=fopen("libtifftest8_speedtest.dat", "w");
        msum=0;
        mmin=0;
        mmax=0;
        sumi=0;
        sum[0]=0;
        sum2[0]=0;
        for (int i=0; i<SPEEDTEST_SIZE; i++) {
            timer.start();
        TIFFTWriteUint8(tifvideo, image8i, WIDTH, HEIGHT);
        TIFFWriteDirectory(tifvideo);
            double duration=timer.get_time();
            sum[sumi]+=duration;
            sum2[sumi]+=duration*duration;
            msum+=duration;
            if (i%SPEEDTEST_OUTPUT==0) {
                sumi++;
                sum[sumi]=0;
                sum2[sumi]=0;
            }
            if (i==0) mmin=mmax=duration;
            else {
                if (duration>mmax) mmax=duration;
                if (duration<mmin) mmin=duration;
            }
        }
        TIFFClose(tifvideo);
        min=0;
        max=0;
        mmean=0;
        for (int i=0; i<SPEEDTEST_SIZE/SPEEDTEST_OUTPUT; i++) {
            double NN=(double)SPEEDTEST_OUTPUT;
            double mean=sum[i]/NN;
            mmean+=mean;
            double std=sqrt((sum2[i]-sum[i]*sum[i]/NN)/(NN-1.0));
            if (i==0) min=max=mean;
            else {
                if (mean>max) max=mean;
                if (mean<min) min=mean;
            }
            //std::cout<<"  "<<(double)i/(double)((double)SPEEDTEST_SIZE/(double)SPEEDTEST_OUTPUT)*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
            fprintf(fstat, "%d, %lf, %lf\n", i*SPEEDTEST_OUTPUT, mean, std);
        }
        std::cout<<"  average time to write one image: "<<msum/(double)(SPEEDTEST_SIZE)<<" usecs    range: ["<<mmin<<".."<<mmax<<"] usecs\n";
        std::cout<<"  average image rate: "<<1/(msum/(double)(SPEEDTEST_SIZE))*1000.0<<" kHz\n";
        fclose(fstat);
        fstat=fopen("libtifftest8_speedtest.plt", "w");
        fprintf(fstat, "set title 'Raw Speed Test, %d 8-bit %dx%d images, each point: average over %d images'\n", SPEEDTEST_SIZE, WIDTH, HEIGHT,
                SPEEDTEST_OUTPUT);
        fprintf(fstat, "set xlabel 'image number'\n");
        fprintf(fstat, "set ylabel 'time to write one image [microseconds]'\n");
        fprintf(fstat, "set logscale y\n");
        fprintf(fstat, "plot [*:*] [%lf:%lf] 'libtifftest8_speedtest.dat' with points\n", min, max);
        fprintf(fstat, "pause -1\n");
        fprintf(fstat, "plot [*:*] [%lf:%lf] 'libtifftest8_speedtest.dat' with yerrorbars\n", min, max);
        fprintf(fstat, "pause -1\n");
        fclose(fstat);





        std::cout<<"LIBTIFF SPEED TEST, 16-Bit "<<SPEEDTEST_SIZE<<" images "<<WIDTH<<"x"<<HEIGHT<<" pixels\n";
        tifvideo=TIFFOpen("libtifftest16_speedtest.tif", "w");
        fstat=fopen("libtifftest16_speedtest.dat", "w");
        msum=0;
        mmin=0;
        mmax=0;
        sumi=0;
        sum[0]=0;
        sum2[0]=0;
        for (int i=0; i<SPEEDTEST_SIZE; i++) {
            timer.start();
        TIFFTWriteUint16(tifvideo, image16i, WIDTH, HEIGHT);
        TIFFWriteDirectory(tifvideo);
            double duration=timer.get_time();
            sum[sumi]+=duration;
            sum2[sumi]+=duration*duration;
            msum+=duration;
            if (i%SPEEDTEST_OUTPUT==0) {
                sumi++;
                sum[sumi]=0;
                sum2[sumi]=0;
            }
            if (i==0) mmin=mmax=duration;
            else {
                if (duration>mmax) mmax=duration;
                if (duration<mmin) mmin=duration;
            }
        }
        TIFFClose(tifvideo);
        min=0;
        max=0;
        mmean=0;
        for (int i=0; i<SPEEDTEST_SIZE/SPEEDTEST_OUTPUT; i++) {
            double NN=(double)SPEEDTEST_OUTPUT;
            double mean=sum[i]/NN;
            mmean+=mean;
            double std=sqrt((sum2[i]-sum[i]*sum[i]/NN)/(NN-1.0));
            if (i==0) min=max=mean;
            else {
                if (mean>max) max=mean;
                if (mean<min) min=mean;
            }
            //std::cout<<"  "<<(double)i/(double)((double)SPEEDTEST_SIZE/(double)SPEEDTEST_OUTPUT)*100.0<<"%     average="<<mean<<" usecs   std="<<std<<" usecs\n";
            fprintf(fstat, "%d, %lf, %lf\n", i*SPEEDTEST_OUTPUT, mean, std);
        }
        std::cout<<"  average time to write one image: "<<msum/(double)(SPEEDTEST_SIZE)<<" usecs    range: ["<<mmin<<".."<<mmax<<"] usecs\n";
        std::cout<<"  average image rate: "<<1/(msum/(double)(SPEEDTEST_SIZE))*1000.0<<" kHz\n";
        fclose(fstat);
        fstat=fopen("libtifftest16_speedtest.plt", "w");
        fprintf(fstat, "set title 'Raw Speed Test, %d 16-bit %dx%d images, each point: average over %d images'\n", SPEEDTEST_SIZE, WIDTH, HEIGHT,
                SPEEDTEST_OUTPUT);
        fprintf(fstat, "set xlabel 'image number'\n");
        fprintf(fstat, "set ylabel 'time to write one image [microseconds]'\n");
        fprintf(fstat, "set logscale y\n");
        fprintf(fstat, "plot [*:*] [%lf:%lf] 'libtifftest16_speedtest.dat' with points\n", min, max);
        fprintf(fstat, "pause -1\n");
        fprintf(fstat, "plot [*:*] [%lf:%lf] 'libtifftest16_speedtest.dat' with yerrorbars\n", min, max);
        fprintf(fstat, "pause -1\n");
        fclose(fstat);
    }
#endif




    free(image8);
    return 0;
}
