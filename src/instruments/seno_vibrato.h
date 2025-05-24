#ifndef SINTESIS_FM
#define SINTESIS_FM

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class seno_vibrato: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
	float A,f0,pass,nota,velc,phase1,phase2,N2,N1,step1,step2,I;
    std::vector<float> tbl;
  public:
    seno_vibrato(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif