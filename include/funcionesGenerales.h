void initStatus(void)
{
  currents.currentA = 0;
  currents.currentB = 0;
  currents.currentTotal = 0;
  status.currents = currents;
  status.FanRpm = 0;
  status.run = false;
  status.temp = 0;
  status.tempDUT = 0;
  status.time = 0;
}

void initSet(void)
{
  set.mode = 1;
  set.setCurrent = 0;
  set.tCutOff = 0;
  set.tempCutOff = 0;
  set.tempCutOffDUT = 0;
  set.vCutOff = 0;
}

void setearVolt(uint mv, uint vMax)
{
  float tmp = (65535.0 / vMax);

  uint x = mv * tmp;

  uint8_t xlow = x & 0xff;
  uint8_t xhigh = (x >> 8);

  digitalWrite(CS_DAC_A, LOW); // SS is pin 10
  tft.spiwrite(xhigh);
  tft.spiwrite(xlow);
  digitalWrite(CS_DAC_A, HIGH);
}

void ISRencoder()
{
  detachInterrupt(ENCODER_A);
  int veces = 0;
  for (int x = 0; x < 4000; x++)
  {
    if (digitalRead(ENCODER_A) == LOW)
      veces++;
  }
  if (veces > 2500)
  {
    if (digitalRead(ENCODER_B) == HIGH) // si B es HIGH, sentido horario
    {
      if (strcmp(estado.pantalla, "setCorte_V") == 0)
      {
        estado.corteVoltajeMinimo = estado.corteVoltajeMinimo - seleccionCorte;
      }
      else if (strcmp(estado.pantalla, "setCorte_Tiempo") == 0)
      {
        estado.corteTiempo = estado.corteTiempo - seleccionCorte;
      }
      else if (strcmp(estado.pantalla, "setCorte_Temperatura") == 0)
      {
        estado.corteTemperatura = estado.corteTemperatura + seleccionCorte;
        if (estado.corteTemperatura > 200)
          estado.corteTemperatura = 200;
      }
      else
      {
        estado.set = estado.set - estado.setSeleccion; // decrementa POSICION en 1
        timeoutSetSeleccionAnt = millis();
        CORRECCION_CORRIENTE = 0.0;
        if (estado.set < 0)
          estado.set = 0;
      }
    }
    else
    { // si B es LOW, senti anti horario
      if (strcmp(estado.pantalla, "setCorte_V") == 0)
      {
        //estado.setCorteVoltajeMinimo = estado.setCorteVoltajeMinimo + estado.setCorteSeleccion;
        estado.corteVoltajeMinimo = estado.corteVoltajeMinimo + seleccionCorte;
      }
      else if (strcmp(estado.pantalla, "setCorte_Tiempo") == 0)
      {
        //estado.setCorteVoltajeMinimo = estado.setCorteVoltajeMinimo + estado.setCorteSeleccion;
        estado.corteTiempo = estado.corteTiempo + seleccionCorte;
      }
      else if (strcmp(estado.pantalla, "setCorte_Temperatura") == 0)
      {
        estado.corteTemperatura = estado.corteTemperatura - seleccionCorte;
        if (estado.corteTemperatura < 0)
          estado.corteTemperatura = 0;
      }
      else
      {
        estado.set = estado.set + estado.setSeleccion; // incrementa POSICION en 1
        timeoutSetSeleccionAnt = millis();
        CORRECCION_CORRIENTE = 0.0;
        if (200000 < estado.set)
          estado.set = 200000;
      }
    }
    encoderAnt = millis(); // guarda valor actualizado del tiempo
    actualizarDisplayAnt = 0;
    actualizarEstadoAnt = 0;
  }
}
void ISRtouch()
{
  detachInterrupt(TOUCH_IRQ);
  X_Raw = touch.ReadRawY();
  Y_Raw = touch.ReadRawX();
  //Serial.print("X = ");
  //Serial.println(X_Raw);
  //Serial.print("Y = ");
  //Serial.println(Y_Raw);
  touchAnt = millis();
}

void readCurrents(void)
{
  static byte adc_input = 0;

  if (adc.isDataReady())
  {
    if (adc_input == 0)
    {
      status.currents.currentA = adc.readADC();
      status.currents.currentTotal = status.currents.currentA + status.currents.currentB;
      adc_input = 1;
      adc.setMultiplexer(0x09);
    }
    else if (adc_input == 1)
    {
      status.currents.currentB = adc.readADC();
      status.currents.currentTotal = status.currents.currentA + status.currents.currentB;
      adc_input = 0;
      adc.setMultiplexer(0x08);
    }
  }
}

