void initStatus(void)
{
  status.run = false;
  currents.currentA = 0;
  currents.currentB = 0;
  currents.currentTotal = 0;
  status.currents = currents;
  status.voltage = 0;
  status.power = 0;
  status.temp = 0;
  status.tempDUT = 0;
  status.FanPower = 0;
  status.initTime = 0;
  status.pantalla = 0;
  status.setCurrent = 0;
  status.selUnidad = 1 * 100;
}

void initSet(void)
{
  set.mode = 1;
  set.selCurrent = 0;
  set.tCutOff = 0;
  set.tempCutOff = 0;
  set.tempCutOffDUT = 0;
  set.vCutOff = 0;
}

void setCurrent(unsigned long ma)
{
  unsigned int mv = ma * 2000.00 / 5000.00; // Obtengo mv necesarios para esos ma.
  dac.setMillivolts(mv);
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
        if (estado.set < 0)
          estado.set = 0;
      }
    }
    else
    { // si B es LOW, sentido anti horario
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
        if (200000 < estado.set)
          estado.set = 200000;
      }
    }
    encoderAnt = millis(); // guarda valor actualizado del tiempo
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

void readCurrentsVoltage(void)
{
  static byte adc_input = 0;
  static byte adc_reads = 0;
  static unsigned long suma = 0;

  if (adc.isDataReady())
  {
    if (adc_input == 0)
    {
      suma = suma + (adc.readADC() * MA_MAX / 32767.00 * 100.00);
      adc_reads++;
      if (adc_reads >= 10)
      {
        status.currents.currentA = (suma / 10);
        status.currents.currentTotal = status.currents.currentA + status.currents.currentB;
        adc.setMultiplexer(0x09);
        adc_reads = 0;
        adc_input = 1;
        suma = 0;
      }
    }
    else if (adc_input == 1)
    {
      suma = suma + (adc.readADC() * MA_MAX / 32767.00 * 100.00);
      adc_reads++;
      if (adc_reads >= 10)
      {
        status.currents.currentB = (suma / 10);
        status.currents.currentTotal = status.currents.currentA + status.currents.currentB;
        adc.setMultiplexer(0x05);
        adc_reads = 0;
        adc_input = 2;
        suma = 0;
      }
    }
    else
    {
      suma = suma + (adc.readADC() * MV_MAX / 32767.00);
      adc_reads++;
      if (adc_reads >= 10)
      {
        status.voltage = (suma / 10);
        status.power = (status.currents.currentTotal / 100.00) * status.voltage;
        adc.setMultiplexer(0x08);
        adc_reads = 0;
        adc_input = 0;
        suma = 0;
      }
    }
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

void powerOn(void)
{
  {
    status.run = true;
    TFT_Estado();
    digitalWrite(REGULATOR_ENABLE, HIGH);
  }
}

void powerOff(void)
{
  {
    status.run = false;
    TFT_Estado();
    digitalWrite(REGULATOR_ENABLE, LOW);
  }
}

void checkCutOffV(void)
{
  if (status.run and set.vCutOff > 0)
  {
    if (status.voltage >= set.vCutOff)
    {
      powerOff();
      TFT_Estado();
    }
  }
}

void powerCooler(void)
{
  byte x = 0;
  if (status.run)
  {
    x = map(status.temp, COOLER_TEMP_MIN, COOLER_TEMP_MAX, COOLER_POTENCIA_MINIMA, 255);
    ledcWrite(0, x);
    status.FanPower = map(x, COOLER_POTENCIA_MINIMA, 255, 1, 100);
  }
  else
  {
    x = map(status.temp, COOLER_TEMP_MIN, COOLER_TEMP_MAX, COOLER_POTENCIA_MINIMA, 255);
    ledcWrite(0, x);
    status.FanPower = map(x, COOLER_POTENCIA_MINIMA, 255, 1, 100);
  }
  // ledcWrite(0, 0);
}

void adjustCurrent(void){

}

void readTemps(void){

}


