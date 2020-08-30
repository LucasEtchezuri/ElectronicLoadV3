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
  int mvA = 0;
  int mvB = 0;
  SPI.setDataMode(SPI_MODE0);

  ma = ma * 10;

  if ((ma + CORRECCION_MA_DAC) < 0)
  {
    ma = 0;
  }
  else
  {
    ma = ma + CORRECCION_MA_DAC;
  }

  int mv = (ma * 2000.00 / 20000.00); // Obtengo mv necesarios para esos ma.
  if (BALANCE_ENTRE_MOSFET >= 0)
  {
    mvA = mv + BALANCE_ENTRE_MOSFET;
    mvB = mv - BALANCE_ENTRE_MOSFET;
  }
  else
  {
    float tmp = -1 * BALANCE_ENTRE_MOSFET;
    mvA = mv - tmp;
    mvB = mv + tmp;
  }

  if (mv <= 0)
  {
    dacA.setMillivolts(0);
    dacB.setMillivolts(0);
  }
  else
  {
    dacA.setMillivolts10(mvA);
    dacB.setMillivolts10(mvB);
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
        timeSelectionDigit = millis();
        if (set.selCurrent - status.selUnidad <= 0)
          set.selCurrent = 0;
        else
        {
          set.selCurrent = set.selCurrent - status.selUnidad; // decrementa POSICION en 1
        }
        dibujaSet = true;
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
        timeSelectionDigit = millis();
        if (MAX_AMP <= set.selCurrent + status.selUnidad)
          set.selCurrent = MAX_AMP;
        else
        {
          set.selCurrent = set.selCurrent + status.selUnidad;
        }
        dibujaSet = true;
      }
    }
    encoderAnt = millis(); // guarda valor actualizado del tiempo
  }
}
void ISRtouch()
{
  detachInterrupt(TOUCH_IRQ);
  X_Raw = map(touch.ReadRawY(), 0, 4095, 4095, 0);
  Y_Raw = touch.ReadRawX();
  touchAnt = millis();
}

void readCurrentsVoltage(void)
{
  static byte adc_input = 0;
  static byte adc_reads = 0;
  static long val = 0;
  static long suma = 0;
  SPI.setDataMode(SPI_MODE1);

  if (adc.isDataReady())
  {
    if (adc_input == 0)
    {
      val = adc.readADC();
      suma = suma + (val * 2048.000 / 32767.000 / INA_MV_A * 10000.00 * 100.00);
      adc_reads++;
      if (adc_reads >= 50)
      {
        status.currents.currentA = (suma / 50);
        status.currents.currentTotal = status.currents.currentA + status.currents.currentB;
        adc.setMultiplexer(0x09);
        adc_reads = 0;
        adc_input = 1;
        suma = 0;
      }
    }
    else if (adc_input == 1)
    {
      val = adc.readADC();
      suma = suma + (val * 2048.000 / 32767.000 / INA_MV_A * 10000.00 * 100.00);

      adc_reads++;
      if (adc_reads >= 50)
      {
        status.currents.currentB = (suma / 50);
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
      if (adc_reads >= 50)
      {
        status.voltage = (suma / 50);
        status.power = ((status.currents.currentTotal / 100.00) * status.voltage) / 1000;
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

  x = constrain(status.temp, COOLER_TEMP_MIN - 1, COOLER_TEMP_MAX);
  if (x == COOLER_TEMP_MIN - 1)
  {
    ledcWrite(0, 0);
    status.FanPower = 0;
  }
  else
  {
    x = map(x, COOLER_TEMP_MIN - 1, COOLER_TEMP_MAX, COOLER_POTENCIA_MINIMA, 255);
    ledcWrite(0, x);
    status.FanPower = map(x, COOLER_POTENCIA_MINIMA, 255, 1, 100);
  }
}

void adjustCurrent(void)
{
}

void readTemps(void)
{
  sensor.requestTemperatures();
  status.temp = sensor.getTempC();
}
