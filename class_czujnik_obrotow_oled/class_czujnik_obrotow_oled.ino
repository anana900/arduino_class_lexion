/*
 * Support
 * Oled library manual https://github.com/olikraus/u8g2/wiki/u8g2reference
 * Fonts https://github.com/olikraus/u8g2/wiki/fntgrpiconic#open_iconic_app_1x
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

#include "zmienne_globalne.h"
#include "rpm.h"
#include "manipulator.h"
#include "dzwiek.h"
#include "obsluga_eeprom.h"
#include "oled.h"

OneWire oneWire(ONE_WIRE_CZUJNIK_TEMPERATURA);
DallasTemperature czujnik_temperatury(&oneWire);

#include "temperatura.h"

void setup(void)
{
  Serial.begin(9600);

 /*
 * Obsluga wyswietlacza
 */
  u8g2.begin();

/*
 * Pomiar RPM (liczby obrotow na minut) z wykozystaniem przerwan
 */
  pinMode(CZUJNIK_OBROTOW_ROZRZUTNIKA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CZUJNIK_OBROTOW_ROZRZUTNIKA), isr_zliczanie_impulsow_czujnik_rozrzutnika, FALLING);

/*
 * Obsluga one wire - pomiar temperatury
 */
  czujnik_temperatury.begin();
  for(int index = 0 ; index < LICZBA_CZUJNIKOW_TEMPERATURY ; ++index)
  {
    czujnik_temperatury.setResolution(ds1820_adresy[index], 9);
  }

/*
 * Eliminuje dzwiek przy uruchamianiu
 */
  digitalWrite(P_ENTER, HIGH);

/*
 * Obsługa manipulatora / enkodera z przyciskiem
 */
  pinMode(P_LEWY, INPUT);
  pinMode(P_PRAWY, INPUT);
  pinMode(P_ENTER, INPUT_PULLUP);
  lewy_ostatni_stan = digitalRead(P_LEWY);

/*
 * Obsługa dziekow
 */
  pinMode(ALARM, OUTPUT);
  pinMode(ALARM_AKTYWACJA_ZEWNETRZNA, INPUT);

/*
 * Obsługa pamiecie EEPROM
 */
  //zapisz_konfiguracje(class_ustawienia); // uzywamy tylko przy pierwszyl ladowaniu na plytke
  odczytaj_konfiguracje(class_ustawienia);
}

void loop(void)
{
  czytaj_temperature();
  obroty_na_minute();
  pokretlo();
  przycisk();
  dzwiek_przycisku();
  oled_manager();
  alarm();
}
