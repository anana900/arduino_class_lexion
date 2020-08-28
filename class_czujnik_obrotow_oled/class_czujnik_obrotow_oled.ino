/*
 * Support
 * Oled library manual https://github.com/olikraus/u8g2/wiki/u8g2reference
 * Fonts https://github.com/olikraus/u8g2/wiki/fntgrpiconic#open_iconic_app_1x
 */

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

// Dane nieulotne

struct Ustawienia
{
  int globalna_limit_obroty_alarm = 200;
  bool global_czy_uzywac_alarmu = false;
} class_ustawienia;

#include "obsluga_eeprom.h"

// CZUJNIKI
uint8_t CZUJNIK_OBROTOW_ROZRZUTNIKA = 2;

// PRZYCISKI
uint8_t P_LEWY = 11;
uint8_t P_PRAWY = 12;
uint8_t P_ENTER = 13;

int licznik_pokretla = 0;
int lewy_stan;
int lewy_ostatni_stan;

// ALARM
uint8_t ALARM = A0;

const uint8_t CZUJNIK_POMIAR_OPOZNIENIE = 10;
const unsigned long MINUTE = 60000;

const int LICZBA_CZUJNIKOW_TEMPERATURY = 4;
int temperatura[LICZBA_CZUJNIKOW_TEMPERATURY] = {0};

void setup(void)
{
  Serial.begin(9600);
  u8g2.begin();

  pinMode(CZUJNIK_OBROTOW_ROZRZUTNIKA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CZUJNIK_OBROTOW_ROZRZUTNIKA), isr_zliczanie_impulsow_czujnik_rozrzutnika, FALLING);

  digitalWrite(P_ENTER, HIGH); // eliminuje dzwiek przy uruchamianiu

  pinMode (P_LEWY, INPUT);
  pinMode (P_PRAWY, INPUT);
  pinMode (P_ENTER, INPUT_PULLUP);
  
  lewy_ostatni_stan = digitalRead(P_LEWY);

  pinMode (ALARM, OUTPUT);

  //save_config(class_ustawienia); // - uzywamy tylko przy pierwszyl ladowaniu na plytke
  read_config(class_ustawienia);
}

static unsigned long czasOstatnio = 0;
volatile unsigned long t_start = 0;
volatile unsigned long t_end = 0;
volatile unsigned long t_diff = 0;
void isr_zliczanie_impulsow_czujnik_rozrzutnika()
{
  unsigned long czasTeraz = millis();
  if (czasTeraz - czasOstatnio < CZUJNIK_POMIAR_OPOZNIENIE)
  {
    // eliminacja drgan czujnika
    return;
  }

  t_start = millis();
  t_diff = abs(t_start - t_end);
  t_end = t_start;
 
  czasOstatnio = czasTeraz;
}

int obroty_na_min = 0;
bool obroty_zmiana = false;
unsigned long t_obroty_odswiezanie = 0;
const int t_obroty_odswiezanie_interwal = 1000;
void obroty_na_minute2()
{
  // Jako improvement mozna dodac usrednianie wynikow przed wyswietleniem
  // wyswietlenie jest co ok 500ms
  
  obroty_zmiana = false;

  if (t_diff > 1 && t_diff < t_obroty_odswiezanie_interwal)
  {
    obroty_na_min = MINUTE/t_diff;
  }
  else
  {
    obroty_na_min = 0;
  }

  if (millis() - t_obroty_odswiezanie > t_obroty_odswiezanie_interwal)
  {
    obroty_zmiana = true;
    t_obroty_odswiezanie = millis();
    t_diff = 0;
  }
}

int pokretlo_fix_counter = 0;
bool pokretlo_zmiana = false;
void pokretlo()
{
  pokretlo_zmiana = false;
  lewy_stan = digitalRead(P_LEWY);
  
  if (lewy_stan != lewy_ostatni_stan)
  {
    if (digitalRead(P_PRAWY) != lewy_stan)
    {
      pokretlo_fix_counter++;
    }
    else
    {
      pokretlo_fix_counter--;
    }
  }

  if(pokretlo_fix_counter == 2)
  {
    pokretlo_zmiana = true;
    licznik_pokretla++;
    pokretlo_fix_counter = 0;
    Serial.print("Licznik pokretla: ");
    Serial.println(licznik_pokretla);
  }
  else if (pokretlo_fix_counter == -2)
  {
    pokretlo_zmiana = true;
    licznik_pokretla--;
    pokretlo_fix_counter = 0;
    Serial.print("Licznik pokretla: ");
    Serial.println(licznik_pokretla);
  }

  lewy_ostatni_stan = lewy_stan;
}

