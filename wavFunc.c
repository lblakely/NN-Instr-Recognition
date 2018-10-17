/****************************************************************/
/*								*/
/* wavFunc.c							*/
/* 								*/
/* Logan Blakely						*/
/* January-March  2017						*/
/*								*/
/* Functions to read a .wav file and record the header 		*/
/*   information in a header structure and the audio samples    */
/*   in an array of integers, read sections of data, transform  */
/*   the data to the frequency domain, and join the amplitude   */
/*   and frequency data together.				*/
/*								*/
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "wav.h"
#include "fft.h"
#include <complex.h>
#include <math.h>
#include "nn.h"

extern char trainingFilenames[][100];
extern char testFilenames[][100];
/****************************************************************/
/*								*/
/* wavTrainingInputControl Function				*/
/*								*/
/* Takes pointers to the array of samples and the array of      */
/*    headers, and the number of clips to read as parameters.   */
/*    Controls the input of of the training set of audio        */
/*    samples.      		  				*/
/*   								*/
/****************************************************************/

void wavTrainingInputControl(int16_t **samples, HEADER *headers, int clipNumber)
{

   int trainingCtr = 0;
   int minSamples = 1000000;
   int numberOfSamples = 1000000;

   while(trainingCtr < clipNumber)
   {
      
      wavTrainingInput(samples, headers, clipNumber,trainingCtr, &numberOfSamples);
      trainingCtr++;

      if (numberOfSamples < minSamples)
      {
	 minSamples = numberOfSamples;
      }
     // printf("minSamples = %d \n", minSamples);
      numberOfSamples = 0;
   }
   printf("minSamples = %d \n", minSamples);
   
}//end of wavControl



/****************************************************************/
/*								*/
/* wavTrainingInput						*/
/*								*/
/* Takes pointers to the array of samples and the array of 	*/
/*   headers, the number of clips, the current clip number,and  */
/*   a pointer to the numberOfSamples variable as parameters.   */
/*   Reads in the contents of one .wav file.			*/
/*								*/
/****************************************************************/

void wavTrainingInput(int16_t **samples, HEADER *header, int clipNumber, int trainingCtr, int *numberOfSamples)
{
   int filedescriptor;
   int16_t *newSamples;

   //
   //Open file and read the header information
   //
   if(clipNumber == TRAININGSAMPLES)
   {
      filedescriptor = open(trainingFilenames[trainingCtr], O_RDONLY);

      if(filedescriptor == -1)
      {
	 printf("Fatal Error reading training file: file <%s> does not exist\n", trainingFilenames[trainingCtr]);
	 exit(1);
      }
      read(filedescriptor, &header[trainingCtr], sizeof(HEADER));
   }
   else
   {
      filedescriptor = open(testFilenames[trainingCtr], O_RDONLY);
      if(filedescriptor == -1)
      {
	 printf("Fatal Error reading test file: file <%s> does not exist\n", testFilenames[trainingCtr]);
	 exit(1);
      }
      read(filedescriptor, &header[trainingCtr], sizeof(HEADER));
   }

   //
   //Read in the audio samples from the file
   //
   newSamples = malloc(header[trainingCtr].subChunk2Size);
   samples[trainingCtr] = newSamples;
   read(filedescriptor, newSamples, header[trainingCtr].subChunk2Size);
   *numberOfSamples = (header[trainingCtr].subChunk2Size) / 2;
   
   close(filedescriptor);

} //end of wavInput



/****************************************************************/
/*								*/
/* printWavHeader						*/
/*								*/
/* takes pointers to the array of audio samples, the array of   */
/*    headers, and a counter value of which sample to print.    */
/*    Prints the header info for the given sample.		*/
/*								*/
/****************************************************************/

