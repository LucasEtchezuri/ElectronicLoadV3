//MODO
int tft_pos_modo_x = 90;
int tft_pos_modo_y = 4;

//ESTADO
int tft_pos_estado_x = 315;
int tft_pos_estado_y = 4;

//SET
int tft_pos_set_x = 15;
int tft_pos_set_y = 60;

void cargarCoordenadas(void)
{
    ONOFF.x1 = 2400;
    ONOFF.y1 = 0;
    ONOFF.x2 = 3700;
    ONOFF.y2 = 800;

    MODE.x1 = 0; 
    MODE.y1 = 0;
    MODE.x2 = 1700;
    MODE.y2 = 800;

    SET_AMPERE.x1 =600;
    SET_AMPERE.y1 =900;
    SET_AMPERE.x2 =1400;
    SET_AMPERE.y2 =1700;

    SET_DECIMAL.x1 =1400;
    SET_DECIMAL.y1 =1200;
    SET_DECIMAL.x2 =2100;
    SET_DECIMAL.y2 =1550;
    
    
    SET_AMPERE_100.x1 =1401;
    SET_AMPERE_100.y1 =900;
    SET_AMPERE_100.x2 =1700;
    SET_AMPERE_100.y2 =1700;
    
    SET_AMPERE_10.x1 =1701;
    SET_AMPERE_10.y1 =900;
    SET_AMPERE_10.x2 =2000;
    SET_AMPERE_10.y2 =1700;
    
    SET_AMPERE_1.x1 =2001;
    SET_AMPERE_1.y1 =900;
    SET_AMPERE_1.x2 =2300;
    SET_AMPERE_1.y2 =1700;

    SET_VOLTAJE_MINIMO.x1 = 700;
    SET_VOLTAJE_MINIMO.y1 = 2100;
    SET_VOLTAJE_MINIMO.x2 = 1250;
    SET_VOLTAJE_MINIMO.y2 = 2600;

    SET_TIEMPO.x1 = 1300;
    SET_TIEMPO.y1 = 2100;
    SET_TIEMPO.x2 = 1900;
    SET_TIEMPO.y2 = 2600;

    SET_TEMP.x1 = 2050;
    SET_TEMP.y1 = 2100;
    SET_TEMP.x2 = 2550;
    SET_TEMP.y2 = 2600;
    
    SET_CORTE_V_ENTERO.x1 = 1150;
    SET_CORTE_V_ENTERO.y1 = 1550;
    SET_CORTE_V_ENTERO.x2 = 1750;
    SET_CORTE_V_ENTERO.y2 = 2350;

    SET_CORTE_V_DECIMAL.x1 = 1950;
    SET_CORTE_V_DECIMAL.y1 = 1550;
    SET_CORTE_V_DECIMAL.x2 = 2600;
    SET_CORTE_V_DECIMAL.y2 = 2350;

    SET_CORTE_T_MIN.x1 = 1400;
    SET_CORTE_T_MIN.y1 = 1550;
    SET_CORTE_T_MIN.x2 = 2100;
    SET_CORTE_T_MIN.y2 = 2350;

    SET_CORTE_T_SEC.x1 = 2150;
    SET_CORTE_T_SEC.y1 = 1550;
    SET_CORTE_T_SEC.x2 = 2900;
    SET_CORTE_T_SEC.y2 = 2350;
    
}