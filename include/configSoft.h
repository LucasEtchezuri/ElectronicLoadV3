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
#define TFT_ORIENTACION 3       // Orientacion del display. valores de 1 a 4 (Todas las coordenadas estan configuradas para 1)
#define timeSplash  1000        // Tiempo de la pantalla SplashScreen Inicial
// ---------------------------------------------------------------------

// COOLER
#define COOLER_POTENCIA_MINIMA 60    // potencia minima a la que puede funcionar el cooler.
#define COOLER_TEMP_MIN 35  // Temperatura a la que enciente el cooler a la potencia minima
#define COOLER_TEMP_MAX 70  // Temperatura en la que el cooler lega el 100% de potencia

// Timeout pantallas
#define TIMEOUT_MENU 3
#define TIMEOUT_SET_SELECCION 5

#define CORRECCION_SET_CORRIENTE 1   //Correccion de corriente para compensar errores en resistencias, etc
#define CORRECCION_LECTURA_CORRIENTE 1.009   //Correccion de corriente para compensar errores en resistencias, etc - calibrado en 1 ampere
#define DERIVA_X_AMPERE 10   //deriva de MV por AMPERE
#define DERIVA_X_10VOLT 10   //deriva de MA por cada 10V de entrada

#define CORRECCION_LECTURA_CORRIENTE_SUMA 0.0   //Correccion de corriente para compensar errores en resistencias, etc

#define CORRECCION_MEDICION_VOLTAJE 0.9030   //Correccion de corriente para compensar errores en resistencias, etc

#define CANT_MEDIAS_VOLT 50
#define AJUSTE_TEMP 1.2

#define ADC_REFERENCE 4096          // millivotls


