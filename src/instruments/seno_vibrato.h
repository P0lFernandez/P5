#ifndef SENO_VIBRATO
#define SENO_VIBRATO

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class seno_vibrato: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
	  float A, N1, N2, I;
    float phase;
    float step, alpha, phase_m;
    std::vector<float> tbl;
  public:
    seno_vibrato(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif