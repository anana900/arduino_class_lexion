
unsigned long t_odczytanie_temperatur = 0;
void czytaj_temperature()
{
  temperatura_zmiana = false;

  if (millis() - t_odczytanie_temperatur > 5000)
  {
    czujnik_temperatury.requestTemperatures();

    temperatura_zmiana = true;
    temperatura_alarm = false;
 
    for (int adres_index = 0 ; adres_index < LICZBA_CZUJNIKOW_TEMPERATURY ; ++adres_index )
    {
      float temp = czujnik_temperatury.getTempC(ds1820_adresy[adres_index]);

      if (temp > class_ustawienia.global_limit_temperatura_alarm)
      {
        temperatura_alarm = true;
        temperatura[1][adres_index] = "1";
      }
      else
      {
        temperatura[1][adres_index] = "0";
      }
      
      if (temp == -127)
      {
          temperatura[0][adres_index] = "Nc";
      }
      else if (temp > -55 && temp < 125)
      {
          temperatura[0][adres_index] = String(int(temp));
      }
      else if (temp >= 125)
      {
        temperatura[0][adres_index] = "max";
      }

    t_odczytanie_temperatur = millis(); 
    }
  }
}
