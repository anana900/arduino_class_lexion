
volatile unsigned long t_start = 0;
volatile unsigned long t_end = 0;
volatile unsigned long t_diff = 0;
void isr_zliczanie_impulsow_czujnik_rozrzutnika()
{
/*
 * Wyznaczanie RPM na podstawie roznicy czasu odczytywanego
 * z pojedynczego czujnika umieszczonego na elemencie rotujacym.
 */
 
  t_start = millis();
  if (t_start - t_end < CZUJNIK_POMIAR_OPOZNIENIE)
  {
    // eliminacja drgan czujnika
    return;
  }

  t_diff = t_start - t_end;
  t_end = t_start;
}

unsigned long t_obroty_odswiezanie = 0;
const int t_obroty_odswiezanie_interwal = 1000;
bool stabilizacja_po_uruchomieniu = true;
void obroty_na_minute()
{
  // Jako improvement mozna dodac usrednianie wynikow przed wyswietleniem
  // wyswietlenie jest co ok 500ms
  
  obroty_zmiana = false;

  if (stabilizacja_po_uruchomieniu)
  {
    stabilizacja_po_uruchomieniu = false;
    t_diff = 0;
  }

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

    if (obroty_na_min < class_ustawienia.global_limit_obroty_alarm)
    {
      obroty_alarm = true;
    }
    else
    {
      obroty_alarm = false;
    }
  }
}
