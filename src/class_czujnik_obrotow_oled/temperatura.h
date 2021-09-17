
unsigned long t_odczytanie_temperatur = 0;

void ds1820_wyswietl_adres(DeviceAddress ds_adres)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (ds_adres[i] < 0x10)
    {
      Serial.print("0");
    }
    Serial.print(ds_adres[i], HEX);
    if (i < 7)
    {
      Serial.print(", ");
    }
  }
  Serial.println("");
}

void czytaj_temperature()
{
  temperatura_zmiana = false;

  if (millis() - t_odczytanie_temperatur > 5000)
  {
#ifdef DEBUG
    Serial.println("Czytanie temperatury");
#endif
    czujnik_temperatury.requestTemperatures();

    temperatura_zmiana = true;
    temperatura_alarm = false;
 
    for (int adres_index = 0 ; adres_index < LICZBA_CZUJNIKOW_TEMPERATURY ; ++adres_index )
    {
#ifdef DEBUG
      ds1820_wyswietl_adres(class_ustawienia.ds1820_adresy[adres_index]);
#endif
      float temp = czujnik_temperatury.getTempC(class_ustawienia.ds1820_adresy[adres_index]);

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
