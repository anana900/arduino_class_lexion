// ---------------------------------PORTY------------------------------------
// CZUJNIKI
uint8_t CZUJNIK_OBROTOW_ROZRZUTNIKA = 2;

// PRZYCISKI
uint8_t P_LEWY = 11;
uint8_t P_PRAWY = 12;
uint8_t P_ENTER = 13;

// ALARM
uint8_t ALARM = A0;
uint8_t ALARM_AKTYWACJA_ZEWNETRZNA = A1;

// OLED REZERWACJA I2C
// A4
// A5

// TEMPERATURA
uint8_t ONE_WIRE_CZUJNIK_TEMPERATURA = 4;

// ---------------------------------ZMIENNE------------------------------------
int licznik_pokretla = 0;
bool pokretlo_zmiana = false;

bool enter_krotki = false;
bool enter_dlugi = false;

const int EXT_ZRODLO_OPOZNIENIE = 4000;
const uint8_t CZUJNIK_POMIAR_OPOZNIENIE = 10;
const unsigned long MINUTE = 60000;

int obroty_na_min = 0;
bool obroty_alarm = false;
bool obroty_zmiana = false;

bool wyswietl_alarm = false;

bool temperatura_alarm = false;
bool temperatura_zmiana = true;
const int LICZBA_CZUJNIKOW_TEMPERATURY = 4;
String temperatura[2][LICZBA_CZUJNIKOW_TEMPERATURY] = {{"--", "--", "--", "--",},
                                                       {"0","0","0","0"}};

// ---------------------------------STRUKTURY------------------------------------
// Dane nieulotne
struct Ustawienia
{
  int global_limit_obroty_alarm = 200;
  int global_limit_temperatura_alarm = 90;
  bool global_czy_uzywac_alarmu = false;
  bool global_alarm_aktywacja_zewnetrzna = true;
  DeviceAddress ds1820_adresy[LICZBA_CZUJNIKOW_TEMPERATURY] = {{ 0x28, 0xF0, 0x3E, 0x45, 0x92, 0x12, 0x02, 0x8D },
                                                               { 0x28, 0xE4, 0xD2, 0x45, 0x92, 0x11, 0x02, 0xA4 },
                                                               { 0x28, 0x3E, 0xBC, 0x45, 0x92, 0x19, 0x02, 0x6A },
                                                               { 0x28, 0xFF, 0x1C, 0x91, 0xC2, 0x15, 0x01, 0x2A }};
  String temp_nazwy_czujnikow[LICZBA_CZUJNIKOW_TEMPERATURY] = {"a","b","c","d"};
} class_ustawienia;
