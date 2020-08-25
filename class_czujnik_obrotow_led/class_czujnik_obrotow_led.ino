
// CZUJNIKI
uint8_t CZUJNIK_OBROTOW_ROZRZUTNIKA = 2;

// RGB
uint8_t LED_R = 13;
uint8_t LED_G = 12;
uint8_t LED_B = 11;
uint8_t LED_OUT_OBROTOW_ROZRZUTNIKA = 10;

// ALARM
uint8_t ALARM = A0;

volatile word licznik_czujnik_obrotow_rozrzutnika = 0;
const int CZUJNIK_POMIAR_OPOZNIENIE = 10;
const unsigned int MINUTE = 60000;

void setup()
{
  Serial.begin(9600);

  pinMode(CZUJNIK_OBROTOW_ROZRZUTNIKA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CZUJNIK_OBROTOW_ROZRZUTNIKA), zliczanie_impulsow_czujnik_rozrzutnika, FALLING);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_OUT_OBROTOW_ROZRZUTNIKA, OUTPUT);
  
  digitalWrite(LED_R, 0);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_B, 0);
  digitalWrite(LED_OUT_OBROTOW_ROZRZUTNIKA, 0);
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
unsigned long odliczanie = millis();
int obroty_na_minute()
{
  if(millis() - odliczanie > 1000)
  {
    obroty_na_min = licznik_czujnik_obrotow_rozrzutnika * 60;
    licznik_czujnik_obrotow_rozrzutnika = 0;
    odliczanie = millis();
    return 1;
  }
  return 0;
}

unsigned long t_przeliczania = 1000;
unsigned long t_ostatnio = millis();
const int n_probki = 5;
int t_probki[n_probki] = {0};
int n_pozycja = 0;
void obroty_na_minute2()
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

unsigned long alarm_odliczanie = millis();
bool alarm_switch = true;
void alarm_led_dzwiek(int typ)
{
  /*
   * 0 - brak
   * 1 - zwolnienie
   * 2 - zatrzymanie
   */
   if (typ > 0)
   {
    if (millis() - alarm_odliczanie > 200/typ)
    {
      if (alarm_switch)
      {
        tone(ALARM, 200*typ);
        led_kontroler(2%typ,1,1%typ);
        alarm_switch = false;
      }
      else
      {
        noTone(ALARM);
       // analogWrite(ALARM, 0);
        led_kontroler(1,1,1);
        alarm_switch = true;
      }
      alarm_odliczanie = millis();
    }
   }
   else
   {
     noTone(ALARM);
    // analogWrite(ALARM, 0);
     led_kontroler(1,0,1);
   }
}

void led_kontroler(int r, int g, int b)
{
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}

void loop()
{
   if ( 10 <= obroty_na_min && obroty_na_min < 180 )
   {
     alarm_led_dzwiek(1);
     Serial.print("ALARM: ");
   }
   else if (obroty_na_min < 10)
   {
     alarm_led_dzwiek(2);
     Serial.print("ERROR: ");
   }
   else
   {
     alarm_led_dzwiek(0);
     Serial.print("OK: ");
   }
   Serial.print(obroty_na_min);Serial.println(" obr/min");
   obroty_na_minute2();
}
