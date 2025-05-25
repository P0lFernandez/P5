PAV - P5: síntesis musical polifónica
=====================================

Obtenga su copia del repositorio de la práctica accediendo a [Práctica 5](https://github.com/albino-pav/P5) 
y pulsando sobre el botón `Fork` situado en la esquina superior derecha. A continuación, siga las
instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para crear una rama con el apellido de
los integrantes del grupo de prácticas, dar de alta al resto de integrantes como colaboradores del proyecto
y crear la copias locales del repositorio.

Como entrega deberá realizar un *pull request* con el contenido de su copia del repositorio. Recuerde que
los ficheros entregados deberán estar en condiciones de ser ejecutados con sólo ejecutar:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.sh
  make release
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A modo de memoria de la práctica, complete, en este mismo documento y usando el formato *markdown*, los
ejercicios indicados.

Ejercicios.
-----------

### Envolvente ADSR.

Tomando como modelo un instrumento sencillo (puede usar el InstrumentDumb), genere cuatro instrumentos que
permitan visualizar el funcionamiento de la curva ADSR.

* Un instrumento con una envolvente ADSR genérica, para el que se aprecie con claridad cada uno de sus
  parámetros: ataque (A), caída (D), mantenimiento (S) y liberación (R).

Usamos el instrumento Dumb con los siguientes parametros:

1   InstrumentDumb	ADSR_A=0.3; ADSR_D=0.3; ADSR_S=0.2; ADSR_R=0.2; N=40;

  ![alt text](image.png)

* Un instrumento *percusivo*, como una guitarra o un piano, en el que el sonido tenga un ataque rápido, no
  haya mantenimiemto y el sonido se apague lentamente.
  - Para un instrumento de este tipo, tenemos dos situaciones posibles:
    * El intérprete mantiene la nota *pulsada* hasta su completa extinción.

Vemos como la nota se va apagando lentamente para simular que dejamos la nota pulsada hasta su extinción.

1   InstrumentDumb	ADSR_A=0.2; ADSR_D=0.2; ADSR_S=0.009; ADSR_R=0.2;

  ![alt text](image-2.png)

    * El intérprete da por finalizada la nota antes de su completa extinción, iniciándose una disminución
	  abrupta del sonido hasta su finalización.

En esta imagen vemos como el ataque es rápido, como la caída es lenta y la nota se acaba de golpe sin tiempo de liberación.

1   InstrumentDumb	ADSR_A=0.2; ADSR_D=0.2; ADSR_S=0; ADSR_R=0; N=40;

    ![alt text](image-1.png)
    
  - Debera representar en esta memoria **ambos** posibles finales de la nota.
* Un instrumento *plano*, como los de cuerdas frotadas (violines y semejantes) o algunos de viento. En
  ellos, el ataque es relativamente rápido hasta alcanzar el nivel de mantenimiento (sin sobrecarga), y la
  liberación también es bastante rápida.

  Ponemos un ataque y una caída rapidas, el mantenimiento bastante largo y la liberación algo más lenta que la caída.

1   InstrumentDumb	ADSR_A=0.1; ADSR_D=0.1; ADSR_S=0.7; ADSR_R=0.2; N=40;

  ![alt text](image-3.png)

Para los cuatro casos, deberá incluir una gráfica en la que se visualice claramente la curva ADSR. Deberá
añadir la información necesaria para su correcta interpretación, aunque esa información puede reducirse a
colocar etiquetas y títulos adecuados en la propia gráfica (se valorará positivamente esta alternativa).

### Instrumentos Dumb y Seno.

Implemente el instrumento `Seno` tomando como modelo el `InstrumentDumb`. La señal **deberá** formarse
mediante búsqueda de los valores en una tabla.

- Incluya, a continuación, el código del fichero `seno.cpp` con los métodos de la clase Seno.

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
  }
}

