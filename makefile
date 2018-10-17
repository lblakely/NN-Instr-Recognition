CC = gcc

OBJS = nnDriver.o nnFunc.o wavFunc.o 

LIBPATHS = -L/u/lblakely/nnProject/DFTAlgo/UFFT

LDLIBS = -llibfft-dit.a -llibift-dit.a
 

LDFLAGS = -g -Wall -lm


all: nnDriver

nnDriver:  nnFunc.o nnDriver.o wavFunc.o 
	$(CC) -o nnDriver $(OBJS) $(LDFLAGS) /u/lblakely/nnProject/DFTAlgo/UFFT/libfft-dit.a /u/lblakely/nnProject/DFTAlgo/UFFT/libift-dit.a  
	
nnDriver.o: nn.h trainingSet.h nnDriver.c wav.h constants.h filenames.h
	$(CC) -c nnDriver.c  nn.h trainingSet.h wav.h constants.h filenames.h $(LDFLAGS)

nnFunc.o:  nnFunc.c nn.h trainingSet.h constants.h filenames.h
	$(CC) -c nnFunc.c nn.h trainingSet.h constants.h filenames.h $(LDFLAGS)

wavFunc.o: wav.h filenames.h wavFunc.c constants.h fft.h
	$(CC) $(LIBPATHS) $(LDLIBS) -c wavFunc.c wav.h filenames.h constants.h fft.h  $(LDFLAGS) 
		
clean:
	rm -f nnDriver *.o core
