/************************************************/
/* 						*/
/* Logan Blakely				*/
/* December 2016				*/
/*						*/
/* Neural Network example driver		*/
/*						*/
/* Code taken from Jones Ch5			*/
/*						*/
/************************************************/

//new comment

#include <stdio.h>
#include "nn.h"
#include "trainingSet.h"
#include <time.h>
#include "wav.h"
#include "filenames.h"
#include "constants.h"
#include <complex.h>
#include <stddef.h>



char *strings[2] = {"Violin", "Trumpet"};

int main()
{
   double err;
   double errorBoundary;
   int errCtr;
   int cycleCtr;
   float averageAccuracy;
   int i;
   int sample;
   int iterations;
   int totalIterations;
   int sum = 0;
   int ctr;
   int outCtr;
   int padding;
   int resultsIndex;
   int longCyclesCtr;


   int16_t **trainingAudioSamples;
   int16_t **squashedTrainingAudio;
   int16_t **testAudioSamples;
   int16_t **squashedTestAudio;
   int16_t **freqRawTrainingAudio;
   int16_t **freqRawTestAudio;
   int16_t **sectionAmplTrainingAudio;
   int16_t **sectionAmplTestAudio;
   HEADER *trainingHeaders;
   HEADER *testHeaders;

   double **freqMagnTrainingAudio;  //frequency magnitude samples from the amplitude sections
   double **freqMagnTestAudio;     //Frequency Magnitude sampls from the amplitude sections
   double **freqAmplTrainingAudio; //Amplitude section samples with the frequency magnitude samples appended to the end
   double **freqAmplTestAudio;     //Amplitude section samples with the freqency magnitude samples appended to the end



   int16_t *tempPtr;
   double *tempDblPtr;
   int tempNumSamples = 0;

   int numberOfSamples = 0;

   float complex **comTrainingSamples;
   float complex **comTestSamples;
   double cycleActualOutputs[TOTALCYCLES][TESTSAMPLES][OUTPUT_NEURONS];
   double cycleAverageActualOutputs[TESTSAMPLES][OUTPUT_NEURONS];


   trainingAudioSamples = malloc(sizeof(int16_t*) * TRAININGSAMPLES);
   squashedTrainingAudio = malloc(sizeof(int16_t*) * TRAININGSAMPLES);
   testAudioSamples = malloc(sizeof(int16_t*) * TESTSAMPLES);
   squashedTestAudio = malloc(sizeof(int16_t*) * TESTSAMPLES);
   freqRawTrainingAudio = malloc(sizeof(int16_t*) * TRAININGSAMPLES);
   freqRawTestAudio = malloc(sizeof(int16_t*) * TESTSAMPLES);
   sectionAmplTrainingAudio = malloc(sizeof(int16_t*) * TRAININGSAMPLES);
   sectionAmplTestAudio = malloc(sizeof(int16_t*) * TESTSAMPLES);
   trainingHeaders = malloc(sizeof(HEADER) * TRAININGSAMPLES);
   testHeaders = malloc(sizeof(HEADER) * TESTSAMPLES);

   freqMagnTrainingAudio = malloc(sizeof(double*) * TRAININGSAMPLES);
   freqMagnTestAudio = malloc(sizeof(double*) * TESTSAMPLES);
   freqAmplTrainingAudio = malloc(sizeof(double*) * TRAININGSAMPLES);
   freqAmplTestAudio = malloc(sizeof(double*) * TESTSAMPLES);

   comTrainingSamples = malloc(sizeof(float complex*) * TRAININGSAMPLES);
   comTestSamples = malloc(sizeof(float complex*) * TESTSAMPLES);

   /* Read in the audio samples */
   printf("reading training samples \n");
   wavTrainingInputControl(trainingAudioSamples, trainingHeaders, TRAININGSAMPLES);

   printf("reading test samples \n");
   wavTrainingInputControl(testAudioSamples, testHeaders,TESTSAMPLES); 

   printf("squashing the training samples\n");
   squashWavSamples(trainingAudioSamples,TRAININGSAMPLES, trainingHeaders,squashedTrainingAudio);

   printf("squashing the test samples \n");  
   squashWavSamples(testAudioSamples,TESTSAMPLES, testHeaders,squashedTestAudio);

   /* Allocate space for the complex number arrays for the audio samples */   
   numberOfSamples = ((trainingHeaders[0].subChunk2Size / 2)  / SQUASHFACTOR);    //This is used in the general case where all clips 
   										//    have the same number of samples
   printf("numberOfSamples = %d \n", numberOfSamples);


   for(ctr = 0; ctr < (TRAININGSAMPLES); ctr++)
   {

      tempNumSamples = ( (trainingHeaders[ctr].subChunk2Size / 2) / SQUASHFACTOR);
      padding = determineComPadding(tempNumSamples);
      comTrainingSamples[ctr] = malloc(sizeof(float complex) * (tempNumSamples + padding ));
   }

   for(ctr=0; ctr < (TESTSAMPLES); ctr++)
   {

      tempNumSamples = ( (testHeaders[ctr].subChunk2Size / 2) / SQUASHFACTOR);
      padding = determineComPadding(tempNumSamples);
      comTestSamples[ctr] = malloc(sizeof(float complex) * (tempNumSamples + padding ));
   }

   for(ctr = 0; ctr < (TRAININGSAMPLES); ctr++)
   {

      tempNumSamples = ( (trainingHeaders[ctr].subChunk2Size / 2) / SQUASHFACTOR);
      padding = determineComPadding(tempNumSamples);
      tempPtr = malloc(sizeof(int16_t) * (tempNumSamples + padding));
      freqRawTrainingAudio[ctr] = tempPtr;
   }

   for(ctr = 0; ctr < (TESTSAMPLES); ctr++)
   {

      tempNumSamples = ( (testHeaders[ctr].subChunk2Size / 2) / SQUASHFACTOR);
      padding = determineComPadding(tempNumSamples);
      tempPtr= malloc(sizeof(int16_t) * (tempNumSamples + padding) );
      freqRawTestAudio[ctr] = tempPtr;
   }

   for(ctr = 0; ctr < (TRAININGSAMPLES); ctr++)
   {
      tempPtr = malloc(sizeof(int16_t) * (AMPLSECTIONLENGTH));
      sectionAmplTrainingAudio[ctr] = tempPtr;
   }

   for(ctr=0;ctr<TESTSAMPLES; ctr++)
   {
      tempPtr = malloc(sizeof(int16_t) * (AMPLSECTIONLENGTH));
      sectionAmplTestAudio[ctr] = tempPtr;
   }

   for(ctr = 0; ctr < TRAININGSAMPLES; ctr++)
   {
      tempDblPtr = malloc(sizeof(double) * (FREQSECTIONLENGTH / 2));
      freqMagnTrainingAudio[ctr] = tempDblPtr;
   }

   for(ctr = 0; ctr < TESTSAMPLES; ctr ++)
   {
      tempDblPtr = malloc(sizeof(double) * (FREQSECTIONLENGTH /2));
      freqMagnTestAudio[ctr] = tempDblPtr;
   }

   printf("BINNUMBER = %d \n", BINNUMBER);
   for(ctr = 0; ctr < TRAININGSAMPLES; ctr++)
   {
      tempDblPtr = malloc(sizeof(double) * (AMPLSECTIONLENGTH + BINNUMBER));
      freqAmplTrainingAudio[ctr] = tempDblPtr;
   }

   for(ctr = 0; ctr < TESTSAMPLES; ctr++)
   {
      tempDblPtr = malloc(sizeof(double) * (AMPLSECTIONLENGTH + BINNUMBER));
      freqAmplTestAudio[ctr] = tempDblPtr;
   }


   //
   //Allocate space for the neurons and weights
   //
   wih = malloc((INPUT_NEURONS+1) * (sizeof(double*)));
   for(ctr = 0; ctr < (INPUT_NEURONS+1); ctr++)
   {
     wih[ctr] = malloc(HIDDEN_NEURONS * sizeof(double));
   } 

   who = malloc((HIDDEN_NEURONS+1) * sizeof(double*));
   for(ctr = 0; ctr < (HIDDEN_NEURONS+1); ctr++)
   {
      who[ctr] = malloc(OUTPUT_NEURONS * sizeof(double));
   }

   target = malloc(MAX_SAMPLES * sizeof(double*));
   for(ctr = 0; ctr < (MAX_SAMPLES); ctr++)
   {
      target[ctr] = malloc(OUTPUT_NEURONS * sizeof(double));
   }

   actual = malloc(OUTPUT_NEURONS * sizeof(double));  //allocate space for the actual outputs
   hidden = malloc(HIDDEN_NEURONS * sizeof(double));  //allocate space for the hidden node values
   errh = malloc(HIDDEN_NEURONS * sizeof(double));    //hidden node error values
   erro = malloc(OUTPUT_NEURONS * sizeof(double));    //output node error values


   //
   // Grab a section of the amplitude data, AMPLSECTIONLENGTH number of samples
   // Get the frequency data from the full-length samples
   // Append the frequency data to the section of amplitude data
   //

   grabAllWavSections(trainingAudioSamples, sectionAmplTrainingAudio, trainingHeaders, TRAININGSAMPLES, AMPLSECTIONLENGTH);
   grabAllWavSections(testAudioSamples, sectionAmplTestAudio, testHeaders, TESTSAMPLES, AMPLSECTIONLENGTH);
   getFreqData(trainingAudioSamples,freqMagnTrainingAudio,trainingHeaders, TRAININGSAMPLES);
   getFreqData(testAudioSamples,freqMagnTestAudio,testHeaders, TESTSAMPLES);
   joinAmplFreqData(freqAmplTrainingAudio, freqMagnTrainingAudio, sectionAmplTrainingAudio, TRAININGSAMPLES);
   joinAmplFreqData(freqAmplTestAudio, freqMagnTestAudio, sectionAmplTestAudio, TESTSAMPLES);



   for(longCyclesCtr = 0; longCyclesCtr < 1; longCyclesCtr++)
   {

      FILE *out = fopen("results.txt", "a");
   

      fprintf(out,"\n\nStarting longCycle #%d\n", longCyclesCtr);


      averageAccuracy = 0; 
      for(cycleCtr = 0; cycleCtr < TOTALCYCLES; cycleCtr++)
      {


	 /* Seed the random number generator */
	 printf("Assigning Random Weights\n");
	 srand(time(NULL));
	 assignRandomWeights();

	 /*Assign the target 'answers' to for the training set */
	 printf("Training Targets\n");
	 assignTargets(TRAININGSAMPLES);

	 //
	 /* Train the network */
	 //
	 sample = 0;
	 errCtr = 0;
	 iterations = 0;
	 errorBoundary = 0;
	 totalIterations = ITERATIONS;
	 sum = 0;
	 
	 while(1)
	 {

	    feedForward(sample, freqAmplTrainingAudio);

	    err = 0.0;

	    for(i=0; i<OUTPUT_NEURONS; i++)
	    {
	       err += sqr( (target[sample][i] - actual[i]) );
	    }

	    err = 0.5 * err;
	    //fprintf(out, "%g\n", err);
	    
	    if((iterations % 10000) == 0)
	    {
	       //printf("mse = %g, iterations = %d, T[%d]: %g\n", err, iterations,sample, target[sample][0]);
	    }
	    else if(iterations < 10)
	    {
	       printf("mse = %g\n", err);
	    }
	    else if( (iterations > (totalIterations /2)) && (iterations < (totalIterations/2 + 10)))
	    {
	       printf("mse = %g\n", err);
	    }
	    else if (iterations > (totalIterations - 10))
	    {
	       printf("mse = %g\n", err);
	    }
	    

	    if( iterations >= (totalIterations - 1000) )
	    {
	       errCtr++;
	       errorBoundary = errorBoundary + err;
	    }

	    backPropagate(sample, freqAmplTrainingAudio);

	    sample++;
	    if(sample == TRAININGSAMPLES)
	    {
	       sample = 0;
	    }

	    iterations++;
	    
	    
	    if(iterations > ITERATIONS)
	    {
	       break;
	    }
	    
	    /*
	    if(iterations > 100000)
	    {
	       break;
	    }
	    else if (iterations >= totalIterations)
	    {
	       if( (errorBoundary / errCtr) > 0.004)
	       {
		  totalIterations = totalIterations + 10000;
		  errorBoundary = 0;
		  errCtr = 0;
	       }
	       else
	       {
		  break;
	       }
	    }
	   */
	 }

	 printf("End of Training\n");

	 //
	 /* Test the network */
	 //

	 assignTargets(TESTSAMPLES);
	 
	 printf("\n\t\t\t\t\t\tInstantiation #%d results\n\n", cycleCtr);
	 for(sample = 0; sample < TESTSAMPLES; sample++)
	 {


	    feedForward(sample, freqAmplTestAudio);

	    //Record the actual output values for each clip on each cycle
	    resultsIndex = action(actual);
	    for(outCtr = 0; outCtr < OUTPUT_NEURONS; outCtr++)
	    {
	       if(outCtr == resultsIndex)
	       {
		  cycleActualOutputs[cycleCtr][sample][outCtr] = 1;
	       }
	       else
	       { 
		  cycleActualOutputs[cycleCtr][sample][outCtr] = 0;
	       }
	    }


	    if(action(actual) != action(target[sample]))
	    {
	       //printf("Sample #: %d, Actual[0] = %g, Actual[1] = %g, Target[0] = %g, Target[1] = %g, A: %s T: %s\n",sample,
		//     actual[0], actual[1], target[sample][0], target[sample][1],strings[action(actual)], strings[action(target[sample])]);
	       
	    }
	    else
	    {
	       //printf("Sample #: %d, Actual[0] = %g, Actual[1] = %g, Target[0] = %g, Target[1] = %g, A: %s T: %s\n",sample,
		//     actual[0], actual[1], target[sample][0], target[sample][1],strings[action(actual)], strings[action(target[sample])]);
	       sum++;
	    }

	    printf("Network Answer:  %s,     Target Answer:  %s\n", strings[action(actual)], strings[action(target[sample])]);
	 }

	 printf("\nNetwork is %g%% correct on the %d instantiation\n", ( (float)sum / 
		  (float)TESTSAMPLES) * 100.0, cycleCtr);

	 fprintf(out, "Network is %g%% correct on the %d Cycle\n", ( (float)sum / 
		  (float)TESTSAMPLES) * 100.0, cycleCtr);

	 averageAccuracy = averageAccuracy + (( (float)sum / (float)TESTSAMPLES) * 100.0);

	 for(ctr = 0; ctr < OUTPUT_NEURONS; ctr++)
	 {
	    actual[ctr] = 0;
	 }

	 printf("Finished Instantiation #%d\n\n\n", cycleCtr);

      } //end of cyclCtr for loop


      averageCycleOutputs(cycleActualOutputs, cycleAverageActualOutputs);
      calculateAndPrintAverageResults(out, cycleAverageActualOutputs, target);

      averageAccuracy = averageAccuracy / TOTALCYCLES;

      //fprintf(out, "\n\n The total accuracy across all instantiations = %g%% \n\n\n\n", averageAccuracy);
      //printf("\n\n The total accuracy across all the instantiations = %g%% \n", averageAccuracy);
      fclose(out);

   }


   //
   //Free memory
   //
   printf("Freeing Memory\n");
   for(ctr = 0; ctr < (TRAININGSAMPLES); ctr++)
   {
      free(trainingAudioSamples[ctr]);
      free(squashedTrainingAudio[ctr]);
      free(freqRawTrainingAudio[ctr]);
      free(sectionAmplTrainingAudio[ctr]);
      free(freqMagnTrainingAudio[ctr]);
      free(freqAmplTrainingAudio[ctr]);
   }

   for(ctr=0;ctr<TESTSAMPLES; ctr++)
   {
      free(testAudioSamples[ctr]);
      free(squashedTestAudio[ctr]);
      free(freqRawTestAudio[ctr]);
      free(sectionAmplTestAudio[ctr]);
      free(freqMagnTestAudio[ctr]);
      free(freqAmplTestAudio[ctr]);
   } 
   
   free(trainingAudioSamples);
   free(squashedTrainingAudio);
   free(freqRawTrainingAudio);
   free(sectionAmplTrainingAudio);
   free(trainingHeaders);
   free(freqMagnTrainingAudio);
   free(freqAmplTrainingAudio);
   free(testAudioSamples);
   free(squashedTestAudio);
   free(freqRawTestAudio);
   free(sectionAmplTestAudio);
   free(testHeaders);
   free(freqMagnTestAudio);
   free(freqAmplTestAudio);

   for(ctr = 0; ctr < (INPUT_NEURONS+1); ctr++)
   {
      free(wih[ctr]);
   }
   for(ctr=0; ctr < (HIDDEN_NEURONS+1); ctr++)
   {
      free(who[ctr]);
   }
   for(ctr=0; ctr < MAX_SAMPLES; ctr++)
   {
      free(target[ctr]);
   }

   free(wih);
   free(who);
   free(target);
   free(actual);
   free(hidden);
   free(errh);
   free(erro);
   
   return 0;
}



