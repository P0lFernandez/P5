#include <iostream>
#include <math.h>
#include "instrument_dumb.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;


//El modo sencillo de generar un sonido periodico de frecuencia
//variable es almacenar un periodo del mismo en una tabala y 
//recorrer esta a una velocidad adecuada para conseguir
//el pitch deseado

//Constructor
InstrumentDumb::InstrumentDumb(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */

 //Usando funciones de la libreria keyvalue.h analizamos
 //la cadena de parámetros de interes para el instrumento, en este caso
 //solamente la longitud de la tabla N, ya que los parametros ADSR se procesan 
 //en otra parte
  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value
  
//Se crea la tabla con un periodo de sinusoide
  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}

//Cada vez que el programa encuentra un comando MIDI en el fichero score, invoca al
//método command(comando, la nota, la velocidad)
//si el comando es 9 (NoteOn) se declara activo el instrumento
//inicializa curva ADSR y un contador index que recorre la tabla,
//de amplitud A. Si cmd = 8 o 0 (NoteOff o EndNote) el metoo inicia la fase release de la curva
//o fiinaliza el sonido, respectivament
void InstrumentDumb::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	A = vel / 127.;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}

//La sintesis se realiza aquí
//La nota puede encontrarse en 3 situaciones Finalizada, Inactiva, Activa:
const vector<float> & InstrumentDumb::synthesize() {
  //Finalizada: si la curva ha llegado a su nivel final
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  //Inactiva si ya ha sido marcada como inactiva
  else if (not bActive)
    return x;
  //Activa, se aplica la envolvente ADSR, aquí es donde se recorre la tabla
  for (unsigned int i=0; i<x.size(); ++i) {
    x[i] = A * tbl[index++];
    if (index == tbl.size())
      index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}