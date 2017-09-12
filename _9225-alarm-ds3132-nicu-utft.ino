// original sketch from http://educ8s.tv/arduino-real-time-clock/
// Nicu FLORICA (niq_ro) from http://www.arduinotehniq.com 
// made some chenages for animate display with blinking signs
// eliminate library for clock, use just subroutines
// use 3 buttons for manual adjust the data & clock
// add alarm (now usee eeprom memory for store hour alarm)


#include <UTFT.h>
#include <Wire.h>
#include <SPI.h>
#include "EEPROM.h"


UTFT tft(QD220A,11,13,10,8,9); 
//cs 10
//rst 8
//rs 9
//sdi 11
//clk 13
//led +5v

extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFontPlusPlus[];

float maxTemperature=0;
float minTemperature=200;
char charMinTemperature[10];
char charMaxTemperature[10];
char timeChar[100];
char dateChar[50];
char temperatureChar[10];

float temperature = 0;
float previousTemperature = 0;

String dateString;
int minuteNow=0;
int minutePrevious=0;



#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
float temp3231;
float temperatura, temperatura0;
byte hh, mm, ss;
byte yy, ll, dd, zz;

#define meniu 2 
#define minus 3
#define plus 4
#define alarm 5
#define buzzer 6
int nivel = 0;   // if is 0 - clock
                 // if is 1 - hour adjust
                 // if is 2 - minute adjust

int hh1, mm1, ss1, zz1, dd1, ll1, yy1;   
int maxday;
boolean initial = 1;
char chartemp[3];
int hha, mma;   // for alarm
byte al;     // for alarm 
byte xa = 130;
byte ya = 120;

void setup () 
{
    tft.InitLCD();
    tft.setBackColor(0, 0, 0);
    tft.clrScr();
    Serial.begin(9600);
    Wire.begin();
pinMode(meniu, INPUT); 
pinMode(plus, INPUT); 
pinMode(minus, INPUT); 
pinMode(alarm, INPUT); 
pinMode(buzzer, OUTPUT);
digitalWrite(meniu, HIGH);  // put inputs in high state (when push is to ground -> low state)
digitalWrite(plus, HIGH);
digitalWrite(minus, HIGH);
digitalWrite(alarm, HIGH);
digitalWrite(buzzer, LOW);
tft.setColor(VGA_WHITE);tft.setFont(SmallFont);
    printText("Temperatura", 15,85);  // Temperature Static Text
   tft.setColor(VGA_RED);
    printText("MAX", 18,130);
    tft.setColor(VGA_BLUE);
    printText("MIN", 95,130);

// setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
//setDS3231time(0, 45, 13, 0, 25, 11, 2016;

//  hha = 7;
//  mma = 00;
//  al = 1;  // 0 = alarm is off (must put in 1 foar active)

hha = EEPROM.read(100);
mma = EEPROM.read(101);
al = EEPROM.read(102);

if ((hha < 0) || (hha > 23)) hha = 7;
if ((mma < 0) || (mma > 59)) mma = 0;
if ((al < 0) || (al > 1)) al = 0;
}


void loop () 
{

if (nivel == 0)
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  hh1=hh;
  mm1=mm;
  zz1=zz;
  yy1=yy;
  ll1=ll;
  dd1=dd;

  delay(500);
 
 tft.clrScr();
 
  }


  float temperature = get3231Temp();
  readDS3231time(&ss, &mm, &hh, &zz, &dd, &ll,&yy);

    String halarma = "";
    if(hha<10)
    {
         halarma = halarma+" "+String(hha);
    }else  
    halarma = halarma+ String(hha);
    if(mma<10)
    {
        halarma = halarma+":0"+String(mma);
    }else
    {
        halarma = halarma+":"+String(mma);
    }
    halarma.toCharArray(timeChar,100);

if (digitalRead(alarm) == LOW)
  {
  al = al + 1;
  EEPROM.write(102, al%2);
  delay(500);
  }
