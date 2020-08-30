
uint8_t ol_0 = 0;
uint8_t ol_1 = 0;
uint8_t ol_2 = 0;
uint8_t ol_3 = 0;

unsigned long t_liczydlo = 0;
void ustawienie_limitu_obrotow(bool kierunek)
{
  if(class_ustawienia.global_limit_obroty_alarm < 10000 && kierunek)
  {
    if(millis() - t_liczydlo < 150 && class_ustawienia.global_limit_obroty_alarm <= 9990)
    {
      class_ustawienia.global_limit_obroty_alarm += 10;
    }
    else
    {
      class_ustawienia.global_limit_obroty_alarm++;
    }
    t_liczydlo = millis();
  }
  else if (class_ustawienia.global_limit_obroty_alarm > 0 && !kierunek)
  {
    if(millis() - t_liczydlo < 150 && class_ustawienia.global_limit_obroty_alarm >= 10)
    {
      class_ustawienia.global_limit_obroty_alarm -= 10;
    }
    else
    {
      class_ustawienia.global_limit_obroty_alarm--;
    }
    t_liczydlo = millis();
  }
}

void ustawienie_limitu_temperatury(bool kierunek)
{
  if(class_ustawienia.global_limit_temperatura_alarm < 125 && kierunek)
  {
    if(millis() - t_liczydlo < 150 && class_ustawienia.global_limit_temperatura_alarm <= 115)
    {
      class_ustawienia.global_limit_temperatura_alarm += 10;
    }
    else
    {
      class_ustawienia.global_limit_temperatura_alarm++;
    }
    t_liczydlo = millis();
  }
  else if (class_ustawienia.global_limit_temperatura_alarm > -55 && !kierunek)
  {
    if(millis() - t_liczydlo < 150 && class_ustawienia.global_limit_temperatura_alarm >= -45)
    {
      class_ustawienia.global_limit_temperatura_alarm -= 10;
    }
    else
    {
      class_ustawienia.global_limit_temperatura_alarm--;
    }
    t_liczydlo = millis();
  }
}

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
  u8g2.drawStr(95,16,"RPM");
}

void oprint_temp_wybrana(int temp_index)
{
  switch(temp_index)
  {
    case 0:
    {
      u8g2.setFont(u8g2_font_helvR24_tr);
      u8g2.drawStr(0,30,"T");
      u8g2.setFont(u8g2_font_helvR18_tr);
      u8g2.drawStr(15,30,"a");break;
    }
    case 1:
    {
      u8g2.setFont(u8g2_font_helvR24_tr);
      u8g2.drawStr(0,30,"T");
      u8g2.setFont(u8g2_font_helvR18_tr);
      u8g2.drawStr(15,30,"b");break;
    }
    case 2:
    {
      u8g2.setFont(u8g2_font_helvR24_tr);
      u8g2.drawStr(0,30,"T");
      u8g2.setFont(u8g2_font_helvR18_tr);
      u8g2.drawStr(15,30,"c");break;
    }
    case 3:
    {
      u8g2.setFont(u8g2_font_helvR24_tr);
      u8g2.drawStr(0,30,"T");
      u8g2.setFont(u8g2_font_helvR18_tr);
      u8g2.drawStr(15,30,"d");break;
    }
  }
  
  u8g2.setFont(u8g2_font_helvR24_tr);
  u8g2.setCursor(35, 30);
  u8g2.print(temperatura[0][temp_index]);

  u8g2.setFont(u8g2_font_helvR18_tr);
  u8g2.drawStr(102,24,"'c");
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
  u8g2.print(temperatura[0][0]);

  u8g2.setCursor(3, 32);
  u8g2.print("Tc");
  u8g2.setCursor(31, 32);
  u8g2.print(temperatura[0][2]);

  u8g2.setCursor(67, 15);
  u8g2.print("Tb");
  u8g2.setCursor(95, 15);
  u8g2.print(temperatura[0][1]);

  u8g2.setCursor(67, 32);
  u8g2.print("Td");
  u8g2.setCursor(95, 32);
  u8g2.print(temperatura[0][3]);
}

