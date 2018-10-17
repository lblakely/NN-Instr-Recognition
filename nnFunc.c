/********************************************************/
/*							*/
/* nnFunc.c - support functions for Neural Network 	*/
/*							*/
/* Logan Blakely					*/
/* November 2016					*/
/*							*/
/*							*/
/********************************************************/

//Original Code taken from Jones Ch5 on Neural Networks, and then 
//  modified for my purposes
//
#include <stdio.h>
#include "nn.h"
#include <math.h>

#include "constants.h"
extern char trainingFilenames[][100];
extern char testFilenames[][100];
//
//
//Support Functions for Backpropagation
//
//
void assignRandomWeights(void)
{
   int hid;
   int inp;
   int out;

   for(inp=0;inp<INPUT_NEURONS+1;inp++)
   {
      /*
      if((inp % 1000) == 0)
      {
	 printf("inp = %d\n", inp);
      }
      */
      for(hid=0;hid<HIDDEN_NEURONS; hid++)
      {
	 wih[inp][hid] = RAND_WEIGHT;

      }
   }

   for(hid=0; hid<HIDDEN_NEURONS+1; hid++)
   {
      for(out=0;out<OUTPUT_NEURONS; out++)
      {
	 who[hid][out] = RAND_WEIGHT;
      }
   }


   printf("end of assignRandmWeights\n");
} //end of assignRandomWeights


double sigmoid(double val)
{
   return (1.0 / (1.0 + exp(-val)));
}//end of sigmoid

double sigmoidDerivative(double val)
{
   return (val * (1.0 - val));
}//end of sigmoidDerivative


/*************************************************************/
//
//Feed-forward Algorithm
//
void feedForward(int sample, double **inputs )
{
   int inp;
   int hid;
   int out;
   double sum;

   /*Calculate input to hidden layer */
   for(hid=0;hid<HIDDEN_NEURONS;hid++)
   {
      sum = 0.0;


      for(inp=0;inp<INPUT_NEURONS; inp++)
      {
	 sum += inputs[sample][inp] * wih[inp][hid];  //This is not efficient memory access, think about switching it
      }

      /*Add in Bias */
      sum += wih[INPUT_NEURONS][hid];

      hidden[hid] = sigmoid(sum);
   }

   /*Calculat the hidden to output layer */
   for(out=0;out<OUTPUT_NEURONS;out++)
   {
      sum = 0.0;
      for(hid = 0; hid<HIDDEN_NEURONS; hid++)
      {
	 sum += hidden[hid] * who[hid][out];
      }

      /*Add in bias */

      sum += who[HIDDEN_NEURONS][out];

      actual[out] = sigmoid(sum);
   }

}//end of feedForward


void backPropagate(int sample, double **inputs)
{
   int inp;
   int hid;
   int out;

   /*Calculate the output layer error (step 3 for Output cell) */

   for(out=0; out<OUTPUT_NEURONS; out++)
   {
      erro[out] = (target[sample][out] - actual[out]) * sigmoidDerivative(actual[out]);
   }

   /*Calculate the hidden layer error (step 3 for hidden cell) */
   for(hid=0;hid<HIDDEN_NEURONS;hid++)
   {
      errh[hid] = 0.0;
      for(out=0;out<OUTPUT_NEURONS; out++)
      {
	 errh[hid] += erro[out] * who[hid][out];
      }

      errh[hid] *= sigmoidDerivative( hidden[hid]);

   }

   /*Update the wieghts for the output layer (step 4) */

//   printf("weights for the output layer\n");
   for(out=0;out<OUTPUT_NEURONS; out++)
   {
  //    printf("who[%d][%d] = %g,", 999, out, who[999][out]);
  //    printf("erro[0] = %g, erro[1] = %g ", erro[0], erro[1]);
      for(hid=0;hid<HIDDEN_NEURONS;hid++)
      {
	 who[hid][out] += (LEARN_RATE * erro[out] * hidden[hid]);
      }
  //    printf("updated:  %g\n", who[999][out]);
  //    printf("Bias = %g, updated ", who[1000][out] );
  //    printf("Learn_Rate * error[0] * hidden[0] =  %g \n", LEARN_RATE*erro[0]*hidden[0]);

      /* Update the Bias */
      who[HIDDEN_NEURONS][out] += (LEARN_RATE * erro[out]);

  //    printf("%g \n", who[1000][out]);
   }


   /*Update the weigths for the hidden layer (step 4) */
   for(hid=0; hid<HIDDEN_NEURONS; hid++)
   {
      for(inp = 0; inp<INPUT_NEURONS; inp++)
      {
	 wih[inp][hid] += (LEARN_RATE * errh[hid] * inputs[sample][inp]);
      }

      /*Update the Bias */
      wih[INPUT_NEURONS][hid] += (LEARN_RATE * errh[hid]);

   }

} //end of backPropagate