if (mma == mm & hha == hh & al%2)
{
//digitalWrite(buzzer, HIGH);
tone(buzzer, 1000, 200);
  delay(200);
  noTone(buzzer);
delay(200);

tft.setColor(VGA_AQUA);
   tft.drawCircle(xa, ya, 5); 
   tft.drawLine(xa, ya, xa, ya-5);
   tft.drawLine(xa, ya, xa+4, ya+4); tft.setFont(BigFont);
   printText(timeChar, xa+6,ya);
}
else
{
if (al%2 == 0)
{
   tft.setColor(VGA_OLIVE); tft.drawCircle(xa, ya, 5); 
    tft.drawLine(xa, ya, xa, ya-5 );
    tft.drawLine(xa, ya, xa+4, ya+4); 
  digitalWrite(buzzer, LOW);tft.setFont(BigFont);
    printText(timeChar, xa+6,ya);
}
if (al%2 == 1)
{
  tft.setColor(VGA_GRAY);  tft.drawCircle(xa, ya, 5); 
    tft.drawLine(xa, ya, xa, ya-5);
    tft.drawLine(xa, ya, xa+4, ya+4 ); 
  digitalWrite(buzzer, LOW);tft.setFont(BigFont);
    printText(timeChar, xa+6,ya);
}
}
 
  minuteNow = mm;
 
  if(minuteNow!=minutePrevious || initial)
  {
    initial = 0;
    dateString = getDayOfWeek(zz)+",";
    //dateString = dateString+String(dd)+"-"+String(ll);
    dateString = dateString+String(dd)+"-"+month(ll);
    dateString= dateString+"-"+ String(yy+2000); 
    minutePrevious = minuteNow;
   
    String hours = "";
    if(hh<10)
    {
         hours = hours+"0"+String(hh);
    }else  
    hours = hours+ String(hh);
    if(mm<10)
    {
        hours = hours+" 0"+String(mm);
    }else
    {
        hours = hours+" "+String(mm);
   
    }
   
   
    tft.setColor(VGA_YELLOW);
    hours.toCharArray(timeChar,100);
    
   tft.setColor(VGA_YELLOW);
 // tft.setFont(BigFont);
   tft.setFont(SevenSegNumFontPlusPlus);
 
    printText(timeChar,25,25);


    
   tft.setColor(VGA_BLACK); tft.fillRect(86,20,121,80);
    dateString.toCharArray(dateChar,50);
   tft.setFont(SmallFont);   tft.setColor(VGA_FUCHSIA);printText(dateChar, CENTER,5);
   tft.setColor(VGA_TEAL);tft.print("Alarm clock Arduino DS3231",CENTER,160);
    tft.setFont(SmallFont);   tft.setColor(VGA_TEAL);tft.print("ALARMA:",155,100);
    tft.drawRect(120,99,218,137);
  }
  
  if(temperature != previousTemperature)
  {
    previousTemperature = temperature;
    String temperatureString = String(temperature,1);
    temperatureString.toCharArray(temperatureChar,10);
  
   tft.setColor(VGA_WHITE);tft.setFont(BigFont);
    printText(temperatureChar, 10,105);
    printText("o", 80,100);
    printText("C", 95,105);
tft.setFont(SmallFont);
    if(temperature>maxTemperature)
    {
      maxTemperature = temperature;
    }
      dtostrf(maxTemperature,5, 1, charMaxTemperature); 
      
      tft.setColor(VGA_RED);
      printText(charMaxTemperature, 3,145);
      printText("o", 45,140);
      printText("C", 51,145);
   
    if(temperature<minTemperature)
    {
      minTemperature = temperature;
    }
      dtostrf(minTemperature,5, 1, charMinTemperature); 
      
  
      tft.setColor(VGA_BLUE);
      printText(charMinTemperature, 80,145);
      printText("o", 122,140);
      printText("C", 128,145);
    
  }

delay(500);

     tft.setColor(VGA_YELLOW); 
tft.fillCircle(106,40,4);tft.fillCircle(106,61,4);
delay(500);
   tft.setColor(VGA_BLACK);  tft.fillCircle(106,40,4);tft.fillCircle(106,61,4);
 
} // end usual case (clock)


if (nivel == 1)   // change hours
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
   delay(500);
  tft.clrScr();
 
  }    
 tft.setColor(VGA_AQUA);tft.setFont(SmallFont);
printText("SETTING", 10,18);  // Temperature Static Text
printText("HOUR:", 10,36);  // Temperature Static Text
dtostrf(hh1,3, 0, chartemp); 
     
      delay(50);
      printText(chartemp,60,50);
      delay(50);
      
