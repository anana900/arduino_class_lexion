#include <EEPROM.h>

void save_config(Ustawienia &item)
{
  EEPROM.put(100, item);
}

void read_config(Ustawienia &item)
{
  EEPROM.get(100, item);
}