/*********************************************************/
//Winner-take-all Determiner Function


int action (double *vector)
{
   int index;
   int sel;
   double max;

   sel = 0;
   max = vector[sel];

   
   for(index = 1; index < OUTPUT_NEURONS; index++) 
   {
      if(vector[index] > max)
      {
	 max = vector[index];
	 sel = index;
      }
   }




   return (sel);
}

/****************************************************************/
/*								*/
/* assignTargets						*/
/*								*/
/* Takes the number of clips at a parameter			*/
/*								*/
/* Function searches each filename to determine if it is a 	*/
/*   violin or trumpet clip and assigns the target values 	*/
/*   appropriately						*/
/*								*/
/****************************************************************/

void assignTargets(int sampleNumber)
{
   int ctr = 0;
   int letterCtr;
   char currentLtr;


   if(sampleNumber == TRAININGSAMPLES)
   {
      
      printf("assigning Training Targets\n");
      while(ctr < TRAININGSAMPLES)
      {
	 letterCtr = 0;
	 currentLtr = trainingFilenames[ctr][letterCtr];
	 while( (currentLtr != '.') &&(letterCtr < 100))
	 {
//	    printf("currentLtr = %c \n", currentLtr);
	    if(currentLtr == 'V')
	    {
	       target[ctr][0] = 1;
	       target[ctr][1] = 0;
	       currentLtr = '.';
	    }
	    else if(currentLtr == 'T')
	    {
	       target[ctr][0] = 0;
	       target[ctr][1] = 1;
	       currentLtr = '.';
	    }
	    else
	    {
	       letterCtr++;
	       currentLtr = trainingFilenames[ctr][letterCtr];
	    }

         }

//	 printf("target[%d][0] = %g, target[%d] = %g \n", ctr, target[ctr][0], ctr, target[ctr][1]);
	 ctr++;
      }

   }
   else
   {
      ctr = 0;
 
      printf("assigning Test Targets\n");
      while(ctr < TESTSAMPLES)
      {

	 letterCtr = 0;
	 currentLtr = testFilenames[ctr][letterCtr];
	 while(currentLtr != '.')
	 {
	    if(currentLtr == 'V')
	    {
	       target[ctr][0] = 1;
	       target[ctr][1] = 0;
	       currentLtr = '.';
	    }
	    else if(currentLtr == 'T')
	    {
	       target[ctr][0] = 0;
	       target[ctr][1] = 1;
	       currentLtr = '.';
	    }
	    else
	    {
	       letterCtr++;
	       currentLtr = testFilenames[ctr][letterCtr];
	    }

        }


	// printf("target[%d][0] = %g, target[%d][1] = %g \n", ctr, target[ctr][0], ctr, target[ctr][1]);

	 ctr++;
      }
   }
}//end assignTargets



