/********************************************************/
/*							*/
/* nn.h  - Neural Networks Project header file		*/
/* 							*/
/* Logan Blakely					*/
/* November 2016					*/
/*							*/
/********************************************************/

//Original Code taken from Jones Ch5 on Neural Networks

#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

/* Input to Hidden Weights (with Biases) */
//double wih[INPUT_NEURONS + 1][HIDDEN_NEURONS];
double **wih;

/* Hidden to Output Weights (with Biases) */
//double who[HIDDEN_NEURONS+1][OUTPUT_NEURONS];
double **who;


/*Activations */
//double inputs[INPUT_NEURONS];
//double hidden[HIDDEN_NEURONS];
//double target[OUTPUT_NEURONS];
//double actual[OUTPUT_NEURONS];


//double *inputs;
double *hidden;
double **target;
double *actual;




/* Unit Errors */
double *erro;
//double errh[HIDDEN_NEURONS];
double *errh;

typedef struct {
   int16_t audioSamples[INPUT_NEURONS]; 
   double out[OUTPUT_NEURONS];

} ELEMENT;



/*******************************************************/
//
//Macros

#define RAND_WEIGHT ( ((float)rand() / (float)RAND_MAX) - 0.5 )
#define getSRand()  ((float)rand() / (float)RAND_MAX)
#define getRand(x) (int)((x) * getSRand())
#define sqr(x) ((x) * (x))


/*******************************************************/
//
//Function Declarations


void assignRandomWeights(void);
double sigmoid(double);
double sigmoidDerivative(double);
void feedForward(int, double **);
void backPropagate(int, double **);
int action (double*);
void assignTargets(int);
int averageCycleOutputs(double [TOTALCYCLES][TESTSAMPLES][OUTPUT_NEURONS], double [TESTSAMPLES][OUTPUT_NEURONS]);
int calculateAndPrintAverageResults(FILE *, double [TESTSAMPLES][OUTPUT_NEURONS], double **);















