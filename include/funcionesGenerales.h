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
  status.selUnidad = 1;
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

void setCurrent(long ma)
{
  SPI.setDataMode(SPI_MODE0);
  int mv = (ma * 2000.00 / 5000.00); // Obtengo mv necesarios para esos ma.
  if (mv <= 0)
  {
    dac.setMillivolts(0);
  }
  else
  {
    dac.setMillivolts(mv);
  }

  SPI.setDataMode(SPI_MODE1);
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
      if (status.pantalla == PANTALLA_SET_CORTE_V)
      {
        set.vCutOff = set.vCutOff - seleccionCorte;
      }
      else if (status.pantalla == PANTALLA_SET_CORTE_TIME)
      {
        set.tCutOff = set.tCutOff - seleccionCorte;
      }
      else if (status.pantalla == PANTALLA_SET_CORTE_TEMP)
      {
        set.tempCutOff = set.tempCutOff - seleccionCorte;
        if (set.tempCutOff > 200)
          set.tempCutOff = 200;
      }
      else
      {
        timeoutSetSeleccionAnt = millis();
        if (set.selCurrent <= 0)
          set.selCurrent = 0;
        else
        {
          set.selCurrent = set.selCurrent - status.selUnidad; // decrementa POSICION en 1
        }
      }
    }
    else
    { // si B es LOW, sentido anti horario
      if (status.pantalla == PANTALLA_SET_CORTE_V)
      {
        set.vCutOff = set.vCutOff + seleccionCorte;
      }
      else if (status.pantalla == PANTALLA_SET_CORTE_TIME)
      {
        set.tCutOff = set.tCutOff + seleccionCorte;
      }
      else if (status.pantalla == PANTALLA_SET_CORTE_TEMP)
      {
        set.tempCutOff = set.tempCutOff + seleccionCorte;
        if (set.tempCutOff < 0)
          set.tempCutOff = 0;
      }
      else
      {
        timeoutSetSeleccionAnt = millis();
        if (10000 < set.selCurrent)
          set.selCurrent = 10000;
        else
        {
          set.selCurrent = set.selCurrent + status.selUnidad;
        }
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
  SPI.setDataMode(SPI_MODE1);

  if (adc.isDataReady())
  {
    if (adc_input == 0)
    {
      suma = suma + (adc.readADC() * MA_MAX / 32767.00 * 100.00);
      adc_reads++;
      if (adc_reads >= 100)
      {
        status.currents.currentA = (suma / 100);
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
      if (adc_reads >= 100)
      {
        status.currents.currentB = (suma / 100);
        status.currents.currentB = 0;    //quitar cuando agrego el otro modulo
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
  SPI.setDataMode(SPI_MODE0);
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
  ledcWrite(0, 0);
}

void adjustCurrent(void)
{
}

void readTemps(void)
{
}