if (digitalRead(plus) == LOW)
    {
    hh1 = hh1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    hh1 = hh1-1;
    delay(150);
    }    
if (hh1 > 23) hh1 = 0;
if (hh1 < 0) hh1 = 23;       
     
}  // end loop nivel = 1 (change the hours)

if (nivel == 2)   // change minutes
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500);
 
  }    

printText("SETTING", 10,18);  // Temperature Static Text

printText("MINUTE:", 10,35);  // Temperature Static Text
Serial.println(mm1);
dtostrf(mm1,3, 0, chartemp); 
    //  tft.fillRect(50,50,70,18);
      delay(50);
      printText(chartemp, 60,50);
      delay(50);
     
if (digitalRead(plus) == LOW)
    {
    mm1 = mm1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    mm1 = mm1-1;
    delay(150);
    }    
if (mm1 > 59) mm1 = 0;
if (mm1 < 0) mm1 = 59;       
     
}  // end loop nivel = 2 (change the minutes)

if (nivel == 3)   // change day in week
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500); tft.clrScr();
  tft.setBackColor(0, 0, 0);
 
  }    

printText("SETTING",10,18);  // Temperature Static Text

printText("Day in Week:", 10,35);  // Temperature Static Text

dtostrf(zz1,3, 0, chartemp); 
     
      delay(50);
      printText(chartemp, 60,50);
//tft.fillRect(10,80,100,18);      
if (zz1 == 1) printText("1 - Marti   ",10,80);  // Temperature Static Text
if (zz1 == 2) printText("2 - Miercuri", 10,80);  // Temperature Static Text
if (zz1 == 3) printText("3 - Joi     ", 10,80);  // Temperature Static Text
if (zz1 == 4) printText("4 - Vineri  ", 10,80);  // Temperature Static Text
if (zz1 == 5) printText("5 - Simbat  ", 10,80);  // Temperature Static Text
if (zz1 == 6) printText("6 - Duminica",10,80);  // Temperature Static Text
if (zz1 == 0) printText("0 - Luni    ", 10,80);  // Temperature Static Text  
    delay(50);
     
if (digitalRead(plus) == LOW)
    {
    zz1 = zz1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    zz1 = zz1-1;
    delay(150);
    }    
if (zz1 > 6) zz1 = 0;
if (zz1 < 0) zz1 = 6;       
     
}  // end loop nivel = 3 (change the day of the week)

if (nivel == 4)   // change year
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500); tft.clrScr();
  tft.setBackColor(0, 0, 0);
   
  }    

printText("SETTING", 10,18);  // Temperature Static Text

printText("                    ", 10,80); 
printText("YEAR:         ", 10,35);  // Temperature Static Text
dtostrf(yy1,3, 0, chartemp); 
    
      delay(50);
      printText("20", 45,50);
      printText(chartemp, 60,50);
      delay(50);

     
if (digitalRead(plus) == LOW)
    {
    yy1 = yy1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    yy1 = yy1-1;
    delay(150);
    }    
if (yy1 > 49) yy1 = 25;
if (yy1 < 16) yy1 = 16;       
     
}  // end loop stare = 4 (change the year)


if (nivel == 5)   // change mounth
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500); tft.clrScr();
  tft.setBackColor(0, 0, 0);
   
  }    
printText("SETTING", 10,18);  // Temperature Static Text

printText("MONTH:",10,35);  // Temperature Static Text
dtostrf(ll1,3, 0, chartemp); 
      
      delay(50);
      printText("  ", 45,50);
      printText(chartemp, 60,50);
      delay(50);
     
if (digitalRead(plus) == LOW)
    {
    ll1 = ll1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    ll1 = ll1-1;
    delay(150);
    }    
   
if (ll1 > 12) ll1 = 1;
if (ll1 < 1) ll1 = 12;       
     
}  // end loop stare = 5 (change the day as data)

if (nivel == 6)   // change day as data
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500); tft.clrScr();
  tft.setBackColor(0, 0, 0);
     
  }    
 
printText("SETTING", 10,18);  // Temperature Static Text

