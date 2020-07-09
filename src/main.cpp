#include <Arduino.h>

//---------------------------------------------------------------
#include <User_Setup.h> //
// User_Setup.h se sobreescribe al actualizar la libreria. Setear los siguientes pines:
//#define TFT_MOSI 13
//#define TFT_SCLK 14
//#define TFT_CS   17  // Chip select control pin
//#define TFT_DC    16  // Data Command control pin
//#define TFT_RST   22  // Reset pin (could connect to RST pin)
//Setar la frecuencia en 10MHZ

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
st_status status;
st_set set;
st_currents currents;

unsigned int touchDebounce = 30;
volatile unsigned int touchAnt = 0;
unsigned int encoderDebounce = 50;
volatile unsigned int encoderAnt = 0;
unsigned long timeDisplayMenu = 0;
unsigned long timeSelectionDigit = 0;
unsigned long timeSetCurrentAnt = 0;
bool dibujaSet = false;
unsigned vMinCant = 0;
int dobleClick = 300;
int dobleClickAnt = 0;
int seleccionCorte = 1;

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

  pinMode(ENCODER_A, INPUT); // ENCODER entrada A
  pinMode(ENCODER_B, INPUT); // ENCODER entrada B
  //pinMode(ENCODER_BUTTON, INPUT); // ENCODER button
  pinMode(TOUCH_IRQ, INPUT); // TOUCH IRQ
  pinMode(CS_DAC_A, OUTPUT); // CS DAC
  pinMode(CS_DAC_B, OUTPUT); // CS DAC
  pinMode(FAN, OUTPUT);      // Fan Externo PWM
  pinMode(BUZZER, OUTPUT);   // Buzzer
  pinMode(V_SELECT, OUTPUT); // Sensor Voltaje
  //pinMode(REGULATOR_ENABLE, OUTPUT);      // Sensor Voltaje
  //digitalWrite(REGULATOR_ENABLE, LOW); // disable regulator

  tft.init();

  adc.begin(14, 32, 13, ADC_CS_PIN, ADC_READY_PIN);

  dac.DAC8830_REFERENCE_MV = -1;
  dac.DAC8830_CS_PIN = CS_DAC_A;
  digitalWrite(CS_DAC_A, HIGH); // Set CS high
  dac.setReference(DAC_REFERENCE);
  dac.writeDAC(0);

  adc.setGain(1);
  adc.setOpMode(0x02);         //Turbo Mode
  adc.setDataRate(0x06);       // 2000SPS
  adc.setConversionMode(0x01); //modo continuo
  adc.setMultiplexer(0x08);    // AIN0
  adc.setVoltageRef(0);

  ledcSetup(0, 15000, 8); // Set PWM FAN
  ledcAttachPin(FAN, 0);  // Set PWM FAN

  cargarCoordenadas(); // setea los valores de coordenadas de TFT de las opciones
  initStatus();        // init status structure
  initSet();           // init set structure
  setCurrent(set.selCurrent);

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

  tft.setRotation(TFT_ORIENTACION);
  touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP); // Raw xmin, xmax, ymin, ymax, width, height
  touch.setRotation(2);

  TFT_Creacion_Sprites();      // Creacion de los sprites
  TFT_Pantalla_SplashScreen(); // Dibujo de la pantall de bienvenida
  TFT_Pantalla_Completa();     // Dibujo de la pantalla principal y todos los valores
  TFT_DatosEnPantalla();
  TFT_Set_Corte_V(); //Grafica el valor de SET de Corte
  TFT_Set_Corte_Tiempo();
  TFT_Set_Corte_Temperatura();
  delay(500);
}

void loop()
{

  static unsigned long refreshDisplayAnt = 0;
  static unsigned long readTempsAnt = 0;

  activacionInterrupcionTouch();   // debounce y activacion de la interrupcion del touch. ya que dentro de ISRtouch, la interrupcion de desactiva para no llamarla mas de una vez.
  activacionInterrupcionEncoder(); // debounce y activacion de la interrupcion del ENCODER
  resetDobliClick();

  readCurrentsVoltage(); // mide la corriente (cada vez que ejecuta mide en una entrada diferente).  Promedia 10 lecturas.
  TFT_SpriteCooler();    // Dibuja el sprite del cooler

  if (status.run)
  {
    checkCutOffV();
    //checkCorteTemp();
    //checkCorteTime();

    if ((timeSetCurrentAnt == 0) or ((timeSetCurrentAnt + (1000 / FPS_SET_CURRENT)) < millis())) // FRECUENCIA DE ACTUALIZACION DAC
    {
      setCurrent(set.selCurrent);
      timeSetCurrentAnt = millis();
    }

    // Frecuencia de actualizar Corriente
    //adjustCurrent();
  }
  else
  {
    if ((timeSetCurrentAnt == 0) or ((timeSetCurrentAnt + (1000 / FPS_SET_CURRENT)) < millis())) // FRECUENCIA DE ACTUALIZACION DAC
    {
      setCurrent(0);
      timeSetCurrentAnt = millis();
    }
  }

  if ((refreshDisplayAnt == 0) or ((refreshDisplayAnt + (1000 / FPS_DISPLAY)) < millis())) // FRECUENCIA DE ACTUALIZACION TFT
  {
    TFT_Info(); //Grafica el recuadro de Info con los valores
    refreshDisplayAnt = millis();
  }

  if ((readTempsAnt == 0) or ((readTempsAnt + (TIME_READ_TEMPS * 1000)) < millis())) // LECTURA TEMP Y SET COOLER
  {
    readTemps();
    powerCooler();
    readTempsAnt = millis();
    Serial.print("Actual Current A = ");
    Serial.println(status.currents.currentA / 100.00);
    Serial.print("Actual Current B = ");
    Serial.println(status.currents.currentB / 100.00);
    Serial.print("Current total = ");
    Serial.println(status.currents.currentTotal / 100.00);
    Serial.print("Actual Voltage");
    Serial.println(status.voltage);
  }
  //TFT_DibujaPantallaPrincipal(); // Dibuja la pntalla principal solo si paso el timeout (timeoutMenu)

  TFT_Dibuja(); // Dibuja la pntalla principal solo si paso el timeout (timeoutSetSeleccion)

  if (X_Raw != 0 or Y_Raw != 0)
  {
    opcionTouch(X_Raw, Y_Raw);
  }
}