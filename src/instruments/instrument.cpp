#include <iostream>

#include "instrument_dumb.h"
#include "seno.h"
#include "seno_vibrato.h"

/*
  For each new instrument:
  - Add the header in this file
  - Add the call to the constructor in get_instrument() (also in this file)
  - Add the source file to src/meson.build
*/

using namespace std;

namespace upc {
  Instrument * get_instrument(const string &name,
			      const string &parameters) {
    Instrument * pInst = 0;
    //    cout << name << ": " << parameters << endl;
    if (name == "InstrumentDumb") {
      pInst = (Instrument *) new InstrumentDumb(parameters);
    }
    if (name == "seno") {
      pInst = (Instrument *) new seno(parameters);
    }
     if (name == "seno_vibrato") {
       pInst = (Instrument *) new seno_vibrato(parameters);
    }
    return pInst;
  }
}