void printWavHeader(int16_t **samples, HEADER *headers, int trainingCtr)
{
   int numberOfSamples;

   numberOfSamples = headers[trainingCtr].subChunk2Size / 2;

   printf("\n\nHeader Information for headers[%d] \n", trainingCtr);
   printf("chunkID = %s\n", headers[trainingCtr].chunkID);
   printf("chunkSize = %d\n", headers[trainingCtr].chunkSize);
   printf("format = %s \n", headers[trainingCtr].format);
   printf("subChunk1ID = %s\n", headers[trainingCtr].subChunk1ID);
   printf("subChunk1Size = %d\n", headers[trainingCtr].subChunk1Size);
   printf("audioFormat = %d\n", headers[trainingCtr].audioFormat);
   printf("numChannels = %d\n", headers[trainingCtr].numChannels);
   printf("sampleRate = %d\n", headers[trainingCtr].sampleRate);
   printf("byteRate = %d\n", headers[trainingCtr].byteRate);
   printf("blockAlign = %d\n", headers[trainingCtr].blockAlign);
   printf("bitsPerSample = %d\n", headers[trainingCtr].bitsPerSample);
   printf("subChunk2ID = %s \n", headers[trainingCtr].subChunk2ID);
   printf("subChunk2Size = %d \n", headers[trainingCtr].subChunk2Size);
   printf("numberOfSamples = %d \n", numberOfSamples);
   printf("\n");

}//end of printWavHeader


/********************************************************/
/*							*/
/* printWavSamples					*/
/*							*/
/*  takes pointers to the array of audio samples, the   */
/*     array of headers, and a counter value of which   */
/*     wav samples to print.  Prints all audio samples  */
/*     from the given sample in the array.		*/
/*							*/
/********************************************************/

void printWavSamples(int16_t **samples, HEADER *headers, int trainingCtr)
{
   int samplesCtr = 0;
   int numberOfSamples;

   numberOfSamples = headers[trainingCtr].subChunk2Size / 2;

   printf("List of Audio Samples from samples[%d]\n",trainingCtr);
   while(samplesCtr < numberOfSamples)
   {
      printf("%2x, ", samples[trainingCtr][samplesCtr]);
      samplesCtr++;
   }
   printf("\n\n");
}//end of printWavSamples

/********************************************************************************/
/*										*/
/* squashWavSamples								*/
/*										*/
/* Takes a pointer to the int16_t array of audio clips, the number of clips, a  */
/*    pointer to the array of clip header info, and a pointer to a new (smaller)*/
/*    array to hold the squashed samples.					*/
/*										*/
/* Takes a number of adjacent samples and averages them together to create a    */
/*    smaller number of samples per clip.  The number of samples to average is  */
/*    defined by the constant SQUASHFACTOR in constants.h			*/
/*										*/
/********************************************************************************/

void squashWavSamples(int16_t **samples, int totalClips,HEADER *headers, int16_t **newSamples)
{
   int samplesCtr = 0;
   int clipsCtr;
   int avgCtr;
   int numberOfSamples;
   int squashedSamples;
   int sum = 0;
   int upperLimit;

   for(clipsCtr = 0; clipsCtr < totalClips; clipsCtr++)
   {
      newSamples[clipsCtr] = malloc(sizeof(int16_t) * squashedSamples);
      numberOfSamples = headers[0].subChunk2Size /2;
      squashedSamples = numberOfSamples / SQUASHFACTOR;

      for(samplesCtr = 0; samplesCtr < squashedSamples; samplesCtr++)
      {
	 upperLimit = ((samplesCtr * SQUASHFACTOR) + SQUASHFACTOR);
	 for(avgCtr = (samplesCtr * SQUASHFACTOR); avgCtr < upperLimit; avgCtr++)
	 {
	    sum = sum + samples[clipsCtr][avgCtr];
	 }
	 newSamples[clipsCtr][samplesCtr] = (sum / SQUASHFACTOR);
	 sum = 0; 
      }
   }
}//end of squashWavSamples


/************************************************************************/
/*									*/
/* wavToComplex								*/
/*									*/
/*  Take a pointer to an entry in the array of amplitude section data,  */
/*  and a pointer to the an entry in the new complex array.		*/
/*									*/
/*  Function just transfers the int16_t array values into the real      */
/*    portion of the complex array.					*/
/*									*/
/************************************************************************/

int wavToComplex(float complex *comSamples, int16_t *amplSectionsamples)
{
   int sampleCtr;
   int padding;
   int totalSpace;
      
      for(sampleCtr = 0; sampleCtr < FREQSECTIONLENGTH; sampleCtr++)
      {
	 comSamples[sampleCtr] = amplSectionsamples[sampleCtr];
      }
      
      //
      //The fft function requires that the section be a power of 2, so pad the difference with 0's
      //
      padding = determineComPadding(FREQSECTIONLENGTH);
      totalSpace = FREQSECTIONLENGTH + padding; 
      while(sampleCtr < totalSpace)
      {
	 comSamples[sampleCtr] = 0;
	 sampleCtr++;
      }
   
  return (0);
} //end of wavToComplex function 



