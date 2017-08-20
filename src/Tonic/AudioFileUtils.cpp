//
//  AudioFileUtils.cpp
//  TonicLib
//
//  Created by Morgan Packard on 10/26/13.
//  Copyright (c) 2013 Nick Donaldson. All rights reserved.
//

#include "AudioFileUtils.h"
#include <fstream>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#else
#include <sndfile.h>
#endif

namespace Tonic {


  #ifdef __APPLE__

  void checkCAError(OSStatus error, const char *operation){
    if (error == noErr) return;
    char errorString[20];
    // See if it appears to be a 4-char-code
    *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
    if (isprint(errorString[1]) && isprint(errorString[2]) &&
        isprint(errorString[3]) && isprint(errorString[4])) {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    } else {
        // No, format it as an integer
        sprintf(errorString, "%d", (int)error);
    }
    fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
  }
  

  SampleTable loadAudioFile(string path, int numChannels){
  
    static const int BYTES_PER_SAMPLE = sizeof(TonicFloat);
    
    // Get the file handle
    ExtAudioFileRef inputFile;
    CFStringRef cfStringRef; 
    cfStringRef = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingMacRoman);
    CFURLRef inputFileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfStringRef, kCFURLPOSIXPathStyle, false);
    CFRelease(cfStringRef);
    
    checkCAError(ExtAudioFileOpenURL(inputFileURL,  &inputFile), "ExtAudioFileOpenURL failed");
    CFRelease(inputFileURL);
    
    // Define the format for the data we want to extract from the audio file
    AudioStreamBasicDescription outputFormat;
    memset(&outputFormat, 0, sizeof(outputFormat));
    outputFormat.mSampleRate = 44100.0;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mFormatFlags = kAudioFormatFlagIsFloat;
    outputFormat.mBytesPerPacket = BYTES_PER_SAMPLE * numChannels;
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = BYTES_PER_SAMPLE * numChannels;
    outputFormat.mChannelsPerFrame = numChannels;
    outputFormat.mBitsPerChannel = 32;
    OSStatus error = ExtAudioFileSetProperty(inputFile, kExtAudioFileProperty_ClientDataFormat, sizeof(AudioStreamBasicDescription), &outputFormat);
    checkCAError(error, "Error setting kExtAudioFileProperty_ClientDataFormat.");

    // Determine the length of the file, in frames
    SInt64 numFrames;
    UInt32 intSize = sizeof(SInt64);
    error = ExtAudioFileGetProperty(inputFile, kExtAudioFileProperty_FileLengthFrames, &intSize, &numFrames);
    checkCAError(error, "Error reading number of frames.");
    
    // change sampleTable numframes to long long
    SampleTable destinationTable = SampleTable((int)numFrames, numChannels);
    
    // wrap the destination buffer in an AudioBufferList
    AudioBufferList convertedData;
    convertedData.mNumberBuffers = 1;
    convertedData.mBuffers[0].mNumberChannels = outputFormat.mChannelsPerFrame;
    convertedData.mBuffers[0].mDataByteSize = (UInt32)destinationTable.size() * BYTES_PER_SAMPLE;
    convertedData.mBuffers[0].mData = destinationTable.dataPointer();
    
    UInt32 numFrames32 = (UInt32)numFrames;
    ExtAudioFileRead(inputFile, &numFrames32, &convertedData);
    
    ExtAudioFileDispose(inputFile);
    
    return destinationTable;
    
  }
  
  #else
  
  SampleTable loadAudioFile(string path, int numChannels)
  {
      /*
      int       sampleChunkSize;
      int		sampleSubChunk1Size;
      short 	sampleFormat;
      short 	sampleChannels;
      int   	sampleSampleRate;
      int   	sampleByteRate;
      short 	sampleBlockAlign;
      short 	sampleBitsPerSample;
      int		sampleDataSize;

       ifstream inFile( path.c_str(), ios::in | ios::binary);

       cout << "Reading file %s" << path.c_str() << endl;

       if(!inFile.is_open())
       {
           cout << "Error opening file. File not loaded." << endl;
           return NULL;
       }

       char id[4];
       inFile.read((char*) &id, 4);
       if(strncmp(id,"RIFF",4) != 0)
       {
           cout << "Error reading sample file. File is not a RIFF (.wav) file" << endl;
           return NULL;
       }

       inFile.seekg(4, ios::beg);
       inFile.read( (char*) &sampleChunkSize, 4 ); // read the ChunkSize

       inFile.seekg(16, ios::beg);
       inFile.read( (char*) &sampleSubChunk1Size, 4 ); // read the SubChunk1Size

       //inFile.seekg(20, ios::beg);
       inFile.read( (char*) &sampleFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM
       if(sampleFormat != 1) {
           cout << "File format should be PCM, sample file failed to load." << endl;
           return NULL;
       }

       //inFile.seekg(22, ios::beg);
       inFile.read( (char*) &sampleChannels, sizeof(short) ); // read the # of channels (1 or 2)

       //inFile.seekg(24, ios::beg);
       inFile.read( (char*) &sampleSampleRate, sizeof(int) ); // read the Samplerate

       //inFile.seekg(28, ios::beg);
       inFile.read( (char*) &sampleByteRate, sizeof(int) ); // read the byterate

       //inFile.seekg(32, ios::beg);
       inFile.read( (char*) &sampleBlockAlign, sizeof(short) ); // read the blockalign

       //inFile.seekg(34, ios::beg);
       inFile.read( (char*) &sampleBitsPerSample, sizeof(short) ); // read the bitsperSample

       inFile.seekg(40, ios::beg);
       inFile.read( (char*) &sampleDataSize, sizeof(int) ); // read the size of the data
       //cout << sampleDataSize << endl;

        // read the data chunk
       char* myData = new char[sampleDataSize];
       inFile.seekg(44, ios::beg);
       inFile.read(myData, sampleDataSize);
       inFile.close(); // close the input file

       // change sampleTable numframes to long long
       SampleTable destinationTable = SampleTable((int)sampleDataSize, sampleChannels);
       float* data;
       data = destinationTable.dataPointer();



       delete myData;
       */

      #define FRAMES_PER_BUFFER	1024

      SNDFILE    		*infile;
      SF_INFO    		sfinfo ;
      long              readcount;
      long              numSamples;
      static float tempbuf[FRAMES_PER_BUFFER];


      /* Open sample file for reading */
      if (! (infile = sf_open (path.c_str(), SFM_READ, &sfinfo)))
      {   /* Open failed so print an error message. */
          cout << "Not able to open input file " << path << endl;
          cout <<  (sf_strerror (NULL)) << endl;
          exit(1) ;
      }

      long frames =  sfinfo.frames;
      int sampleChannels = sfinfo.channels;
      numSamples = frames * sampleChannels;

      SampleTable destinationTable = SampleTable(static_cast<unsigned int>(sfinfo.frames), static_cast<unsigned int>(sampleChannels));

      float* data = destinationTable.dataPointer();

      if(sampleChannels == 2) {
          while ((readcount = sf_read_float (infile, tempbuf, FRAMES_PER_BUFFER)))
          {
              for (unsigned int i=0;i < readcount;i++) {
                  *data++ = tempbuf[i];
              }
          }
      } else if(sampleChannels == 1 ) {
          while ((readcount = sf_read_float (infile, tempbuf, FRAMES_PER_BUFFER)))
          {
              for (unsigned int i=0;i < readcount;i++) {
                  *data++ = tempbuf[i];
                  *data++ = tempbuf[i];
              }
          }
      } else {
          cout << "Wrong number of channels: " << sampleChannels << endl;
          exit(1);
      }

      cout << "name = " << path << "  frames: " << frames << "  samples: " << numSamples << "  channels: " << sampleChannels << endl;

      /* Close input file */
      sf_close (infile) ;


    return destinationTable;
  }
  
  
  #endif
  
}
