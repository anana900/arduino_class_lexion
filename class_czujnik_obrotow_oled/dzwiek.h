
unsigned long t_alarm_odliczanie = 0;
bool dzwiek_przycisku_status = false;
int licznik_pokretla_stary = licznik_pokretla;
void dzwiek_przycisku()
{
  if (pokretlo_zmiana || enter_krotki || enter_dlugi)
  {
    dzwiek_przycisku_status = true;
    tone(ALARM, 1600);
    t_alarm_odliczanie = millis();
  }

  if (millis() - t_alarm_odliczanie > 50 && dzwiek_przycisku_status)
  {
    dzwiek_przycisku_status = false;
    noTone(ALARM);
  }
}

int alarm_aktywacja_zewnetrzna(uint8_t port, bool g_aktywacja)
{
  /*
   * Opcja pozwala na kontrolowanie uruchamiania alarmu
   * dzwiekowego porzez monitorowanie stanu na jednym z portow.
   */
  if (g_aktywacja)
  {
    if (analogRead(port) > 500)
    {
      // aktywowanie pracy alarmu z zewnetrzengo systemu
      return true;
    }
    return false;
  }
  return true;
}

unsigned long t_tymczasowe_wyciszenie_przyciskiem = 0;
unsigned long t_alarm_odliczanie_2 = 0;
bool sygnal = true;
bool pauza = false;
bool first_enter = true;
void alarm()
{
  if (((obroty_alarm && alarm_aktywacja_zewnetrzna(ALARM_AKTYWACJA_ZEWNETRZNA, class_ustawienia.global_alarm_aktywacja_zewnetrzna))
       ||
       temperatura_alarm)
       &&
       class_ustawienia.global_czy_uzywac_alarmu)
  {
    if (first_enter)
    {
      first_enter = false;
      pauza = false;
      t_tymczasowe_wyciszenie_przyciskiem = millis();
    }
    
    if (enter_krotki)
    {
      pauza = true;
      t_tymczasowe_wyciszenie_przyciskiem = millis();
    }
    
    if(pauza && millis() - t_tymczasowe_wyciszenie_przyciskiem < 30000)
    {
      wyswietl_alarm = false;
      return;
    }
    else
    {
      pauza = false;
    }

    if (millis() - t_tymczasowe_wyciszenie_przyciskiem > 5000)
    {
      if (sygnal)
      {
        tone(ALARM, 1600);
      }
      else
      {
        noTone(ALARM);
      }
  
      if (millis() - t_alarm_odliczanie_2 > 100)
      {
        sygnal = !sygnal;
        t_alarm_odliczanie_2 = millis();
      }

      wyswietl_alarm = true;
    } 
    else
    {
      wyswietl_alarm = false;
    }
  }
  else
  {
    wyswietl_alarm = false;
    first_enter = true;
    pauza = false;
    noTone(ALARM);
  }
}