/********************************************************************************/
/*										*/
/* printComplexSamples								*/
/*										*/
/* Takes a pointer to a single audio clip, the number of the clip and the       */
/*   frequency window number as parameters.                                     */
/*										*/
/* Function prints out the complex values of the audio clip			*/
/*										*/
/********************************************************************************/

int printComplexSamples(float complex *comSamples, int clipNumber, int windowNumber)
{
   int sampleCtr;
   int padding;
   int totalSamples;
   
   padding = determineComPadding(FREQSECTIONLENGTH);
   totalSamples = FREQSECTIONLENGTH + padding; 
   for(sampleCtr = 0; sampleCtr < totalSamples; sampleCtr++) 
   {
      printf("%.2f%+.2fi,  ", creal(comSamples[sampleCtr]), cimag(comSamples[sampleCtr]));
   }
   printf("\n\n");
   
   return (0);
} //end of printComplex Samples


/********************************************************************************/
/* 										*/
/* transformToFreq								*/
/*										*/
/* Takes a pointer to the array of a single window of complex amplitude samples */
/*										*/
/* Function takes an complex audio section which is amplitude in time and uses a*/
/*    Fast Fourier Transform function to transform them into the frequency      */
/*    domain.									*/
/*										*/
/* The Fast Fourier Transform function is copyright David Barina of MIT   	*/
/*   I have included his license information in the fft.h file.			*/
/*										*/
/********************************************************************************/
int transformToFreq(float complex *comSamples)
{
   size_t N;
   int returnVal = 9;
   int totalSamples;
   int padding;

   //
   //Runs the UFFT Fast Fourier Transform function which replaces the amplitude samples in comSamples with the frequency values

   padding = determineComPadding(FREQSECTIONLENGTH);
   totalSamples = FREQSECTIONLENGTH + padding;

   N = totalSamples;
   returnVal = fft(comSamples, N);
   if(returnVal != 0)
   {
      printf("FATAL ERROR:  fft function failed\n");
      exit(-1);
   }

  return (0);
}  //end of transformToFreq 


/************************************************************************/
/*									*/
/* determineComPadding							*/
/*									*/
/* Function returns an integer value of the number of cells that need   */
/*    to be added to the end of the clips to bring the number of        */
/*    samples up to a power of two.  The UFFT Fast Fourier Transform    */
/*    function requires the number of samples to be a power of 2.       */
/*									*/
/************************************************************************/

int determineComPadding(int sampleNumber)
{
   int padding = 0;
   int exponent = 1;

   while(pow(2,exponent) < sampleNumber)
   {
      exponent++;
   }
   padding = pow(2,exponent) - sampleNumber;
   return(padding);
} //end of determineComPadding



/************************************************************************/
/*									*/
/* grabAllWavSectionWindows						*/
/*									*/
/* Take a pointer to all 'full-length' amplitude audio samples, a       */ 
/*    pointer to storage for all sections of amplitude samples,(one     */ 
/*    section for each sample, a pointer to the header info, the clip   */
/*    number and the section length.  					*/
/* Function pulls out a single section of amplitude samples for all of  */
/*    the audio clips, starting at the middle of the clip.		*/
/*									*/
/************************************************************************/

int grabAllWavSections(int16_t **audioSamples, int16_t **sectionSamples, HEADER *headers, int clipNumber, int sectionLength)
{
   int startCtr;
   int end;
   int clipCtr;
   int sampleCtr;
   int numberOfSamples;
   int sectionSamplesCtr;

   for(clipCtr = 0; clipCtr < clipNumber; clipCtr++)
   {
      numberOfSamples = ((headers[clipCtr].subChunk2Size / 2)  / SQUASHFACTOR);

      //Grabs a section starting from the middle of the sample
      startCtr = numberOfSamples / 2;
      end = startCtr + sectionLength;
      sectionSamplesCtr = 0;
      for(sampleCtr = startCtr; sampleCtr < end; sampleCtr++)
      {
	 sectionSamples[clipCtr][sectionSamplesCtr] = audioSamples[clipCtr][sampleCtr];
	 sectionSamplesCtr++;
      }
   }
  return (0);
}//end of grabAllWavSections 



