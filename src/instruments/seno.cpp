
#include <iostream>
#include <math.h>
#include "seno.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

seno::seno(const std::string &param) 
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
  

  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
   // cout <<tbl[i] << "\n";
  }
}

void seno::command(long cmd, long note, long vel) {
  
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	  A = vel / 127.;
   
    float F0 = 440.00 * pow(2, (note - 69.00)/12.00) / SamplingRate;
    step = tbl.size() * F0;
  }
  
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  //Finaliza el sonido
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}

const vector<float> & seno::synthesize() {
  
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
    cout << x[i] << "\n"; 
    index += 1;
    if (new_index == tbl.size()) index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
