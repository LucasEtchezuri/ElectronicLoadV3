bool checkTouch(st_area area, int x, int y)
{
    if ((area.x1 <= x) and (x <= area.x2))
        if ((area.y1 <= y) and (y <= area.y2))
            return true;
    return false;
}

void opcionTouch(int x, int y)
{
    if (strcmp(estado.pantalla, "prohibido") == 0)
    {
        TFT_Pantalla_Inicial();
        strcpy(estado.pantalla, "principal");
    }
    else if (strcmp(estado.pantalla, "setCorte_V") == 0)
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
            TFT_Pantalla_Inicial();
            strcpy(estado.pantalla, "principal");
        }
    }
    else if (strcmp(estado.pantalla, "setCorte_Tiempo") == 0)
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
            TFT_Pantalla_Inicial();
            strcpy(estado.pantalla, "principal");
        }
    }
    else if (strcmp(estado.pantalla, "setCorte_Temperatura") == 0)
    {
        TFT_Pantalla_Inicial();
        strcpy(estado.pantalla, "principal");
    }
    else if (strcmp(estado.pantalla, "principal") == 0)
    {
        if (checkTouch(ONOFF, x, y))
        {
            if (estado.estado == 0)
            {
                estado.estado = 1;
                estado.tiempoInicio = millis();
                segundosAnt = 0;
                estado.ah=0;
            }
            else
            {
                estado.estado = 0;
                estado.tiempoInicio = 0;
            }
            TFT_Estado();
        }
        if (checkTouch(MODE, x, y))
        {
            if (estado.estado == 0)
            {
                if (estado.modo == 1)
                    estado.modo = 2;
                else if (estado.modo == 2)
                    estado.modo = 3;
                else
                    estado.modo = 1;
                TFT_Modo();
            }
            else
            {
                spriteProhibidoCambioModo.pushSprite(20, 20);
                strcpy(estado.pantalla, "prohibido");
                timeoutMenu = TIMEOUT_MENU * 1000;
                timeoutMenuAnt = millis();
            }
        }
        if (checkTouch(SET_AMPERE, x, y))
        {
            estado.setSeleccion = 10000;
            timeoutSetSeleccionAnt = millis();
            timeoutSetSeleccion = TIMEOUT_SET_SELECCION * 1000;
        }
        if (checkTouch(SET_AMPERE_100, x, y))
        {
            estado.setSeleccion = 1000;
            timeoutSetSeleccionAnt = millis();
            timeoutSetSeleccion = TIMEOUT_SET_SELECCION * 1000;
        }
        if (checkTouch(SET_AMPERE_10, x, y))
        {
            estado.setSeleccion = 100;
            timeoutSetSeleccionAnt = millis();
            timeoutSetSeleccion = TIMEOUT_SET_SELECCION * 1000;
        }
        if (checkTouch(SET_AMPERE_1, x, y))
        {
            estado.setSeleccion = 10;
        }
        if (checkTouch(SET_VOLTAJE_MINIMO, x, y))
        {
            if (dobleClickAnt == 0)
            {
                dobleClickAnt = millis();
                if (estado.setCorteVoltajeMinimo)
                    estado.setCorteVoltajeMinimo = false;
                else
                    estado.setCorteVoltajeMinimo = true;
                TFT_VIN();
            }
            else
            {
                if (estado.setCorteVoltajeMinimo)
                    estado.setCorteVoltajeMinimo = false;
                else
                    estado.setCorteVoltajeMinimo = true;
                TFT_VIN();
                seleccionCorte = 1;
                strcpy(estado.pantalla, "setCorte_V");
                timeoutMenu = TIMEOUT_MENU * 1000;
                timeoutMenuAnt = millis();
            }
        }
        if (checkTouch(SET_TIEMPO, x, y))
        {
            if (dobleClickAnt == 0)
            {
                dobleClickAnt = millis();
                if (estado.setCorteTiempo)
                    estado.setCorteTiempo = false;
                else
                    estado.setCorteTiempo = true;
                TFT_Tiempo();
            }
            else
            {
                if (estado.setCorteTiempo)
                    estado.setCorteTiempo = false;
                else
                    estado.setCorteTiempo = true;
                TFT_Tiempo();
                seleccionCorte = 1;
                strcpy(estado.pantalla, "setCorte_Tiempo");
                timeoutMenu = TIMEOUT_MENU * 1000;
                timeoutMenuAnt = millis();
            }
        }

        if (checkTouch(SET_TEMP, x, y))
        {
            if (dobleClickAnt == 0)
            {
                dobleClickAnt = millis();
                if (estado.setCorteTemperatura)
                    estado.setCorteTemperatura = false;
                else
                    estado.setCorteTemperatura = true;
                TFT_Temp();
            }
            else
            {
                if (estado.setCorteTemperatura)
                    estado.setCorteTemperatura = false;
                else
                    estado.setCorteTemperatura = true;
                TFT_Temp();
                seleccionCorte = 1;
                strcpy(estado.pantalla, "setCorte_Temperatura");
                timeoutMenu = TIMEOUT_MENU * 1000;
                timeoutMenuAnt = millis();
            }
        }
    }
    X_Raw = 0;
    Y_Raw = 0;
}