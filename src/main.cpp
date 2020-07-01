#include <Arduino.h>
#include <user_setup_tft.h>
#include <TFT_Touch.h>
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include <ADS1120.h>
#include <DAC8830.h>

#include "estructuras.h"
#include "configHard.h"

TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT); /* Create an instance of TOUCH */
TFT_eSPI tft = TFT_eSPI();                         /* Create an instance of TFT screen */
TFT_eSprite spriteCooler = TFT_eSprite(&tft);      // Sprite
TFT_eSprite spriteProhibidoCambioModo = TFT_eSprite(&tft);
TFT_eSprite spriteSetCorte = TFT_eSprite(&tft);

ADS1120 adc;
DAC8830 dac;

// Variables Globales  ----------------
volatile unsigned int X_Raw;
volatile unsigned int Y_Raw;
st_estado estado;
st_status status;
st_set set;
st_currents currents;

unsigned int touchDebounce = 30;
volatile unsigned int touchAnt = 0;
unsigned int encoderDebounce = 50;
unsigned int encoderAnt = 0;
unsigned int dibujarCoolerAnt = 0;
unsigned int velocidadCooler = 2; // 1= min   30=max
unsigned int timeoutMenu = 0;     // Segundos
unsigned int timeoutMenuAnt = 0;
unsigned int timeoutSetSeleccion = 0; // Segundos
unsigned int timeoutSetSeleccionAnt = 0;
unsigned vMinCant = 0;
int32_t segundosAnt = 0;
int dobleClick = 300;
int dobleClickAnt = 0;
int seleccionCorte = 1;
int posMedia = 0;

volatile unsigned int actualizarDisplayAnt = 0;
volatile unsigned int actualizarEstadoAnt = 0;
unsigned int obtenerCorrienteAnt = 0;
unsigned int FPS_Display = 10;         // Veces por segundo que se actualizan los datos en el display
unsigned int actualizarEstadoFPS = 30; // veces por segundo que se calculan los datos del estado y se actualiza el seteo de corriente

bool conversionActiva = false;
int rotacionCooler = 0;

int ant = 7687;
int valor = 0;

int ANTERIOR = 0; // almacena valor anterior de la variable POSICION

volatile int POSICION = 0; // variable POSICION con valor inicial de 50 y definida

float CORRECCION_CORRIENTE = 0.0;

// ------------------------------------

#include "configSoft.h"
#include "TFT_colores.h"
#include "TFT_Coordenadas.h"
#include "TFT_pantallas.h"
#include "funcionesTouch.h"
#include "funcionesGenerales.h"

void setup(void)
{
  Serial.begin(115200);

  pinMode(ENCODER_A, INPUT);      // ENCODER entrada A
  pinMode(ENCODER_B, INPUT);      // ENCODER entrada B
  pinMode(ENCODER_BUTTON, INPUT); // ENCODER button
  pinMode(TOUCH_IRQ, INPUT);      // TOUCH IRQ
  pinMode(CS_DAC_A, OUTPUT);      // CS DAC
  pinMode(CS_DAC_B, OUTPUT);      // CS DAC
  pinMode(ADC_READY_PIN, INPUT);  // ADC ready conversion
  pinMode(FAN, OUTPUT);           // Fan Externo PWM
  pinMode(BUZZER, OUTPUT);        // Buzzer
  pinMode(V_SELECT, OUTPUT);      // Sensor Voltaje
  //pinMode(REGULATOR_ENABLE, OUTPUT);      // Sensor Voltaje

  digitalWrite(REGULATOR_ENABLE, LOW); // disable regulator

  //dac.setReference(DAC_REFERENCE);
  //dac.writeDAC(0);

  //SPI.begin(14, 12, 13);                // cambiar nueva lib
  //adc.begin(ADC_CS_PIN, ADC_READY_PIN); // cambiar nueva lib

  ledcSetup(0, 15000, 8); // Set PWM FAN
  ledcAttachPin(FAN, 0);  // Set PWM FAN

  cargarCoordenadas(); // setea los valores de coordenadas de TFT de las opciones
  initStatus();        // init status structure
  initSet();           // init set structure

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), ISRencoder, FALLING); // interrupcion sobre pin A del encoder
  attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ), ISRtouch, FALLING);   // interrupcion del touch 1=normal   0=Presionado

  //attachInterrupt(digitalPinToInterrupt(conversionReady), ISR_ADC, FALLING); // interrupcion del touch 1=normal   0=Presionado   // no creo que lo use
  //#define RISING    0x01
  //#define FALLING   0x02
  //#define CHANGE    0x03
  //#define ONLOW     0x04
  //#define ONHIGH    0x05
  //#define ONLOW_WE  0x0C
  //#define ONHIGH_WE 0x0D

  tft.init();
  tft.setRotation(TFT_ORIENTACION);
  touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP); // Raw xmin, xmax, ymin, ymax, width, height
  touch.setRotation(2);

  TFT_Creacion_Sprites();      // Creacion de los sprites
  TFT_Pantalla_SplashScreen(); // Dibujo de la pantall de bienvenida
  TFT_Pantalla_Inicial();      // Dibujo de la pantalla principal y los elementos fijos
  TFT_Set();                   // Dibujo el numero seteado
  Serial.println("Termino Setup");
  delay(500);
}

