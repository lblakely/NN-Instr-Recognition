

#define TRAININGSAMPLES 286
#define TESTSAMPLES 20
#define SQUASHFACTOR 1
#define AMPLSECTIONLENGTH 400 //number of samples in the short section pulled out of the wav files for amplitude purposes
#define FREQSECTIONLENGTH 128  //number of samples in the short section for frequency purposes

#define INPUT_NEURONS 464
#define HIDDEN_NEURONS 115 

#define OUTPUT_NEURONS 2

#define MAX_SAMPLES 291
#define ITERATIONS 50000
#define TOTALCYCLES 8
#define LEARN_RATE 0.2  //Rho

#define NYQUIST 4000   //My clips are sampled at 8000 samples/sec and the NYQUIST number is half of that 
#define BINNUMBER ( FREQSECTIONLENGTH / 2)
#define BINWIDTH ( NYQUIST / BINNUMBER)   //~15hz for a section length of 512
#define NUMFREQWINDOWS 30
			
/*

Nyquist Frequency = sampling frequency / 2   i.e 8000/2 = 4000   -> This is the maximum frequency captured at this sampling frequency
Bin Width = nyquist frequency / fft size     i.e 4000/1024 = 4hz -> each element in the resulting array is 4hz wide, from 0 - 4khz
fft size = power of 2

*/




			//  Squash Factor 		 Input: 		Hidden:
			//   
			//    	1	 		 16,000/16384		10,600
			// 	2			 8,000/8192		5,200
			//	4			 4,000/4096		2,600
			//	5			 3,200			2,000
			//      8                        2000/2048		1,300
			//	10			 1600			1,000
		        //      20	                  800
