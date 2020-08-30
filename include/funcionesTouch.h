bool checkTouch(st_area area, int x, int y)
{
    if ((area.x1 <= x) and (x <= area.x2))
        if ((area.y1 <= y) and (y <= area.y2))
            return true;
    return false;
}

void opcionTouch(int x, int y)
{
    if (status.pantalla == PANTALLA_PROHIBIDO)
    {
        TFT_Pantalla_Completa();
        status.pantalla = PANTALLA_PRINCIPAL;
    }
    else if (status.pantalla == PANTALLA_SET_CORTE_V)
    {
        if (checkTouch(SET_CORTE_V_ENTERO, x, y))
        {
            seleccionCorte = 100;
        }
        else if (checkTouch(SET_CORTE_V_DECIMAL, x, y))
        {
            seleccionCorte = 1;
        }
        else
        {
            TFT_Pantalla_Completa();
            status.pantalla = PANTALLA_PRINCIPAL;
        }
    }
    else if (status.pantalla == PANTALLA_SET_CORTE_TIME)
    {
        if (checkTouch(SET_CORTE_T_MIN, x, y))
        {
            seleccionCorte = 60;
        }
        else if (checkTouch(SET_CORTE_T_SEC, x, y))
        {
            seleccionCorte = 1;
        }
        else
        {
            TFT_Pantalla_Completa();
            status.pantalla = PANTALLA_PRINCIPAL;
        }
    }
    else if (status.pantalla == PANTALLA_SET_CORTE_TEMP)
    {
        TFT_Pantalla_Completa();
        status.pantalla = PANTALLA_PRINCIPAL;
    }
    else if (status.pantalla == PANTALLA_PRINCIPAL)
    {
        if (checkTouch(ONOFF, x, y))
        {
            if (!status.run)
            {
                status.run = true;
                status.initTime = millis();
            }
            else
            {
                status.run = false;
                status.initTime = 0;
            }
            TFT_Estado();
        }
        if (checkTouch(MODE, x, y))
        {
            if (!status.run)
            {
                if (set.mode == 1)
                    set.mode = 2;
                else if (set.mode == 2)
                    set.mode = 3;
                else
                    set.mode = 1;
                TFT_Modo();
            }
            else
            {
                spriteProhibidoCambioModo.pushSprite(20, 20);
                status.pantalla = PANTALLA_PROHIBIDO;
                timeDisplayMenu = millis();
            }
        }
        if (checkTouch(SET_AMPERE, x, y))
        {
            status.selUnidad = 1000;
            timeSelectionDigit = millis();
            dibujaSet = true;
        }
        if (checkTouch(SET_DECIMAL, x, y))
        {
            if (status.selUnidad == 1000 or status.selUnidad == 1)
            {
                status.selUnidad = 100;
            }
            else if (status.selUnidad == 100)
            {
                status.selUnidad = 10;
            }
            else if (status.selUnidad == 10)
            {
                status.selUnidad = 1;
            }
            
            timeSelectionDigit = millis();
            dibujaSet = true;
        }

        /*
        if (checkTouch(SET_AMPERE_100, x, y))
        {
            status.selUnidad = 100;
            timeSelectionDigit = millis();
            dibujaSet=true;
        }
        if (checkTouch(SET_AMPERE_10, x, y))
        {
            status.selUnidad = 10;
            timeSelectionDigit = millis();
            dibujaSet=true;
        }
        if (checkTouch(SET_AMPERE_1, x, y))
        {
            status.selUnidad = 1;
            dibujaSet=true;
        }
        */
        if (checkTouch(SET_VOLTAJE_MINIMO, x, y))
        {
            if (dobleClickAnt == 0)
            {
                dobleClickAnt = millis();
                if (set.vCutOff > 0)
                    set.vCutOff = 0;
                else
                    //set.vCutOff = vCutOffEEPROM();
                    TFT_CutOffVoltage();
            }
            else
            {
                TFT_CutOffVoltage();
                seleccionCorte = 1;
                status.pantalla = PANTALLA_SET_CORTE_V;
                timeDisplayMenu = millis();
            }
        }
        if (checkTouch(SET_TIEMPO, x, y))
        {
            if (dobleClickAnt == 0)
            {
                dobleClickAnt = millis();
                if (set.tCutOff > 0)
                    set.tCutOff = 0;
                else
                    //set.tCutOff = tCutOffEEPROM();
                    TFT_CutOffTime();
            }
            else
            {
                TFT_CutOffTime();
                seleccionCorte = 1;
                status.pantalla = PANTALLA_SET_CORTE_TIME;
                timeDisplayMenu = millis();
            }
        }

        if (checkTouch(SET_TEMP, x, y))
        {
            if (dobleClickAnt == 0)
            {
                dobleClickAnt = millis();
                if (set.tempCutOff > 0)
                    set.tempCutOff = 0;
                else
                    //set.tempCutOff = tempCutOffEEPROM();
                    TFT_CutOffTemp();
            }
            else
            {
                TFT_CutOffTemp();
                seleccionCorte = 1;
                status.pantalla = PANTALLA_SET_CORTE_TEMP;
                timeDisplayMenu = millis();
            }
        }
    }
    X_Raw = 0;
    Y_Raw = 0;
}