void oprint_temp_i_rozrzutnik(int obroty)
{
  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.setCursor(0, 16);
  u8g2.print("Ta");
  u8g2.setCursor(0, 32);
  u8g2.print(temperatura[0][0]);

  u8g2.setCursor(24, 16);
  u8g2.print("Tb");
  u8g2.setCursor(24, 32);
  u8g2.print(temperatura[0][1]);

  u8g2.setCursor(48, 16);
  u8g2.print("Tc");
  u8g2.setCursor(48, 32);
  u8g2.print(temperatura[0][2]);

  u8g2.setCursor(72, 16);
  u8g2.print("Td");
  u8g2.setCursor(72, 32);
  u8g2.print(temperatura[0][3]);

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

  //u8g2.setFont(u8g2_font_open_iconic_other_2x_t);
  u8g2.setFont(u8g2_font_helvR10_tr);
  if (class_ustawienia.global_alarm_aktywacja_zewnetrzna)
  {
    // ext on
    u8g2.setCursor(36, 21);
    u8g2.print("Ext");
    //u8g2.drawGlyph(40, 25, 0x0042);
  }
  else
  {
    // ext off
    //u8g2.drawGlyph(40, 25, 0x0042);
    u8g2.setCursor(36, 21);
    u8g2.print("Ext");
    u8g2.drawLine(36, 27, 58, 5);
    u8g2.drawLine(37, 27, 59, 5);
    u8g2.drawLine(38, 27, 60, 5);
  }

  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(104, 25, 0x0044);
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

void oprint_temperatura_opcje(int limit)
{
  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.setCursor(3, 14);
  u8g2.print("Alarm");
  u8g2.setCursor(3, 27);
  u8g2.print("Temp");

  u8g2.setFont(u8g2_font_helvR18_tr);
  u8g2.setCursor(50, 25);
  u8g2.print(limit);
  
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.drawGlyph(104, 25, 0x0044);
}

bool naprzemiennie = false;
void oprint_alarm()
{
  if (naprzemiennie)
  {
    u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
    u8g2.drawGlyph(0, 16, 0x0047);
    u8g2.setFont(u8g2_font_helvR10_tr);
    u8g2.setCursor(16, 14);
    u8g2.print("ALARM");
  }
  naprzemiennie = !naprzemiennie;

  u8g2.setFont(u8g2_font_helvR10_tr);
  u8g2.setCursor(2, 30);

  if(temperatura_alarm)
  {
    u8g2.print("Temp");
    
    u8g2.setFont(u8g2_font_helvR10_tr);
    for (int adres_index = 0 ; adres_index < LICZBA_CZUJNIKOW_TEMPERATURY ; ++adres_index )
    {
      if (temperatura[1][adres_index] == "1")
      {
        if(adres_index < 2)
        {
          u8g2.setCursor(70, (adres_index+1)*14);
        }
        else
        {
          u8g2.setCursor(105, (adres_index-1)*14);
        }
        u8g2.print(temperatura[0][adres_index]);
      }
    }
  }
  else if(obroty_alarm)
  {
    u8g2.print("RPM");
    
    u8g2.setFont(u8g2_font_helvR24_tr);
    u8g2.setCursor(75, 32);
    if (obroty_na_min > 999)
    {
      u8g2.setFont(u8g2_font_helvR18_tr);
      u8g2.setCursor(75, 30);
    }
    u8g2.print(obroty_na_min);
  }
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
    
    if (ol_0 < 7 )
    {
      if (licznik_pokretla != ostatnie_pokretlo)
      {
        if (ostatnie_pokretlo < licznik_pokretla)
        {
          if (ol_0 < 6)
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
            ol_0 = 6;
          }
        }

        ostatnie_pokretlo = licznik_pokretla;
      }
      else if (enter_dlugi)
      {
        ol_0 = 7;
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
        case 2:
        case 3:
        case 4:
        {
          u8g2.firstPage();
          oprint_temp_wybrana(ol_0 - 1);
          u8g2.nextPage();
          break;
        }
        case 5:
        {
          u8g2.firstPage();
          oprint_temperatura();
          u8g2.nextPage();
          break;
        }
        case 6:
        {
          u8g2.firstPage();
          oprint_temp_i_rozrzutnik(obroty_na_min);
          u8g2.nextPage();
          break;
        }
        case 7:
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
    else if (ol_0 == 7)
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
            ol_0 = 8;
            ol_1 = 0;
            ol_2 = 0;
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
            ol_0 = 9;
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
          if (enter_krotki)
          {
            ol_0 = 10;
            ol_1 = 0;
            ol_2 = 0;
            oled_odswiez = true;
          }
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
    else if (ol_0 == 8)
    {
      // ustawienia ogolne

      if (licznik_pokretla != ostatnie_pokretlo)
      {
        if (ostatnie_pokretlo < licznik_pokretla)
        {
          if (ol_1 < 2)
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
            ol_1 = 2;
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
          oprint_ramka(32,32);
          oprint_menu_ustawienia_ogolne();
          u8g2.nextPage();
          if (enter_krotki)
          {
            class_ustawienia.global_alarm_aktywacja_zewnetrzna = !class_ustawienia.global_alarm_aktywacja_zewnetrzna;
            oled_odswiez = true;
          }
          break;
        }
        case 2:
        {
          u8g2.firstPage();
          oprint_ramka(96,32);
          oprint_menu_ustawienia_ogolne();
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 7;
            ol_1 = 3;
            ol_2 = 0;
            oled_odswiez = true;

            // zapis do pamieci
            zapisz_konfiguracje(class_ustawienia);
          }
          break;
        }
      }
    }
    else if (ol_0 == 9)
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
          oprint_rozrzutnik_opcje(class_ustawienia.global_limit_obroty_alarm);
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_2 = ( ol_2 + 3 ) % 2;
            oled_odswiez = true;
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
          oprint_rozrzutnik_opcje(class_ustawienia.global_limit_obroty_alarm);
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 7;
            ol_1 = 3;
            ol_2 = 0;
            oled_odswiez = true;

            // zapisanie do pamieci
            zapisz_konfiguracje(class_ustawienia);
          }
          break;
        }
      }
    }
    else if (ol_0 == 10)
    {
      // ustawienia temperatury

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
            ustawienie_limitu_temperatury(true);
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
            ustawienie_limitu_temperatury(false);
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
          oprint_temperatura_opcje(class_ustawienia.global_limit_temperatura_alarm);
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_2 = ( ol_2 + 3 ) % 2;
            oled_odswiez = true;
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
          oprint_temperatura_opcje(class_ustawienia.global_limit_temperatura_alarm);
          u8g2.nextPage();
          if (enter_krotki)
          {
            ol_0 = 7;
            ol_1 = 3;
            ol_2 = 0;
            oled_odswiez = true;

            // zapisanie do pamieci
            zapisz_konfiguracje(class_ustawienia);
          }
          break;
        }
      }
    }
  }

  if (ol_0 == 0 && obroty_zmiana)
  {
    u8g2.firstPage();
    oprint_rozrzutnik(obroty_na_min);
    u8g2.nextPage();
  }

  if (ol_0 == 5 && temperatura_zmiana)
  {
    u8g2.firstPage();
    oprint_temperatura();
    u8g2.nextPage();
  }

  if (ol_0 == 6 && (obroty_zmiana || temperatura_zmiana))
  {
    u8g2.firstPage();
    oprint_temp_i_rozrzutnik(obroty_na_min);
    u8g2.nextPage();
  }

  if ((ol_0 >= 1 && ol_0 <= 4) &&  temperatura_zmiana)
  {
    u8g2.firstPage();
    oprint_temp_wybrana(ol_0 - 1);
    u8g2.nextPage();
  }

  if(ol_0 != 100 && wyswietl_alarm)
  {
    /*
     * Wlaczenie komunikatu alarmowego
     */
    if (ol_0 != 100)
    {
      ol_3 = ol_0;
      ol_0 = 100; 
    }
    u8g2.firstPage();
    oprint_alarm();
    u8g2.nextPage();
  }
  else if(ol_0 == 100 && (obroty_zmiana || temperatura_zmiana))
  {
    /*
     * Aktualizacja komunikatu alarmowego
     */
    u8g2.firstPage();
    oprint_alarm();
    u8g2.nextPage();
  }
  else if (ol_0 == 100 && !wyswietl_alarm)
  {
    /*
     * Wylaczenie komunikatu alarmowego
     */
    ol_0 = ol_3; 
    oled_odswiez = true;
  }
}
