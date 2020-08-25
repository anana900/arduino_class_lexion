/*
 * Support
 * Oled library manual https://github.com/olikraus/u8g2/wiki/u8g2reference
 * Fonts https://github.com/olikraus/u8g2/wiki/fntgrpiconic#open_iconic_app_1x
 */

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

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

volatile word licznik_czujnik_obrotow_rozrzutnika = 0;
const uint8_t CZUJNIK_POMIAR_OPOZNIENIE = 10;
const unsigned int MINUTE = 60000;

void setup(void)
{
  Serial.begin(9600);
  u8g2.begin();

  pinMode(CZUJNIK_OBROTOW_ROZRZUTNIKA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CZUJNIK_OBROTOW_ROZRZUTNIKA), zliczanie_impulsow_czujnik_rozrzutnika, FALLING);

  digitalWrite(P_ENTER, HIGH);
  pinMode (P_LEWY, INPUT);
  pinMode (P_PRAWY, INPUT);
  pinMode (P_ENTER, INPUT_PULLUP);
  lewy_ostatni_stan = digitalRead(P_LEWY);
}

static unsigned long czasOstatnio = 0;
void zliczanie_impulsow_czujnik_rozrzutnika()
{
  unsigned long czasTeraz = millis();
  if (czasTeraz - czasOstatnio < CZUJNIK_POMIAR_OPOZNIENIE)
    return;

  licznik_czujnik_obrotow_rozrzutnika++;
  czasOstatnio = czasTeraz;
}

int obroty_na_min = 0;
unsigned long t_przeliczania = 700;
unsigned long t_ostatnio = millis();
const int n_probki = 5;
uint8_t t_probki[n_probki] = {0};
uint8_t n_pozycja = 0;
void obroty_na_minute()
{
  int suma_obr = 0;
  if(millis() - t_ostatnio > t_przeliczania)
  {
    t_ostatnio = millis();
    t_probki[n_pozycja] = licznik_czujnik_obrotow_rozrzutnika;
    n_pozycja++;
    if (n_pozycja > n_probki)
    {
      n_pozycja = 0;
    }

    for (int i=0 ; i < n_probki ; i++)
    {
      suma_obr += t_probki[i];
    }
    obroty_na_min = (MINUTE/t_przeliczania)*suma_obr/n_probki;
    
    licznik_czujnik_obrotow_rozrzutnika = 0;
  }
}

int pokretlo_fix_counter = 0;
void pokretlo()
{
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
    licznik_pokretla++;
    pokretlo_fix_counter = 0;
    Serial.print("Licznik pokretla: ");
    Serial.println(licznik_pokretla);
  }
  else if (pokretlo_fix_counter == -2)
  {
    licznik_pokretla--;
    pokretlo_fix_counter = 0;
    Serial.print("Licznik pokretla: ");
    Serial.println(licznik_pokretla);
  }

  lewy_ostatni_stan = lewy_stan;
}

unsigned int t_enter = 0;
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
  if (licznik_pokretla_stary != licznik_pokretla || enter_krotki || enter_dlugi)
  {
    licznik_pokretla_stary = licznik_pokretla;
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

unsigned long t_alarm_odliczanie_2 = 0;
bool sygnal = true;
void alarm(int poziom = 0)
{
  if (obroty_na_min < poziom)
  {
    if (sygnal)
    {
      tone(ALARM, 1600);
    }
    else
    {
      noTone(ALARM);
    }

    if (millis() - t_alarm_odliczanie_2 > 140)
    {
      sygnal = !sygnal;
      t_alarm_odliczanie_2 = millis();
    }
  }
  else
  {
    noTone(ALARM);
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
  u8g2.setFont(u8g2_font_ncenB24_tr);
  u8g2.setCursor(35, 30);

  if (obroty > 999)
  {
    u8g2.setFont(u8g2_font_ncenB18_tr);
    u8g2.setCursor(35, 27);
  }

  u8g2.print(obroty);

  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(95,32,"RPM");
}

void oprint_temperatura_1()
{
  u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
  u8g2.drawGlyph(0, 32, 0x0046);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(40, 25);
  u8g2.print(1);
}

void oprint_temperatura_2()
{
  u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
  u8g2.drawGlyph(0, 32, 0x0046);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(40, 25);
  u8g2.print(2);
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

int ostatnie_obroty = 0;
int ostatnie_pokretlo = 0;
bool oled_odswiez = false;
void oled_manager()
{
  // enter_krotki
  // enter_dlugi
  // wartosc pokretla
  // ol_0 = 0;
  // ol_1 = 0;
  // ol_2 = 0;
  // ol_3 = 0;

  if (licznik_pokretla != ostatnie_pokretlo ||
      enter_krotki ||
      enter_dlugi ||
      oled_odswiez)
  {
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
          oprint_temperatura_1();
          u8g2.nextPage();
          break;
        }
        case 2:
        {
          u8g2.firstPage();
          oprint_temperatura_2();
          u8g2.nextPage();
          break;
        }
        case 3:
        {
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
      // ustawienia
    }
  }

  if (ol_0 == 0 && obroty_na_min != ostatnie_obroty)
  {
    u8g2.firstPage();
    oprint_rozrzutnik(obroty_na_min);
    u8g2.nextPage();
    ostatnie_obroty = obroty_na_min;
  }
}

void loop(void)
{
  obroty_na_minute();
  przycisk();
  pokretlo();
  dzwiek_przycisku();
  oled_manager();
  alarm(-1);
}