printText("DAY:    ", 10,35);  // Temperature Static Text
dtostrf(dd1,3, 0, chartemp); 
     
      delay(50);
      printText(chartemp,60,50);
      delay(50);
 
if (digitalRead(plus) == LOW)
    {
    dd1 = dd1+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    dd1 = dd1-1;
    delay(150);
    }    
    
 // Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
 if (ll == 4 || ll == 5 || ll == 9 || ll == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (ll ==2 && yy % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (ll ==2 && ll % 4 !=0) {
    maxday = 28;
  }

if (dd1 > maxday) dd1 = 1;
if (dd1 < 1) dd1 = maxday;       
     
}  // end loop nivel = 6 (change the day as data)

if (nivel == 7)   // change hours alarm
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500); tft.clrScr();
  
  }    

printText("ALARM", 10,18);  // Temperature Static Text
printText("HOUR:",10,36);  // Temperature Static Text
dtostrf(hha,3, 0, chartemp); 
      
      delay(50);//tft.setColor(VGA_WHITE);
      printText(chartemp, 60,50);
      delay(50);
      
if (digitalRead(plus) == LOW)
    {
    hha = hha+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    hha = hha-1;
    delay(150);
    }    
if (hha > 23) hha = 0;
if (hha < 0) hha = 23;       
     
}  // end loop nivel = 7 (change the hours)

if (nivel == 8)   // change minutes for alarm
{
if (digitalRead(meniu) == LOW)
  {
  nivel = nivel+1;
  delay(500); tft.clrScr();
 
  }    

printText("ALARM",10,18);  // Temperature Static Text

printText("MINUTE:",10,35);  // Temperature Static Text
Serial.println(mm1);
dtostrf(mma,3, 0, chartemp); 
    
      delay(50);
      printText(chartemp,60,50);
      delay(50);
     
if (digitalRead(plus) == LOW)
    {
    mma = mma+1;
    delay(150);
    }
if (digitalRead(minus) == LOW)
    {
    mma = mma-1;
    delay(150);
    }    
if (mma > 59) mma = 0;
if (mma < 0) mma = 59;       
     
}  // end loop nivel = 8 (change the minutes)


 if (nivel >=9)  // readfy to return to main loop
 {
  tft.clrScr();
  setDS3231time(0, mm1, hh1, zz1, dd1, ll1, yy1);
  EEPROM.write(100, hha);
  EEPROM.write(101, mma);
  EEPROM.write(102, al%2);
  
  nivel = 0;  
  previousTemperature= 0.0;
  initial = 1;
   tft.setColor(VGA_LIME);  printText("Temperatura",15,85);  // Temperature Static Text
    printText("MAX",18,130);
    printText("MIN", 95,130);
 }
}  // end main loop


void printText(char *text,  int x, int y)
{

  tft.print(text,x,y);
}

String getDayOfWeek(int i)
{
  switch(i)
  {
    case 0: return "Luni";break;
    case 1: return "Marti";break;
    case 2: return "Miercuri";break;
    case 3: return "Joi";break;
    case 4: return "Vineri";break;
    case 5: return "Simbata";break;
    case 6: return "Duminica";break;
    default: return "***";break;
  }
}

String month(int i)
{
  switch(ll)
  {
    //case 0: return "Luni";break;
    case 1: return "Ianuarie";break;
    case 2: return "Februarie";break;
    case 3: return "Martie";break;
    case 4: return "Aprilie";break;
    case 5: return "Mai";break;
    case 6: return "Iunie";break;
    case 7: return "Iulie";break;
    case 8: return "August";break;
    case 9: return "Septembrie";break;
    case 10: return "Octombrie";break;
    case 11: return "Noiembrie";break;
    case 12: return "Decembrie";break;
    default: return "***";break;
  }
}

float get3231Temp() // from http://www.goodliffe.org.uk/arduino/rtc_ds3231.php
{
  //temp registers (11h-12h) get updated automatically every 64s
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
 
  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion
   
    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }
   
  return temp3231;
}

// http://forum.arduino.cc/index.php?topic=398891.0
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(month)); // set month
    Wire.write(decToBcd(year)); // set year (0 to 99)
    Wire.endTransmission();
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
    return ( (val / 16 * 10) + (val % 16) );
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
    return ( (val / 10 * 16) + (val % 10) );
}

void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

