#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 10
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// variable to hold device addresses
DeviceAddress Thermometer;
int deviceCount = 0;

struct Dsdev{
  int kolejnosc;
  DeviceAddress devaddr;
} devtemp[4];

void cpy_address(DeviceAddress a, DeviceAddress b)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {    
    a[i] = b[i];
  }
}

void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.println("Locating devices...");Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);Serial.println(" devices.");Serial.println("");

  Serial.println("Zapisywanie adresow i ustawianie domyslnej kolejnosci");
  for (int i = 0;  i < deviceCount;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" : ");
    sensors.getAddress(devtemp[i].devaddr, i);
    devtemp[i].kolejnosc = i;
    printAddress(devtemp[i].devaddr);
  }

  //cpy_address(devtemp[0].devaddr, devtemp[1].devaddr);
}

int zmiana_kolejnosci()
{
  int pozycja = 0;

  for(int i = 0;  i < deviceCount;  i++)
  {
    // wybor pozycja
    int pozycja= Serial.read();

    devtemp[i].kolejnosc = pozycja;
  }
}

void loop(void)
{

// test
  for(int i = 0;  i < deviceCount;  i++)
  {
    int j = 0;
    while(i != devtemp[j].kolejnosc && j < deviceCount) j++;
    printAddress(devtemp[j].devaddr);
  }

  zmiana_kolejnosci();
}