/*
void obtenerCorriente(void)
{
  if (conversionActiva and adc.isDataReady())
  {
    if (adc_entrada == 0)
    {
      estado.i0 = (adc.readADC_Single() * 10);
      if (estado.i0 < 0)
        estado.i0 = 0;
      adc_entrada = 2;
    }
    else if (adc_entrada == 1)
    {
      estado.i1 = (adc.readADC_Single() * 10);
      if (estado.i1 < 0)
        estado.i1 = 0;
      adc_entrada = 3;
    }
    else if (adc_entrada == 2)
    {
      estado.temperaturaDisipador = (adc.readADC_Single() - 500) / 10;
      estado.temperaturaDisipador = estado.temperaturaDisipador * AJUSTE_TEMP;
      if (estado.coolerDisipadorPotencia > 0)
      {
        ledcWrite(0, map(estado.coolerDisipadorPotencia, 1, 100, COOLER_POTENCIA_MINIMA, 255));
      }
      else
      {
        ledcWrite(0, 0);
      }
      adc_entrada = 3;
    }
    else if (adc_entrada == 3)
    {
      estado.v = adc.readADC_Single();
      estado.v = estado.v * 10;
      estado.v = estado.v / CORRECCION_MEDICION_VOLTAJE;
      if (estado.v < 0)
        estado.v = 0;
      adc_entrada = 0;
    }
    estado.i = (estado.i0 + estado.i1) * CORRECCION_LECTURA_CORRIENTE;

    int mvCompensacionA = map(estado.i, 10000, 40000, 0, 40 * DERIVA_X_AMPERE);

    //estado.i = estado.i - (uint)((((float)estado.i / 10000) - 1) * DERIVA_X_AMPERE * 10);
    estado.i = estado.i - (mvCompensacionA);

    int mvCompensacion = map(estado.v, 0, 60000, -90, 450);

    estado.i = estado.i + mvCompensacion;

    conversionActiva = false;
  }
  else if ((conversionActiva == false) and ((obtenerCorrienteAnt == 0) or (obtenerCorrienteAnt + (1000 / actualizarEstadoFPS)) < millis())) // Obtiene el voltaje de cada entrada del ADS1115 (ADC).  Cada vez que ejecuta obtiene 1 de las 4 entradas
  {
    if (adc_entrada == 0)
    {
      adc.setMultiplexer(0x00);
    }
    if (adc_entrada == 1)
    {
      adc.setMultiplexer(0x00);
    }
    if (adc_entrada == 2)
    {
      adc.setMultiplexer(0x00);
    }
    if (adc_entrada == 3)
    {
      adc.setMultiplexer(0x00);
      obtenerCorrienteAnt = millis();
    }
    conversionActiva = true;
  }
}

*/

void actualizarEstado(void)
{

  estado.w = (estado.v * estado.i) / 1000000.00;
  if (estado.tiempoInicio != 0)
  {
    estado.segundos = (millis() - estado.tiempoInicio) / 1000;
  }
  if (estado.segundos > segundosAnt)
  {
    estado.ah = estado.ah + (estado.i / 3600.00 / 10000.00);
    segundosAnt = estado.segundos;
  }
  // estado.temperaturaDisipador = map(analogRead(tempDisipador), 0, 4095, 0, 330);
  //Serial.println(estado.temperaturaDisipador);
  //estado.temperaturaDisipador = estado.temperaturaDisipador + map(analogRead(tempDisipador), 0, 4095, 0, 330);
  //estado.temperaturaDisipador = (estado.temperaturaDisipador + map(analogRead(tempDisipador), 0, 4095, 0, 330)) / 3;
  estado.coolerDisipadorPotencia = map(estado.temperaturaDisipador, COOLER_TEMP_MIN, COOLER_TEMP_MAX, 0, 100);
  if (estado.coolerDisipadorPotencia < 0)
  {
    estado.coolerDisipadorPotencia = 0;
  }
  else if (estado.coolerDisipadorPotencia > 100)
  {
    estado.coolerDisipadorPotencia = 100;
  }
}

void activacionInterrupcionTouch(void)
{
  if (digitalRead(TOUCH_IRQ) == 0)
    touchAnt = millis();
  else if ((touchAnt != 0) and ((millis() - touchAnt) > touchDebounce))
  {
    X_Raw = 0;
    Y_Raw = 0;
    attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ), ISRtouch, FALLING); // interrupcion del touch 1=normal   0=Presionado
    touchAnt = 0;
  }
}

void activacionInterrupcionEncoder(void)
{
  if (((millis() - encoderAnt) > encoderDebounce))
  {
    attachInterrupt(digitalPinToInterrupt(ENCODER_A), ISRencoder, FALLING); // interrupcion del touch 1=normal   0=Presionado
    encoderAnt = 0;
  }
}

void resetDobliClick(void)
{
  //if (digitalRead(touchIRQ) == 0)
  //  dobleClickAnt = millis();
  //else
  if ((dobleClickAnt != 0) and ((millis() - dobleClickAnt) > dobleClick))
  {
    dobleClickAnt = 0;
  }
}

void checkCorteV()
{
  if (estado.estado and estado.setCorteVoltajeMinimo)
  {
    if ((estado.v / 10) <= estado.corteVoltajeMinimo)
    {
      estado.estado = 0;
      TFT_Estado();
    }
  }
}
