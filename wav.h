/********************************************************/
/*							*/
/* wav.h						*/
/*							*/
/* Logan Blakely					*/
/* January 2017						*/
/* 							*/
/* Header file for wavFunc.c				*/
/*							*/
/********************************************************/

#include <inttypes.h>
#include "constants.h"
#include <complex.h>
#include <stddef.h>


//
// Structure typedef for the .wav file header information
//

typedef struct {

   char chunkID[4];
   uint32_t chunkSize;
   char format[4];
   char subChunk1ID[4];
   uint32_t subChunk1Size;
   uint16_t audioFormat;
   uint16_t numChannels;
   uint32_t sampleRate;
   uint32_t byteRate;
   uint16_t blockAlign;
   uint16_t bitsPerSample;
   char subChunk2ID[4];
   uint32_t subChunk2Size;

} HEADER;



//
//Function Prototypes
//

void wavTrainingInput(int16_t **, HEADER *,int, int, int *);
void wavTrainingInputControl(int16_t **, HEADER *, int);
void printWavHeader(int16_t **, HEADER *, int);
void printWavSamples(int16_t **, HEADER *, int);
void squashWavSamples(int16_t **, int, HEADER *, int16_t **);
int wavToComplex(float complex *, int16_t *);
int printComplexSamples(float complex*,int, int);
int transformToFreq(float complex *);
int determineComPadding(int);
int getFreqInfo(float complex**, int, int16_t **, HEADER *);
int printFreqBins(int16_t **, int);
int grabAllWavSections(int16_t **, int16_t **, HEADER *, int, int);
int printWavSections(int16_t **, int);
int grabWavSectionWindows(int16_t *, int16_t *, int);
int printWavSectionWindow(int16_t *, int, int);
int getFreqData(int16_t **, double **, HEADER *, int);
int calculateFreqMagnitude(float complex *, double **, int);
int printFreqMagnitude(double **, int, int);
int printAverageMagn(double **, int);
int averageFreqWindows(double **, double **, int);
int printAmplFreqData(double **, int);
int joinAmplFreqData(double **, double **, int16_t **, int);




