struct st_estado
{
  volatile int8_t estado;        // 1=RUNNING 2=STOP
  volatile int8_t modo;          // 1=I,  2=V,  3=W
  volatile long set;              // Seteo actual
  volatile long v;            // Voltaje actual
  volatile long i;            // Intensidad actual
  volatile long i0;           // Intensidad actual
  volatile long i1;           // Intensidad actual
  volatile long i2;           // Intensidad actual
  volatile long i3;           // Intensidad actual
  volatile int32_t w;            // Potencia actual
  volatile int32_t tiempoInicio; // Segundos actual
  volatile int32_t segundos;     // Segundos actual
  volatile float ah;
  volatile int coolerDisipadorPotencia; // Velocidad cooler 0-100
  volatile int temperaturaDisipador;
  volatile int temperaturaDUT;
  char pantalla[30];
  volatile int16_t setSeleccion=10;
  volatile int16_t corteVoltajeMinimo;
  volatile bool setCorteVoltajeMinimo;
  volatile int32_t corteTiempo;
  volatile bool setCorteTiempo;
  volatile int16_t corteTemperatura;
  volatile bool setCorteTemperatura;
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

