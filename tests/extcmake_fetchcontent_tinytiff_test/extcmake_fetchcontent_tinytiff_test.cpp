#include "tinytiffwriter.h"

using namespace std;


int main() {
    TinyTIFFWriterFile* tif=TinyTIFFWriter_open("extcmake_tinytiff_test.tif", 8, 1, 32, 32, TinyTIFFWriter_Greyscale);
	if (tif) {
		uint8_t* data=(uint8_t*)calloc(32*32,sizeof(uint8_t));
        TinyTIFFWriter_writeImage(tif, data);
		free(data);
        TinyTIFFWriter_close(tif);
    }
    return 0;
}
