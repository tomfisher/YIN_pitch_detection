#include "autocorrelation.h"

#define TRESHOLD 0.1
#define SILENCE 10000000

float pitch_detect(int * frame, int length, int samplerate){

    int W = length;
    int i,t,j;


    /** Step 0: Detect silence **/
    int silence = 0;
    for(i = 0; i < length; i++){
        if(abs(frame[i]) >= SILENCE){
            silence = 1;
        }
    }
    if(silence == 0){
        return 0;
    }

    /** Step 1: Autocorrelate (Not needed due to difference function) **/

    // long * acf = (long *) malloc(length*sizeof(long));
    // memset(acf, 0, length*sizeof(long));
    //
    // for(t = 0; t < length; t++){
    //     for(j = 1; j < W-t; j++){
    //         acf[t] += (long)frame[j]*frame[j+t];
    //         // printf("%ld\n", (long)frame[j]*frame[j+t]);
    //     }
    // }
    //
    // int maxima = W-1;
    // //Find largest Local Maxima
    // for(i = 1; i < W-1; i++){
    //     if(acf[i] >= fmax(acf[i-1], acf[i+1]) && acf[i] > acf[maxima]){
    //         maxima = i;
    //     }
    // }


    /** Step 2: Generate Difference Function **/
    float * difference = (float *) malloc(length*sizeof(float));
    memset(difference, 0, length*sizeof(float));

    for(t = 0; t<length; t++){
        for(j = 1; j < W+1; j++){
            difference[t] += pow(((float)frame[j]-frame[j+t]),2);
        }
    }


    /** Step 3: Cumulative Normalized Difference Mean **/
    float * cmndifference = (float *) malloc(length*sizeof(float));
    memset(cmndifference, 0, length*sizeof(float));

    for(t = 0; t<length; t++){
        if(t == 0){
            cmndifference[t] = 1;
        }
        else{
            float denominator = 0;
            for(j = 1; j < t+1; j++){
                denominator += difference[j];
            }
            denominator /= t;
            if(denominator != 0){
                cmndifference[t] = difference[t]/denominator;
            }
            else{
                cmndifference[t] = 1;
            }

        }
    }


    /** Step 4: Aboslute Threshold **/
    int minima = W;
    //Find smallest min below threshold
    for(i = 1; i < W-1; i++){
        if(cmndifference[i] <= fmin(cmndifference[i-1], cmndifference[i+1]) && cmndifference[i] <= TRESHOLD){
            minima = i;
            break;
        }
    }


    /** Free memory from buffers **/
    // free(acf);
    free(difference);
    free(cmndifference);


    //If minima is still W, no suitable minima was found. Return 0.
    if(minima == W){
        return 0;
    }


    /** Return frequency **/
    return (float)samplerate/minima;
}