/************************************************************************/
/*									*/
/*  printWavSections							*/
/*									*/
/*  Takes a pointer to the array of amplitude section samples generated */
/*     by the function grabAllWavSections, and the number of the clip   */
/*     to print. 							*/
/*  Function prints out the section of amplitude samples for the clip   */
/*     number provided.							*/
/*									*/
/************************************************************************/

int printWavSections(int16_t **sectionSamples, int clipNumber)
{
   int clipCtr;
   int sampleCtr;

   printf("Printing the Section Audio Samples\n\n");
   for(clipCtr = 0;clipCtr < clipNumber; clipCtr++)
   {
      printf("clip # %d:  ", clipCtr);
      for(sampleCtr = 0; sampleCtr < AMPLSECTIONLENGTH; sampleCtr++)
      {
	 printf("%d ",sectionSamples[clipCtr][sampleCtr]);
      }
      printf("\n");
   }
   printf("\n\n");
   return (0);
}//end of printWavSections

/************************************************************************/
/*									*/
/* grabWavSectionWindows						*/
/*									*/
/* Take a pointer to one 'full-length' amplitude audio samples, a       */ 
/*    pointer to a temporary storage for a section/window of amplitude  */ 
/*    samples, and the window number.  					*/
/* Function computes a single window/section of amplitude samples,      */
/*    determined by FREQSECTIONLENGTH, called from getFreqData		*/
/*									*/
/************************************************************************/

int grabWavSectionWindows(int16_t *audioSamples, int16_t *tempAmplSection, int windowNumber)
{
   int startCtr;
   int end;
   int sampleCtr;
   int sectionSamplesCtr;
   
   startCtr = windowNumber * FREQSECTIONLENGTH; 
   end = startCtr + FREQSECTIONLENGTH;
   sectionSamplesCtr = 0;

   for(sampleCtr = startCtr; sampleCtr < end; sampleCtr++)
   {
      tempAmplSection[sectionSamplesCtr] = audioSamples[sampleCtr];
      sectionSamplesCtr++;
   }

  return (0);
}//end of grabWavSectionWindows



/********************************************************************************/
/*										*/
/* printWavSectionWindow							*/
/* 										*/
/* Takes a pointer to temporary storage for one section/window of amplitude     */
/*    samples, the clip number, and window number (for printing purposes).	*/
/*										*/
/* Function prints out the amplitude samples for the given section/window	*/
/*										*/
/********************************************************************************/

int printWavSectionWindow(int16_t *tempAmplSection, int clipCtr, int windowCtr)
{
   int sampleCtr;

   printf("Printing Amplitude data for window #%d of Clip #%d \n", windowCtr, clipCtr);
   for(sampleCtr = 0; sampleCtr < FREQSECTIONLENGTH; sampleCtr++)
   {
      printf("%d ", tempAmplSection[sampleCtr]);
   }
   printf("\n\n");

   return (0);
}//end of printWavSectionWindow


int calculateFreqMagnitude(float complex *comSamples, double **freqMagnWindows,int windowCtr)
{
   int sampleCtr;

   for(sampleCtr = 0; sampleCtr < BINNUMBER; sampleCtr++)
   {
      freqMagnWindows[windowCtr][sampleCtr] = sqrt(  (pow(creal(comSamples[sampleCtr]),2)) + (pow(cimag(comSamples[sampleCtr]),2))  );  

   }

   return (0);
}//end of calculateFreqMagnitude

int printFreqMagnitude(double **freqMagnWindows, int clipCtr, int windowCtr)
{
   int sampleCtr;

   printf("Frequency Magnitude Values for Clip #%d, Window #%d \n", clipCtr, windowCtr);

   for(sampleCtr = 0; sampleCtr < BINNUMBER; sampleCtr++)
   {
      printf("%.2f ", freqMagnWindows[windowCtr][sampleCtr]);
   }
   printf("\n\n");

   return (0);
}//end of printFreqMagnitude



int averageFreqWindows(double **freqMagnWindows, double **freqMagnAudio, int clipCtr)
{
   int sampleCtr;
   int windowCtr;
   double sum;

   for(sampleCtr = 0; sampleCtr < BINNUMBER; sampleCtr++)
   {
      //printf("sampleCtr = %d for clip#%d\n", sampleCtr, clipCtr);
      sum = 0;
      for(windowCtr = 0; windowCtr < NUMFREQWINDOWS; windowCtr++)
      {
	 sum = sum + freqMagnWindows[windowCtr][sampleCtr];
      }

      freqMagnAudio[clipCtr][sampleCtr] = sum / NUMFREQWINDOWS;

   }


   return (0);
}//end of averageFreqWindows

