struct st_currents
{
  unsigned long currentA;         // current medida en ma * 100
  unsigned long currentB;         // current medida en ma * 100
  unsigned long currentTotal;     // current suma de A + B
};

struct st_set
{
  byte mode;              // Modo (0=I   1=V    2=P)
  int selCurrent; //milliamp - Select Current.  (different of SET current)
  int vCutOff;   //millivolts  (0 = disabled)
  int tempCutOff;         // (0 = disabled)
  int tempCutOffDUT;      // (0 = disabled)
  unsigned int tCutOff;   // time in seconds to CutOff
};

struct st_status
{
  bool run;             // true=RUNNING false=STOP
  st_currents currents; // (currentA, currentB, currentTotal)
  long voltage;         // mv
  long power;           // milliwatts
  float temp;
  float tempDUT;
  int FanPower;
  unsigned long initTime; // time in seconds running
  volatile byte pantalla;      // Pantalla en la que se encuentra el programa 
  unsigned long setCurrent;
  volatile long selUnidad;
};


struct st_area
{
  int x1;
  int y1;
  int x2;
  int y2;
};

st_area ONOFF;
st_area MODE;
st_area SET_AMPERE;
st_area SET_DECIMAL;
st_area SET_AMPERE_100;
st_area SET_AMPERE_10;
st_area SET_AMPERE_1;
st_area SET_VOLTAJE_MINIMO;
st_area SET_TIEMPO;
st_area SET_TEMP;
st_area SET_CORTE_V_ENTERO;
st_area SET_CORTE_V_DECIMAL;
st_area SET_CORTE_T_MIN;
st_area SET_CORTE_T_SEC;
