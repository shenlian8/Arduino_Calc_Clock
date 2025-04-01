// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <ThreeWire.h>  
#include <RtcDS1302.h>
//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <LiquidCrystal_I2C.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String line1;
String line2;

long randomNumber;

void setup () 
{
    randomSeed(analogRead(0));
    
    lcd.init();                      // initialize the lcd 
    // Print a message to the LCD.
    lcd.backlight();
    
    lcd.setCursor(0,0);
    lcd.print("Initializing...");
    lcd.setCursor(0,1);
    lcd.print("Please wait!");
    
    //Serial.begin(57600);

    // Serial.print("compiled: ");
    // Serial.print(__DATE__);
    // Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // printDateTime(compiled);
    // Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        //Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        // Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        // Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        // Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        // Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        // Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();

    // printDateTime(now);
    // Serial.println();

    // char timeString[16];
    // char datestring[16];

    
    /*snprintf_P(timeString, 
            countof(timeString),
            PSTR("%02u:%02u:%02u"),
            now.Hour(),
            now.Minute(),
            now.Second() );
    */
    /*
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u-%02u-%02u"),
            now.Year(),
            now.Month(),
            now.Day() );
    */
    if (!now.IsValid())
    {
      lcd.print("Time Error!");
    } else
    {
      if (now.Second() % 15 == 0)
      {
        randomNumber = random(2);
        if (randomNumber == 0)
        {
          set2NumberCalc(now.Hour(), line1);
          set3NumberCalc(now.Minute(), line2);          
        } else
        {
          set3NumberCalc(now.Hour(), line1);
          set2NumberCalc(now.Minute(), line2);
        }
        
        lcd.clear();

        lcd.setCursor(0,0);
        lcd.print(line1);

        lcd.setCursor(0,1);
        lcd.print(line2);
      }
    }
    //delay(10000); // ten seconds
}

void set3NumberCalc(const int targetNumber, const String& line)
{
  int totalFound = 0;
  int result;

  for (int i = 0; i <= 9; i ++) 
    for (int j = 0; j <= 9; j ++)
      for (int k = 0; k<= 9; k ++)
      {
        // + +
        result = i + j + k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // + -
        result = i + j - k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // + *
        result = i + j * k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // * +
        result = i * j + k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // * -
        result = i * j - k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // - +
        if (i >= j)
        {
          result = i - j + k;
          if (result == targetNumber)
          {
            totalFound ++;
          }
        }

        // - -
        if (i >= j)
        {
          result = i - j - k;
          if (result == targetNumber)
          {
            totalFound ++;
          }
        }

        // - *
        result = i - j * k;
        if (result == targetNumber)
        {
          totalFound ++;
        }
      }

  randomNumber = random(totalFound);

  /*
  Serial.println("Target:");
  Serial.println(targetNumber);
  Serial.println("Total found:");
  Serial.println(totalFound);
  Serial.println("Random:");
  Serial.println(randomTarget);
  */

  totalFound = 0;

  for (int i = 0; i <= 9; i ++) 
    for (int j = 0; j <= 9; j ++)
      for (int k = 0; k<= 9; k ++)
      {
        // + +
        result = i + j + k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j) + " + " + String(k);
            return;
          }
        }

        // + -
        result = i + j - k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j) + " - " + String(k);
            return;
          }
        }

        // + *
        result = i + j * k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j) + " x " + String(k);
            return;
          }
        }

        // * +
        result = i * j + k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " x " + String(j) + " + " + String(k);
            return;
          }
        }

        // * -
        result = i * j - k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " x " + String(j) + " - " + String(k);
            return;
          }
        }

        // - +
        if (i >= j)
        {
          result = i - j + k;
          if (result == targetNumber)
          {
            totalFound ++;
            if (totalFound > randomNumber)
            {
              line = String(i) + " - " + String(j) + " + " + String(k);
              return;
            }
          }
        }

        // - -
        if (i >= j)
        {
          result = i - j - k;
          if (result == targetNumber)
          {
            totalFound ++;
            if (totalFound > randomNumber)
            {
              line = String(i) + " - " + String(j) + " - " + String(k);
              return;
            }
          }
        }

        // - *
        result = i - j * k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " - " + String(j) + " x " + String(k);
            return;
          }
        }
      }
}

void set2NumberCalc(const int targetNumber, const String& line)
{
  int totalFound = 0;
  int result;

  for (int i = 0; i <= 58; i ++)
    for (int j = 0; j<= 58; j ++)
    {
      if (! (i >= 10 or j <= 10))
      {
        // +
        result = i + j;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // -
        result = i - j;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // *
        result = i * j;
        if (result == targetNumber)
        {
          totalFound ++;
        }
      }
    }

  randomNumber = random(totalFound);

  /*
  Serial.println("Target:");
  Serial.println(targetNumber);
  Serial.println("Total found:");
  Serial.println(totalFound);
  Serial.println("Random:");
  Serial.println(randomTarget);
  */

  totalFound = 0;

  for (int i = 0; i <= 68; i ++)
    for (int j = 0; j<= 68; j ++)
    {
      if (! (i >= 10 or j <= 10))
      {
        // +
        result = i + j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j);
            return;
          }
        }

        // + -
        result = i - j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " - " + String(j);
            return;
          }
        }

        // *
        result = i * j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " x " + String(j);
            return;
          }
        }
      }
    }
}