unsigned long t_enter = 0;
bool p_enter_przycisniety_krotko = false;
bool p_enter_przycisniety_dlugo = false;
bool enter_krotki = false;
bool enter_dlugi = false;
void przycisk()
{
  if (!digitalRead(P_ENTER))
  {
      if (millis() - t_enter > 50 && p_enter_przycisniety_krotko == false )
      {
        // sygnalizacja dzwiekowa
        p_enter_przycisniety_krotko = true;
      }
      
      if (millis() - t_enter > 1000 && p_enter_przycisniety_dlugo == false)
      {
        // sygnalizacja dzwiekowa
        p_enter_przycisniety_dlugo = true;
        tone(ALARM, 1600);
        delay(100);
        noTone(ALARM);
      }
  }
  else
  {
    if (p_enter_przycisniety_krotko == true && p_enter_przycisniety_dlugo == false)
    {
      Serial.println("Enter krotkie");
      enter_krotki = true;
      p_enter_przycisniety_krotko = false; 
    }
    else
    {
      enter_krotki = false;
    }

    if (p_enter_przycisniety_dlugo == true)
    {
      Serial.println("Enter dlugie");
      enter_dlugi = true;
      p_enter_przycisniety_dlugo = false;
      p_enter_przycisniety_krotko = false; 
    }
    else
    {
      enter_dlugi = false;
    }
    
    t_enter = millis();
  }
}

unsigned long t_alarm_odliczanie = 0;
bool alarm_aktywny = false;
int licznik_pokretla_stary = licznik_pokretla;
void dzwiek_przycisku()
{
  if (pokretlo_zmiana || enter_krotki || enter_dlugi)
  {
    alarm_aktywny = true;
    tone(ALARM, 1600);
    t_alarm_odliczanie = millis();
  }

  if (millis() - t_alarm_odliczanie > 50 && alarm_aktywny)
  {
    alarm_aktywny = false;
    noTone(ALARM);
  }
}

unsigned long t_tymczasowe_wyciszenie_przyciskiem = 0;
unsigned long t_alarm_odliczanie_2 = 0;
bool sygnal = true;
bool pauza = false;
void alarm(int poziom = 0)
{
  if (obroty_na_min < poziom && class_ustawienia.global_czy_uzywac_alarmu)
  {
    if (enter_krotki)
    {
      pauza = true;
      t_tymczasowe_wyciszenie_przyciskiem = millis();
    }
    
    if(pauza && millis() - t_tymczasowe_wyciszenie_przyciskiem < 30000)
    {
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
    }
  }
  else
  {
    t_tymczasowe_wyciszenie_przyciskiem = millis();
    pauza = false;
    noTone(ALARM);
  }
}

unsigned long t_liczydlo = 0;
void ustawienie_limitu_obrotow(bool kierunek)
{
  if(class_ustawienia.globalna_limit_obroty_alarm < 10000 && kierunek)
  {
    if(millis() - t_liczydlo < 200 && class_ustawienia.globalna_limit_obroty_alarm <= 9990)
    {
      class_ustawienia.globalna_limit_obroty_alarm += 10;
    }
    else
    {
      class_ustawienia.globalna_limit_obroty_alarm++;
    }
    t_liczydlo = millis();
  }
  else if (class_ustawienia.globalna_limit_obroty_alarm > 0 && !kierunek)
  {
    if(millis() - t_liczydlo < 200 && class_ustawienia.globalna_limit_obroty_alarm >= 10)
    {
      class_ustawienia.globalna_limit_obroty_alarm -= 10;
    }
    else
    {
      class_ustawienia.globalna_limit_obroty_alarm--;
    }
    t_liczydlo = millis();
  }
}

uint8_t ol_0 = 0;
uint8_t ol_1 = 0;
uint8_t ol_2 = 0;
uint8_t ol_3 = 0;

void oprint_ramka(int x = 0, int w = 32)
{
  u8g2.drawFrame(x,0,w,32);
}

void oprint_rozrzutnik(int obroty)
{
  u8g2.setFont(u8g2_font_open_iconic_app_4x_t);
  u8g2.drawGlyph(0, 32, 0x0040);
  u8g2.setFont(u8g2_font_helvR24_tr);
  u8g2.setCursor(35, 30);

  if (obroty > 999)
  {
    u8g2.setFont(u8g2_font_helvR18_tr);
    u8g2.setCursor(35, 27);
  }

  u8g2.print(obroty);

  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.drawStr(95,32,"RPM");
}