int printAverageMagn(double **freqMagnAudio, int clipCtr)
{

   int sampleCtr;

   printf("Average Magnitude Frequency Values for Clip #%d\n", clipCtr);
   for(sampleCtr = 0; sampleCtr < BINNUMBER; sampleCtr++)
   {
      printf("%f ", freqMagnAudio[clipCtr][sampleCtr]);
   }
   printf("\n\n");

   return (0);
}//end of printAverageMagn


int getFreqData(int16_t **fullAmplAudio, double **freqMagnAudio, HEADER *headers, int clipNumber)
{
   int ctr;
   int clipCtr;
   int windowCtr;
   double *tempPtr;

   float complex *tempComFreqSection;
   int16_t *tempAmplSection;
   double **freqMagnWindows;


   tempComFreqSection = malloc(sizeof(float complex) * FREQSECTIONLENGTH);
   tempAmplSection = malloc(sizeof(int16_t) * FREQSECTIONLENGTH);
   freqMagnWindows = malloc(sizeof(double *) * NUMFREQWINDOWS); 

   for(ctr = 0; ctr < NUMFREQWINDOWS; ctr++)
   {
      tempPtr = malloc(sizeof(double) * BINNUMBER);
      freqMagnWindows[ctr] = tempPtr;
   }

   printf("BINNUMBER = %d, BINWIDTH = %d, clipNumber = %d \n", BINNUMBER, BINWIDTH, clipNumber); 

   for(clipCtr = 0; clipCtr < clipNumber; clipCtr++)
   {
      for(windowCtr = 0; windowCtr < NUMFREQWINDOWS; windowCtr++)
      { 
	 grabWavSectionWindows(fullAmplAudio[clipCtr], tempAmplSection, windowCtr);
	 //printWavSectionWindow(tempAmplSection, clipCtr, windowCtr);
	 wavToComplex(tempComFreqSection, tempAmplSection);
	 transformToFreq(tempComFreqSection);
	 //printComplexSamples(tempComFreqSection, clipCtr, windowCtr);
	 calculateFreqMagnitude(tempComFreqSection,freqMagnWindows,windowCtr);
         //printFreqMagnitude(freqMagnWindows, clipCtr, windowCtr);	 
      }

      //printf("finished clip#%d windows\n", clipCtr);
      averageFreqWindows(freqMagnWindows, freqMagnAudio, clipCtr); 
      //printAverageMagn(freqMagnAudio, clipCtr);



   }



  return (0);
} //end of getFreqData 


int joinAmplFreqData(double **amplFreqData, double **freqData, int16_t **amplData, int clipNumber)
{
   int sampleCtr;
   int clipCtr;
   int freqSampleCtr;

   printf("Joining Ampl and Freq\n");
   for(clipCtr = 0; clipCtr < clipNumber; clipCtr++)
   {
      //Move Amplitude portion of the data
      for(sampleCtr = 0; sampleCtr < AMPLSECTIONLENGTH; sampleCtr++)
      {
	 amplFreqData[clipCtr][sampleCtr] = amplData[clipCtr][sampleCtr]; 
      }

      //Append frequency portion of the data
      for(freqSampleCtr = 0; freqSampleCtr < BINNUMBER; freqSampleCtr++)
      {
	 amplFreqData[clipCtr][sampleCtr] = freqData[clipCtr][freqSampleCtr];
	 sampleCtr++;
      }
   }

  return (0);
}//end of joinAmplFreqData 


int printAmplFreqData(double **amplFreqData, int clipNumber)
{
   int clipCtr;
   int sampleCtr;
   int totalSamples = AMPLSECTIONLENGTH + BINNUMBER;

   for(clipCtr = 0; clipCtr < clipNumber; clipCtr++)
   {
      printf("Amplitude and Frequency Inputs together, Clip #%d\n", clipCtr);

      for(sampleCtr = 0; sampleCtr < totalSamples; sampleCtr++)
      {
	 if(sampleCtr == AMPLSECTIONLENGTH)
	 {
	    printf("*****");
	 }
	 printf("%.2f, ", amplFreqData[clipCtr][sampleCtr]);

      }

      printf("\n\n");
   }

   return(0);
}//end of printAmplFreqData

















