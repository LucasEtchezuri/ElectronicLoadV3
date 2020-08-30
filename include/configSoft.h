#define MA_MAX  10000.00   //  Aca va el valor de MA que representa una salida de 2000mv del INA. (En mi caos use un INA de 400mv por Ampere.  por lo tanto si la salida es 2000mv serian 5000ma)
#define MV_MAX  22530.00   //  Aca va el valor de MV que representa una salida de 2048mv en el divisor de voltage que entra al ADC
#define INA_MV_A  1980.00   //   mv x A  que entrega el INA.  (le aaregamos un decimal)

// COMPENSACION DAC
#define CORRECCION_MA_DAC   -70  // en ma  (x 10)
//#define CORRECCION_MA_DAC   0  // en ma  (x 10)


// BALANCE ENTRE MOSFETS
#define BALANCE_ENTRE_MOSFET    -1    // mas negatico= mas mv en mosfet B

// TOUCH
// These are the default min and maximum values, set to 0 and 4095 to test the screen
#define HMIN 0
#define HMAX 4095
#define VMIN 0
#define VMAX 4095
#define XYSWAP 0 // 0 or 1

// This is the screen size for the raw to coordinate transformation
// width and height specified for landscape orientation
#define HRES 320 /* Default screen resulution for X axis */
#define VRES 320 /* Default screen resulution for Y axis */
// ---------------------------------------------------------------------

// TFT
//#define TFT_ORIENTACION 3       // Orientacion del display. valores de 1 a 4 (Todas las coordenadas estan configuradas para 1)
#define TFT_ORIENTACION 1       // Orientacion del display. valores de 1 a 4 (Todas las coordenadas estan configuradas para 1)
#define timeSplash  1000        // Tiempo de la pantalla SplashScreen Inicial
// ---------------------------------------------------------------------

// COOLER
#define COOLER_POTENCIA_MINIMA 80    // potencia minima a la que puede funcionar el cooler.  (de 0 a 255)
#define COOLER_TEMP_MIN 30  // Temperatura a la que enciente el cooler a la potencia minima
#define COOLER_TEMP_MAX 70  // Temperatura en la que el cooler lega el 100% de potencia

// Timeout pantallas
#define TIMEOUT_MENU 3
#define TIMEOUT_SET_SELECCION 5


//#define CORRECCION_MEDICION_VOLTAJE 0.9030   //Correccion de corriente para compensar errores en resistencias, etc

//#define CANT_MEDIAS_VOLT 50
//#define AJUSTE_TEMP 1.2

#define DAC_REFERENCE 4093          // millivotls

#define FPS_DISPLAY 10
#define FPS_SET_CURRENT 2           // Veces por segundo que seteo la corriente

#define TIME_READ_TEMPS 1           // seconds

//#define DESFASEMV_0V 4
//#define DESFASEMV_2V -80

#define MAX_AMP 20000   // Max amperaje de la carga en ma