void oprint_temperatura()
{
  u8g2.drawHLine(0, 17, 128);
  
//  u8g2.drawLine(0, 0, 0, 32);
//  u8g2.drawLine(13, 0, 13, 32);
  u8g2.drawLine(64, 0, 64, 32);
//  u8g2.drawLine(77, 0, 77, 32);

  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.setCursor(3, 15);
  u8g2.print("Ta");
  u8g2.setCursor(31, 15);
  u8g2.print(temperatura[0]);

  u8g2.setCursor(3, 32);
  u8g2.print("Tc");
  u8g2.setCursor(31, 32);
  u8g2.print(temperatura[2]);

  u8g2.setCursor(67, 15);
  u8g2.print("Tb");
  u8g2.setCursor(95, 15);
  u8g2.print(temperatura[1]);

  u8g2.setCursor(67, 32);
  u8g2.print("Td");
  u8g2.setCursor(95, 32);
  u8g2.print(temperatura[3]);
}

void oprint_temp_i_rozrzutnik(int obroty)
{
  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.setCursor(0, 16);
  u8g2.print("Ta");
  u8g2.setCursor(0, 32);
  u8g2.print(temperatura[0]);

  u8g2.setCursor(24, 16);
  u8g2.print("Tb");
  u8g2.setCursor(24, 32);
  u8g2.print(temperatura[1]);

  u8g2.setCursor(48, 16);
  u8g2.print("Tc");
  u8g2.setCursor(48, 32);
  u8g2.print(temperatura[2]);

  u8g2.setCursor(72, 16);
  u8g2.print("Td");
  u8g2.setCursor(72, 32);
  u8g2.print(temperatura[3]);

  u8g2.setCursor(96, 16);
  u8g2.print("RPM");
  u8g2.setCursor(96, 32);
  u8g2.print(obroty);
}

void oprint_menu_opcje()
{
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(8, 25, 0x0048);
  u8g2.setFont(u8g2_font_open_iconic_app_2x_t);
  u8g2.drawGlyph(40, 25, 0x0040);
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(72, 25, 0x0046);
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(104, 25, 0x0044);
}

void oprint_menu_ustawienia_ogolne()
{
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  if (class_ustawienia.global_czy_uzywac_alarmu)
  {
    // dzieki on
    u8g2.drawGlyph(8, 25, 0x0041);
  }
  else
  {
    // dzwieki off
    u8g2.drawGlyph(8, 25, 0x0041);
    u8g2.drawLine(4, 27, 26, 5);
    u8g2.drawLine(5, 27, 27, 5);
    u8g2.drawLine(6, 27, 28, 5);
  }

  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(104, 25, 0x0044);

  // zapis do pamieci
  save_config(class_ustawienia);
}

void oprint_rozrzutnik_opcje(int limit)
{
  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.setCursor(3, 14);
  u8g2.print("Alarm");
  u8g2.setCursor(3, 27);
  u8g2.print("Obroty");

  u8g2.setFont(u8g2_font_helvR18_tr);
  u8g2.setCursor(50, 25);
  u8g2.print(limit);
  
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(104, 25, 0x0044);
}

