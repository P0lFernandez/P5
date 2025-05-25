#include <iostream>
#include <math.h>
#include "seno_vibrato.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

seno_vibrato::seno_vibrato(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */

  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value

  if (!kv.to_float("N1",N1))
    N1 = 4; //default value

  if (!kv.to_float("N2",N2))
    N2 = 1; //default value

  if (!kv.to_float("I",I))
    I = 0.000328; //default value
  
  
  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}

void seno_vibrato::command(long cmd, long note, long vel) {
  
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	  A = vel / 127.;

    float F0 = 440.00 * pow(2, (note - 69.00)/12.00) / SamplingRate;
    float Fc = N1 * F0;
    float Fm = N2 * F0;

  
    step = tbl.size() * Fm;
    phase_m = 0;
    alpha = 2 * M_PI * Fc;
  }
  
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
 
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}

const vector<float> & seno_vibrato::synthesize() {
  
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  
  else if (not bActive)
    return x;
  
  for (unsigned int i=0; i<x.size(); ++i) {
    
    float new_index = round(index * step);
    x[i] = A * (tbl[new_index]);
    index += (1 + I * sin(phase_m)/step);
    phase_m += alpha;
    if (new_index == tbl.size()) index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}