void seno::command(long cmd, long note, long vel) {
  
  if (cmd == 9) {		
    bActive = true;
    adsr.start();
    index = 0;
	  A = vel / 127.;
    
    float F0 = 440.00 * pow(2, (note - 69.00)/12.00);
    step = tbl.size() * F0;
  }
  
  else if (cmd == 8) {	
    adsr.stop();
  }
 
  else if (cmd == 0) {	
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



- Explique qué método se ha seguido para asignar un valor a la señal a partir de los contenidos en la tabla,
  e incluya una gráfica en la que se vean claramente (use pelotitas en lugar de líneas) los valores de la
  tabla y los de la señal generada.

  ![alt text](image-4.png)

-Lo primero que hay que hacer es obtener la frecuencia fundamental y a partir del seno generamos la tabla que se recorre. Así podemos modificar la frecuencia del seno.

- Si ha implementado la síntesis por tabla almacenada en fichero externo, incluya a continuación el código
  del método `command()`.

### Efectos sonoros.

- Incluya dos gráficas en las que se vean, claramente, el efecto del trémolo y el vibrato sobre una señal
  sinusoidal. Deberá explicar detalladamente cómo se manifiestan los parámetros del efecto (frecuencia e
  índice de modulación) en la señal generada (se valorará que la explicación esté contenida en las propias
  gráficas, sin necesidad de *literatura*).

  DOREMI SIN EFFECTS

  ![alt text](image-5.png)

    Ponemos la imagen sin efectos para poder comparar con ambos efectos tanto visiblemente con auditivamente. Para cada efecto cambiamos algunos de los valores de la columna Effects del effects.orc


  DOREMI CON TREMOLO

  Creamos un .sco para crear la partitura del efecto tremolo.

  ![alt text](<Captura de pantalla 2025-05-24 180717.png>)


  Tremolo A=0.5; fm=10;

  DOREMI CON VIBRATO

  Creamos un .sco para crear la partitura del efecto vibrato.

![alt text](image-6.png)

    Vibrato I=0.5; fm=8;

- Si ha generado algún efecto por su cuenta, explique en qué consiste, cómo lo ha implementado y qué
  resultado ha producido. Incluya, en el directorio `work/ejemplos`, los ficheros necesarios para apreciar
  el efecto, e indique, a continuación, la orden necesaria para generar los ficheros de audio usando el
  programa `synth`.

### Síntesis FM.

Construya un instrumento de síntesis FM, según las explicaciones contenidas en el enunciado y el artículo
de [John M. Chowning](https://web.eecs.umich.edu/~fessler/course/100/misc/chowning-73-tso.pdf). El
instrumento usará como parámetros **básicos** los números `N1` y `N2`, y el índice de modulación `I`, que
deberá venir expresado en semitonos.

- Use el instrumento para generar un vibrato de *parámetros razonables* e incluya una gráfica en la que se
  vea, claramente, la correspondencia entre los valores `N1`, `N2` e `I` con la señal obtenida.

Con el código creado jugamos con los valores de N1, N2 e I y obtenemos la siguiente gráfica. Para definirlo usamos le siguiente .orc:
 1	seno_vibrato    ADSR_A=0.08; ADSR_D=0; ADSR_S=0.4; ADSR_R=0.02; N1=70; N2=10; I=10;

![alt text](image-8.png)


- Use el instrumento para generar un sonido tipo clarinete y otro tipo campana. Tome los parámetros del
  sonido (N1, N2 e I) y de la envolvente ADSR del citado artículo. Con estos sonidos, genere sendas escalas
  diatónicas (fichero `doremi.sco`) y ponga el resultado en los ficheros `work/doremi/clarinete.wav` y
  `work/doremi/campana.work`.

  CLARINETE

  ![alt text](image-9.png)

  Para recrear el sonido del clarinete la relacion de frecuencias N1 y N2 tiene que ser 1/3 y el indice de modulación tiene que ser aproximadamente 2. Por lo que hace a la envolvente ADSR el ataque tiene que ser rápido, la caida lenta y mantenimiento constante.

  CAMPANA

  ![alt text](image-10.png)

  En una campana la relación de frecuencias para que suene de una manera correcta debe ser la siguiente: 

  ![alt text](image-11.png)

  


  * También puede colgar en el directorio work/doremi otras escalas usando sonidos *interesantes*. Por
    ejemplo, violines, pianos, percusiones, espadas láser de la
	[Guerra de las Galaxias](https://www.starwars.com/), etc.

### Orquestación usando el programa synth.

Use el programa `synth` para generar canciones a partir de su partitura MIDI. Como mínimo, deberá incluir la
*orquestación* de la canción *You've got a friend in me* (fichero `ToyStory_A_Friend_in_me.sco`) del genial
[Randy Newman](https://open.spotify.com/artist/3HQyFCFFfJO3KKBlUfZsyW/about).

- En este triste arreglo, la pista 1 corresponde al instrumento solista (puede ser un piano, flautas,
  violines, etc.), y la 2 al bajo (bajo eléctrico, contrabajo, tuba, etc.).
- Coloque el resultado, junto con los ficheros necesarios para generarlo, en el directorio `work/music`.
- Indique, a continuación, la orden necesaria para generar la señal (suponiendo que todos los archivos
  necesarios están en directorio indicado).

  En la carpeta creada llamada music dentro de work estan los ficheros necesarios para producir los wavs, es decir los .sco, los .orc y por ultimo los .wav. 

  Tras varias pruebas nos quedamos con estas ultimas que mas se asemejan a las originales.

También puede orquestar otros temas más complejos, como la banda sonora de *Hawaii5-0* o el villacinco de
John Lennon *Happy Xmas (War Is Over)* (fichero `The_Christmas_Song_Lennon.sco`), o cualquier otra canción
de su agrado o composición. Se valorará la riqueza instrumental, su modelado y el resultado final.
- Coloque los ficheros generados, junto a sus ficheros `score`, `instruments` y `efffects`, en el directorio
  `work/music`.
- Indique, a continuación, la orden necesaria para generar cada una de las señales usando los distintos
  ficheros.

  De misma manera, hemos genereado un fichero para el audio de Hawaii.

> NOTA:
>
> No olvide escuchar el resultado generado y comprobar que no se producen ruidos extraños o distorsiones.
> Sobre todo, tenga en cuenta la salud auditiva de quien será encargado de corregir su trabajo.