int ostatnie_pokretlo = 0;
bool oled_odswiez = true;
void oled_manager()
{
  if (pokretlo_zmiana ||
      enter_krotki ||
      enter_dlugi ||
      oled_odswiez)
  {
//    Serial.print("PO   : Poziomy: ");
//    Serial.print(ol_0);
//    Serial.print(ol_1);
//    Serial.print(ol_2);
//    Serial.print(ol_3);
//    Serial.print("  Pokretlo: ");
//    Serial.println(pokretlo_zmiana);
    
    oled_odswiez = false;
    
    if (ol_0 < 3)
    {
      if (licznik_pokretla != ostatnie_pokretlo)
      {
        if (ostatnie_pokretlo < licznik_pokretla)
        {
          if (ol_0 < 2)
          {
            ol_0++;
          }
          else
          {
            ol_0 = 0;
          }
        }
        else
        {
          if (ol_0 > 0)
          {
            ol_0--;
          }
          else
          {
            ol_0 = 2;
          }
        }

        ostatnie_pokretlo = licznik_pokretla;
      }
      else if (enter_dlugi)
      {
        ol_0 = 3;
        ol_1 = 0;
      }

      switch(ol_0)
      {
        case 0:
        {
          u8g2.firstPage();
          oprint_rozrzutnik(obroty_na_min);
          u8g2.nextPage();
          break;
        }
        case 1:
        {
          u8g2.firstPage();
          oprint_temperatura();
          u8g2.nextPage();
          break;
        }
        case 2:
        {
          u8g2.firstPage();
          oprint_temp_i_rozrzutnik(obroty_na_min);
          u8g2.nextPage();
          break;
        }
        case 3:
        {
          // long press
          u8g2.firstPage();
          ol_1 = 0;
          oprint_ramka();
          oprint_menu_opcje();
          u8g2.nextPage();
          break;
        }
      }

    }
    else if (ol_0 == 3)
    {
      if (licznik_pokretla != ostatnie_pokretlo)
      {
        if (ostatnie_pokretlo < licznik_pokretla)
        {
          if (ol_1 < 3)
          {
            ol_1++;
          }
          else
          {
            ol_1 = 0;
          }
        }
        else
        {
          if (ol_1 > 0)
          {
            ol_1--;
          }
          else
          {
            ol_1 = 3;
          }
        }

        ostatnie_pokretlo = licznik_pokretla;
      }

      switch(ol_1)
      {
        case 0:
        {
          u8g2.firstPage();
          oprint_ramka(0,32);
          oprint_menu_opcje();
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 4;
            oled_odswiez = true;
          }
          break;
        }
        case 1:
        {
          u8g2.firstPage();
          oprint_ramka(32,32);
          oprint_menu_opcje();
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 5;
            ol_1 = 0;
            ol_2 = 0;
            oled_odswiez = true;
          }
          break;
        }
        case 2:
        {
          u8g2.firstPage();
          oprint_ramka(64,32);
          oprint_menu_opcje();
          u8g2.nextPage();
          break;
        }
        case 3:
        {
          u8g2.firstPage();
          oprint_ramka(96,32);
          oprint_menu_opcje();
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 0;
            oled_odswiez = true;
          }
          break;
        }
      }
    }
    else if (ol_0 == 4)
    {
      // ustawienia ogolne

      if (licznik_pokretla != ostatnie_pokretlo)
      {
        if (ostatnie_pokretlo < licznik_pokretla)
        {
          if (ol_1 < 1)
          {
            ol_1++;
          }
          else
          {
            ol_1 = 0;
          }
        }
        else
        {
          if (ol_1 > 0)
          {
            ol_1--;
          }
          else
          {
            ol_1 = 1;
          }
        }

        ostatnie_pokretlo = licznik_pokretla;
      }

      switch(ol_1)
      {
        case 0:
        {
          u8g2.firstPage();
          oprint_ramka(0,32);
          oprint_menu_ustawienia_ogolne();
          u8g2.nextPage();
          if (enter_krotki)
          {
            class_ustawienia.global_czy_uzywac_alarmu = !class_ustawienia.global_czy_uzywac_alarmu;
            oled_odswiez = true;
          }
          break;
        }
        case 1:
        {
          u8g2.firstPage();
          oprint_ramka(96,32);
          oprint_menu_ustawienia_ogolne();
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 0;
            oled_odswiez = true;
          }
          break;
        }
      }
    }
    else if (ol_0 == 5)
    {
      // ustawienia rozrzutnika

      if (licznik_pokretla != ostatnie_pokretlo)
      {
        if (ostatnie_pokretlo < licznik_pokretla)
        {
          if (ol_2 == 0)
          {
            if (ol_1 < 1)
            {
              ol_1++;
            }
            else
            {
              ol_1 = 0;
            }
          }
          else if(ol_2 == 1)
          {
            ustawienie_limitu_obrotow(true);
          }
        }
        else
        {
          if (ol_2 == 0)
          {
            if (ol_1 > 0)
            {
              ol_1--;
            }
            else
            {
              ol_1 = 1;
            }
          }
          else if (ol_2 == 1)
          {
            ustawienie_limitu_obrotow(false);
          }
        }
        ostatnie_pokretlo = licznik_pokretla;
      }

      switch(ol_1)
      {
        case 0:
        {
          u8g2.firstPage();
          oprint_ramka(0,96);
          oprint_rozrzutnik_opcje(class_ustawienia.globalna_limit_obroty_alarm);
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_2 = ( ol_2 + 3 ) % 2;
            oled_odswiez = true;

            if (ol_2 == 0)
            {
              // zapisanie do pamieci
              save_config(class_ustawienia);
            }
          }
          if (ol_2 && pokretlo_zmiana)
          {
            oled_odswiez = true;
          }
          break;
        }
        case 1:
        {
          u8g2.firstPage();
          oprint_ramka(96,32);
          oprint_rozrzutnik_opcje(class_ustawienia.globalna_limit_obroty_alarm);
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 0;
            ol_1 = 0;
            ol_2 = 0;
            oled_odswiez = true;
          }
          break;
        }
      }
    }
    else if (ol_0 == 6)
    {
      // ustawienia temperatury
      
    }
  }

  if (ol_0 == 0 && obroty_zmiana)
  {
    u8g2.firstPage();
    oprint_rozrzutnik(obroty_na_min);
    u8g2.nextPage();
  }

  if (ol_0 == 2 && obroty_zmiana)
  {
    u8g2.firstPage();
    oprint_temp_i_rozrzutnik(obroty_na_min);
    u8g2.nextPage();
  }
}

void loop(void)
{
  obroty_na_minute2();
  pokretlo();
  przycisk();
  dzwiek_przycisku();
  oled_manager();
  alarm(class_ustawienia.globalna_limit_obroty_alarm);
}