void loop()
{

  activacionInterrupcionTouch();   // debounce y activacion de la interrupcion del touch. ya que dentro de ISRtouch, la interrupcion de desactiva para no llamarla mas de una vez.
  activacionInterrupcionEncoder(); // debounce y activacion de la interrupcion del ENCODER
  resetDobliClick();

  readCurrents(); // mide la corriente (cada vez que ejecuta mide en un modulo)
  //obtenerCorriente(); // mide la corriente (cada vez que ejecuta mide en un modulo)

  TFT_SpriteCooler(); // Dibuja el sprite del cooler

  TFT_DibujaPantallaPrincipal(); // Dibuja la pntalla principal solo si paso el timeout (timeoutMenu)
  TFT_DibujaSetSeleccion();      // Dibuja la pntalla principal solo si paso el timeout (timeoutSetSeleccion)

/*
  if ((actualizarEstadoAnt) == 0 or ((actualizarEstadoAnt + (1000 / actualizarEstadoFPS)) < millis()))
  {

    if (estado.estado)
    {
      if (estado.i != 0)
      {

        int dif = estado.set - estado.i;
        CORRECCION_CORRIENTE = CORRECCION_CORRIENTE + ((float)dif / 10);
        if (estado.set + CORRECCION_CORRIENTE < 0)
        {
          CORRECCION_CORRIENTE = 0.0;
        }
        else if (estado.set + CORRECCION_CORRIENTE > 40960)
        {
          CORRECCION_CORRIENTE = 0.0;
        }
        //Serial.println(estado.set);
        //Serial.println(estado.i);

        //Serial.println(CORRECCION_CORRIENTE);
      }
    }
    uint voltSet = (uint)(estado.set * CORRECCION_SET_CORRIENTE);

    voltSet = (uint)(estado.estado * (voltSet + CORRECCION_CORRIENTE));

    setearVolt(voltSet, 4096);
    actualizarEstado(); //Actualiza la estructura estado con todos los valores menos la corriente. La corriente la actualiza "obtenerCorriente()"

    checkCorteV();
    actualizarEstadoAnt = millis();
  }

*/
  if ((actualizarDisplayAnt == 0) or ((actualizarDisplayAnt + (1000 / FPS_Display)) < millis()))
  {
    TFT_Info();        //Grafica el recuadro de Info con los valores
    TFT_Set();         //Grafica el valor de SET
    TFT_Set_Corte_V(); //Grafica el valor de SET de Corte
    TFT_Set_Corte_Tiempo();
    TFT_Set_Corte_Temperatura();

    actualizarDisplayAnt = millis();
  }

  if (X_Raw != 0 or Y_Raw != 0)
  {
    opcionTouch(X_Raw, Y_Raw);
  }

}