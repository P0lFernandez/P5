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

  if (!kv.to_float("I",I)) I = 1;
  if(!kv.to_float("N1",N1)) N1=1;
  if(!kv.to_float("N2",N2)) N2=1;
  
}


void seno_vibrato::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
    f0=(pow(2,(note-69.0)/12.0))*440;
    pass = (tbl.size()/(double) SamplingRate)*f0 ; 
    nota = f0/SamplingRate;
    step1 = 2*M_PI*nota;
    step2 = 2*M_PI*(N2/N1)*nota;

  	A = vel / 127.0;

    phase1 = 0; 
    phase2 = 0;

    cout << vel << endl;

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
    x[i] = A *sin(phase1 + I*sin(phase2));
    phase1 += step1;
    phase2 += step2;
    
    while(phase1>2*M_PI) phase1 -= 2*M_PI;
    while(phase2>2*M_PI) phase2 -= 2*M_PI;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR
  
  return x;
}