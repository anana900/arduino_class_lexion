
int lewy_stan;
int lewy_ostatni_stan;
int pokretlo_fix_counter = 0;
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

  if (pokretlo_fix_counter == 2)
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

class Przycisk
{
  private:
    uint8_t port_przycisku;
    uint8_t port_alarmu;
    uint8_t typ_przycisku;;

    unsigned long t_enter = 0;
    bool p_enter_przycisniety_krotko = false;
    bool p_enter_przycisniety_dlugo = false;

    void Przycisk::przycisk();

  public:
    Przycisk(uint8_t port, uint8_t alarm, uint8_t typ = 0) :
      port_przycisku(port),
      port_alarmu(alarm),
      typ_przycisku(typ)
    {}

    ~Przycisk()
    {}

    int licznik_pokretla = 0;
    bool pokretlo_zmiana = false;
    bool enter_krotki = false;
    bool enter_dlugi = false;

    void skanuj_przycisk();
};

void Przycisk::przycisk()
{
  if (!digitalRead(port_przycisku))
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
      tone(port_alarmu, 1600);
      delay(100);
      noTone(port_alarmu);
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

void Przycisk::skanuj_przycisk()
{
  switch (typ_przycisku)
  {
    case 0 :
      {
        // enter krotki, enter dlugi
        przycisk();
        break;
      }
    case 1 :
      {
        przycisk();
        if (enter_krotki || enter_dlugi)
        {
          enter_krotki = false;
          enter_dlugi = false;
          licznik_pokretla ++;
          pokretlo_zmiana = true;
        }
        break;
      }
    case 2 :
      {
        przycisk();
        if (enter_krotki || enter_dlugi)
        {
          enter_krotki = false;
          enter_dlugi = false;
          licznik_pokretla --;
          pokretlo_zmiana = true;
        }
        break;
      }
  }
}
