# TinyTIFF

(c) 2014-2015 by Jan W. Krieger                                                 --

This is a lightweight C/C++ library, which is able to read and write basic TIFF
files. It is significantly faster than libTIFF, especially in writing large
multi-frame TIFFs.

This software is licensed under the term of the GNU Lesser General Public License 3.0 
(LGPL 3.0). 

##TinyTIFFReader

The methods in this file allow to read TIFF files with limited capabilites, but very fast (comapred to libtiff) and also more frames from a multi-frame TIFF than libtiff (which is currently limited to 65535 frames due to internal data sizes!).
   
This library currently support TIFF files, which meet the following criteria:
* TIFF-only (no BigTIFF), i.e. max. 4GB
* uncompressed frames
* one, or more samples per frame
* data types: UINT, INT, FLOAT

This example reads all frames from a TIFF file:
```C++
   TinyTIFFReaderFile* tiffr=NULL;
   tiffr=TinyTIFFReader_open(filename); 
    if (!tiffr) { 
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; 
    } else { 
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; 
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n"; 
        uint32_t frames=TinyTIFFReader_countFrames(tiffr); 
        std::cout<<"    frames: "<<frames<<"\n"; 
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; 
        uint32_t frame=0; 
        do { 
            uint32_t width=TinyTIFFReader_getWidth(tiffr); 
            uint32_t height=TinyTIFFReader_getHeight(tiffr); 
			bool ok=true;
            if (width>0 && height>0) std::cout<<"    size of frame "<<frame<<": "<<width<<"x"<<height<<"\n"; 
            else { std::cout<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height<<"\n"; ok=false; } 
            if (ok) { 
                frame++; 
                uint16_t* image=(uint16_t*)calloc(width*height, sizeof(uint16_t));  
                TinyTIFFReader_getSampleData(tiffr, image, 0); 
                if (TinyTIFFReader_wasError(tiffr)) { ok=false; std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; } 
				
                ///////////////////////////////////////////////////////////////////
				// HERE WE CAN DO SOMETHING WITH THE IMAGE IN image (ROW-MAJOR!)
                ///////////////////////////////////////////////////////////////////
				
                free(image); 
            } 
        } while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
        std::cout<<"    read "<<frame<<" frames\n"; 
    } 
    TinyTIFFReader_close(tiffr); 
```
   
This example reads the first frame in a TIFF file:
```C++
   TinyTIFFReaderFile* tiffr=NULL;
   tiffr=TinyTIFFReader_open(filename); 
   if (!tiffr) { 
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; 
   } else { 
		uint32_t width=TinyTIFFReader_getWidth(tiffr); 
        uint32_t height=TinyTIFFReader_getHeight(tiffr); 
		uint16_t* image=(uint16_t*)calloc(width*height, sizeof(uint16_t));  
        TinyTIFFReader_getSampleData(tiffr, image, 0); 
				
                ///////////////////////////////////////////////////////////////////
				// HERE WE CAN DO SOMETHING WITH THE IMAGE IN image (ROW-MAJOR!)
                ///////////////////////////////////////////////////////////////////
				
		free(image); 
	} 
    TinyTIFFReader_close(tiffr); 
   ```
   
##TinyTIFFWriter

The methods in this file allow to write TIFF files with limited capabilites,  but very fast. Usually writing TIFF files with a library like libTIFF is relatively slow, when multiple images are written into a single file. The methods in this files overcome this problem by implementing a tiny writer lib that allows to write a TIFF file where all images have the same properties (size, bit depth, ...). This is a situation thet occurs e.g. in cases where a camera acquires a video that should be saved as TIFF file. The library works like this (write 50 32x32 pixel 8-bit images:
```C++
   TinyTIFFFile* tif=TinyTIFFWriter_open("myfil.tif", 8, 32, 32);
   if (tif) {
       for (uint16_t frame=0; frame<50; frame++) {
           uint8_t* data=readImage();
           TinyTIFFWriter_writeImage(tif, data);
       }
       TinyTIFFWriter_close(tif);
   }
```
The images are written in big- or little-endian according to your system. The TIFF header is set accordingly, so we do not need to shuffle around bytes when writing, but the created TIFF file may differ from hardware system to hardware system, although the same data is written (once in little-endian, once in big-endian). Currently this library saves all images as unsigned int, but with given bit-depth (8, 16, 32 or 64). Also this library explicitly writes a resolution of 1 in both directions.

Internally this library works like this: TinyTIFFWriter_open() will basically only initialize the internal datastructures and write the TIFF header. It also determines the byte order used by the system and sets the TIFF header acordingly. As the image size is known, the size of every image in the file can be predetermined (we assume a maximum number of TIFF directory entries). The size will be: 
```
      MAX_HEADER_ENTRIES*12 + SOME_FREE_SPACE + WIDTH*HEIGHT*(BITS_PER_SAMPLES/8)
      ---------------------------------------   ---------------------------------
          directory/image description data                 image data
```
The free space, indicated as SOME_FREE_SPACE is used to store contents of extended fields, like RATIONAL or ARRAY fields. Every image in the file will have this size and unused bytes are set to 0x00. TinyTIFFWriter_writeImage() then works like this: The image description data is first assembled in memory, then the complete image description data and the complete image data is written to the file all together. This reduces the number of file access operations and writes the data in two reltively large chunks which allows the operating system to properly optimize file access. Finally this method will save the position of the  NEXT_IFD_OFFSET field in the image header. The  NEXT_IFD_OFFSET field is filled with the adress of the next potential image. Finally the method TinyTIFFWriter_close() will write  0x00000000 into the NEXT_IFD_OFFSET of the last image (as saved above) which ends the list of images in the file. This ansatz for writing TIFF files is only about a factor of 2 slower than directly writing binary data into a file. In addition the time needed to write an image stays equal also when writing many images, which is NOT the case for libtiff. 

Here are some example benchmark data acquired using MinGW on a rather old CentrinoDuo notebook:
```
TIFF SPEED TEST, 8-Bit 1000 images 32x32 pixels
  average time to write one image: 17.1907 usecs    range: [7.82222..274.895] usecs
  average image rate: 58.1709 kHz
RAW SPEED TEST, 8-Bit 1000 images 32x32 pixels
  average time to write one image: 21.8469 usecs    range: [2.23492..299.2] usecs
  average image rate: 45.7731 kHz
TIFF SPEED TEST, 16-Bit 1000 images 32x32 pixels
  average time to write one image: 15.2676 usecs    range: [5.5873..262.044] usecs
  average image rate: 65.4983 kHz
RAW SPEED TEST, 16-Bit 1000 images 32x32 pixels
  average time to write one image: 27.5138 usecs    range: [3.63175..296.406] usecs
  average image rate: 36.3454 kHz
LIBTIFF SPEED TEST, 8-Bit 1000 images 32x32 pixels
  average time to write one image: 3024.75 usecs    range: [113.143..7161.52] usecs
  average image rate: 0.330606 kHz
LIBTIFF SPEED TEST, 16-Bit 1000 images 32x32 pixels
  average time to write one image: 3028.42 usecs    range: [120.965..10426.7] usecs
  average image rate: 0.330205 kHz
```
So this library is about a factor of 2.2 slower than direct binary output (raw) and about a factor of 500 faster than libTIFF. Note however the wide range of per-image write speeds which stems from the time the operating systems takes for file access. But the average rates are very good, so if your image creation is synchronous, you will need to use a FIFO to save images intermediately to account for the write speed jitter.
   
