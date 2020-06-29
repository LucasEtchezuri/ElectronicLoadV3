#include <Arduino.h>
#include <User_Setup.h>
#include <TFT_Touch.h>
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "ADS1120.h"
#include "estructuras.h"
#include "configHard.h"

TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT); /* Create an instance of TOUCH */
TFT_eSPI tft = TFT_eSPI();                         /* Create an instance of TFT screen */
ADS1120 adc;
TFT_eSprite spriteCooler = TFT_eSprite(&tft); // Sprite
TFT_eSprite spriteProhibidoCambioModo = TFT_eSprite(&tft);
TFT_eSprite spriteSetCorte = TFT_eSprite(&tft);

// Variables Globales  ----------------
volatile unsigned int X_Raw;
volatile unsigned int Y_Raw;
st_estado estado;
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
int adc_entrada = 0;
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
  delay(500);
  pinMode(encoderA, INPUT);      // ENCODER entrada A
  pinMode(encoderB, INPUT);      // ENCODER entrada B
  pinMode(encoderButton, INPUT); // ENCODER button
  pinMode(touchIRQ, INPUT);      // TOUCH IRQ
  pinMode(CS_DAC_A, OUTPUT);     // CS DAC
  pinMode(ADC_READY_PIN, INPUT); // ADC ready conversion
  //pinMode(tempInterna, INPUT);     // Temp Interna
  pinMode(tempDisipador, INPUT); // Temp Disipador
  pinMode(tempDUT, INPUT);       // Temp DUT
  pinMode(fanDisipador, OUTPUT); // Fan Externo PWM
  pinMode(buzzer, OUTPUT);       // Buzzer
  pinMode(vSelect, OUTPUT);      // Sensor Voltaje
  //pinMode(regulacionEnable, OUTPUT);      // Sensor Voltaje

  //setearVolt(3000, 4100); // Lo primeor que hago por seguridad seteo 0 a la salida del DAC
  ledcSetup(0, 15000, 8);
  ledcAttachPin(fanDisipador, 0);

  adc.begin(ADC_CS_PIN, ADC_READY_PIN);
  
  SPI.begin(14, 12, 13);
  digitalWrite(CS_DAC_A, HIGH); // SS is pin 10
  tft.init();
  tft.setRotation(TFT_ORIENTACION);

  touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP); // Raw xmin, xmax, ymin, ymax, width, height
  touch.setRotation(2);

  cargarCoordenadas(); // setea los valores de coordenadas de TFT de las opciones
  inicializarEstado(); // inicializa las variables de la estructura estado.

    
  attachInterrupt(digitalPinToInterrupt(encoderA), ISRencoder, FALLING); // interrupcion sobre pin A del encoder
  attachInterrupt(digitalPinToInterrupt(touchIRQ), ISRtouch, FALLING);   // interrupcion del touch 1=normal   0=Presionado
  //attachInterrupt(digitalPinToInterrupt(conversionReady), ISR_ADC, FALLING); // interrupcion del touch 1=normal   0=Presionado

  //#define RISING    0x01
  //#define FALLING   0x02
  //#define CHANGE    0x03
  //#define ONLOW     0x04
  //#define ONHIGH    0x05
  //#define ONLOW_WE  0x0C
  //#define ONHIGH_WE 0x0D

  TFT_Creacion_Sprites();      // Creacion de los sprites
  TFT_Pantalla_SplashScreen(); // Dibujo de la pantall de bienvenida
  TFT_Pantalla_Inicial();      // Dibujo de la pantalla principal y los elementos fijos
  TFT_Set();                   // Dibujo el numero seteado
  Serial.println("Termino Setup");
  
}

void loop()
{

  //SPI.begin(14, 12, 13);
  SPI.setDataMode(SPI_MODE0);
  uint8_t xhigh = 0;
  uint8_t xlow = 0;



  while (1)
  {
    xhigh = 0;
    xlow = 0;

    digitalWrite(CS_DAC_A, LOW); // SS is pin 10
    delayMicroseconds(1);
    SPI.transfer(xhigh);
		SPI.transfer(xlow);
    delayMicroseconds(1);
    digitalWrite(CS_DAC_A, HIGH);

    delay(2000);

    xhigh = 128;
    xlow = 0;

    digitalWrite(CS_DAC_A, LOW); // SS is pin 10
    delayMicroseconds(1);
    SPI.transfer(xhigh);
		SPI.transfer(xlow);
    delayMicroseconds(1);
    digitalWrite(CS_DAC_A, HIGH);

    delay(2000);

    xhigh = 255;
    xlow = 0;

    digitalWrite(CS_DAC_A, LOW); // SS is pin 10
    delayMicroseconds(1);
    SPI.transfer(xhigh);
		SPI.transfer(xlow);
    delayMicroseconds(1);
    digitalWrite(CS_DAC_A, HIGH);

    delay(2000);

    xhigh = 0;
    xlow = 10;

    digitalWrite(CS_DAC_A, LOW); // SS is pin 10
    delayMicroseconds(1);
    SPI.transfer(xhigh);
		SPI.transfer(xlow);
    delayMicroseconds(1);
    digitalWrite(CS_DAC_A, HIGH);

    delay(2000);
    Serial.println("Funcionando");
  }
  SPI.setDataMode(SPI_MODE1);



/*
  activacionInterrupcionTouch();   // debounce y activacion de la interrupcion del touch. ya que dentro de ISRtouch, la interrupcion de desactiva para no llamarla mas de una vez.
  activacionInterrupcionEncoder(); // debounce y activacion de la interrupcion del ENCODER

  resetDobliClick();

  //obtenerCorriente(); // mide la corriente (cada vez que ejecuta mide en un modulo)

  TFT_SpriteCooler(); // Dibuja el sprite del cooler

  TFT_DibujaPantallaPrincipal(); // Dibuja la pntalla principal solo si paso el timeout (timeoutMenu)
  TFT_DibujaSetSeleccion();      // Dibuja la pntalla principal solo si paso el timeout (timeoutSetSeleccion)

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
  */
}