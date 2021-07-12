#include <EEPROM.h>

void zapisz_konfiguracje(Ustawienia &item)
{
  EEPROM.put(100, item);
}

void odczytaj_konfiguracje(Ustawienia &item)
{
  EEPROM.get(100, item);
}