int averageCycleOutputs(double allOutputs[TOTALCYCLES][TESTSAMPLES][OUTPUT_NEURONS], double averageOutputs[TESTSAMPLES][OUTPUT_NEURONS])
{

   int clipCtr;
   int neuronCtr;
   int cycleCtr;
   int sum[OUTPUT_NEURONS];
   double tempAvg[OUTPUT_NEURONS];
   int resultsIndex;

   for(clipCtr = 0; clipCtr < TESTSAMPLES; clipCtr++)
   {
      for(neuronCtr = 0; neuronCtr < OUTPUT_NEURONS; neuronCtr++)
      {
	 sum[neuronCtr] = 0;
      }

      //printf("Clip #%d\n", clipCtr);
      for(cycleCtr = 0; cycleCtr < TOTALCYCLES; cycleCtr++)
      {
	 for(neuronCtr = 0; neuronCtr < OUTPUT_NEURONS; neuronCtr++)
	 {
	    sum[neuronCtr] = sum[neuronCtr] + allOutputs[cycleCtr][clipCtr][neuronCtr];
	    //printf("%g  ", allOutputs[cycleCtr][clipCtr][neuronCtr]);
	 }
	 //printf("\n");
      }

      for(neuronCtr = 0; neuronCtr < OUTPUT_NEURONS; neuronCtr++)
      {
	 tempAvg[neuronCtr] = (float)sum[neuronCtr] / TOTALCYCLES;
	 tempAvg[neuronCtr] = (int)(tempAvg[neuronCtr] + 0.5);
	 //printf("sum[%d] = %d \n", neuronCtr, sum[neuronCtr]);
	 //printf("tempAvg[%d] = %g \n", neuronCtr, tempAvg[neuronCtr]);
      }

      resultsIndex = action(tempAvg);
      //printf("resultsIndex = %d\n", resultsIndex);
      for(neuronCtr = 0; neuronCtr < OUTPUT_NEURONS; neuronCtr++)
      {
	 if(resultsIndex == neuronCtr)
	 {
	    averageOutputs[clipCtr][neuronCtr] = 1;
	 }
	 else
	 { 
	    averageOutputs[clipCtr][neuronCtr] = 0;
	 }
      }

      sum[neuronCtr] = 0;
   }

   /*
   for(clipCtr =0; clipCtr < TESTSAMPLES; clipCtr++)
   {
      printf("Clip #%d\n", clipCtr);
      for(neuronCtr=0; neuronCtr < OUTPUT_NEURONS; neuronCtr++)
      {
	 printf("%g ", averageOutputs[clipCtr][neuronCtr]);
      }
      printf("\n\n");
   }
   */

   return (0);
} //end of averageCycleOutputs



int calculateAndPrintAverageResults(FILE *fp, double averageOutputs[TESTSAMPLES][OUTPUT_NEURONS], double **targets)
{
   int clipCtr;
   int sum = 0;

   char *strings[2] = {"Violin", "Trumpet"};

   printf("\n\n\t\t\t\t\t\tResults across all instantiations\n");   
   fprintf(fp,"\n\nResults across all instantiations\n");
   for(clipCtr = 0; clipCtr < TESTSAMPLES; clipCtr++)
   {
      

      if(action(averageOutputs[clipCtr]) != action(targets[clipCtr]))
      {
	 //printf("Sample #: %d, Actual[0] = %g, Actual[1] = %g, Target[0] = %g, Target[1] = %g, A: %s T: %s\n",clipCtr,
	 //      averageOutputs[clipCtr][0], averageOutputs[clipCtr][1], targets[clipCtr][0], targets[clipCtr][1],
	 //      strings[action(averageOutputs[clipCtr])], strings[action(targets[clipCtr])]);

	 fprintf(fp, "Sample #: %d, Actual[0] = %g, Actual[1] = %g, Target[0] = %g, Target[1] = %g, A: %s T: %s\n",clipCtr,
	       averageOutputs[clipCtr][0], averageOutputs[clipCtr][1], targets[clipCtr][0], targets[clipCtr][1],
	       strings[action(averageOutputs[clipCtr])], strings[action(targets[clipCtr])]);
      }
      else
      {
	 //printf("Sample #: %d, Actual[0] = %g, Actual[1] = %g, Target[0] = %g, Target[1] = %g, A: %s T: %s\n",clipCtr,
	 //      averageOutputs[clipCtr][0], averageOutputs[clipCtr][1], targets[clipCtr][0], targets[clipCtr][1],
	 //      strings[action(averageOutputs[clipCtr])], strings[action(targets[clipCtr])]);

	 fprintf(fp, "Sample #: %d, Actual[0] = %g, Actual[1] = %g, Target[0] = %g, Target[1] = %g, A: %s T: %s\n",clipCtr,
	       averageOutputs[clipCtr][0], averageOutputs[clipCtr][1], targets[clipCtr][0], targets[clipCtr][1],
	       strings[action(averageOutputs[clipCtr])], strings[action(targets[clipCtr])]);
	 sum++;
      }

      printf("Test Sample #%d - Network Answer = %s,  Target Answer = %s\n", clipCtr, strings[action(averageOutputs[clipCtr])], 
	    strings[action(targets[clipCtr])]);
   }


   printf("\nThe total accuracy across all instantiations is  %g%% \n\n", ( (float)sum / 
	    (float)TESTSAMPLES) * 100.0);

   fprintf(fp, "The total accuracy across all instantiations is %g%% \n", ( (float)sum / 
	    (float)TESTSAMPLES) * 100.0);


   return (0);
}//end of calculateAndPrintAVerageResults



