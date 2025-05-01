#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
#include <LEDMatrixDriver.hpp>

// ###########
// init code lce

// DIN is connected to 11
// CLK is connected to 13
// CS is connected to 9

const uint8_t LEDMATRIX_CS_PIN = 9;

// Number of 8x8 segments you are connecting
const int LEDMATRIX_SEGMENTS = 10;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

// Marquee text
// char text[] = "** LED MATRIX DEMO! ** (1234567890) ++ \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\" ++ <$%/=?'.@,> --";
String text = "CLOCKINIT.";

// Only un-comment one of the lines below
#define L2R		//Left to right scroll		
//#define R2L		//Right to left scroll

int x=0, y=0;   // start top left

const byte interruptPin = 2;

// ###########

RtcDS3231<TwoWire> Rtc(Wire);

String line1;
String line2;

String oldLine1 = "";
String oldLine2 = "";

long randomNumber;

bool showReadable = true;
bool oldShowReadable = ! showReadable;

int birthMonth = 10;
int birthDay = 26;

int showHappy = 0;

RtcDateTime lastUpdate;

// handy routine to return true if there was an error
// but it will also print out an error message with the given topic
bool wasError(const char* errorTopic = "")
{
    uint8_t error = Rtc.LastError();
    if (error != 0)
    {
        // we have a communications error
        // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
        // for what the number means
        // Serial.print("[");
        // Serial.print(errorTopic);
        // Serial.print("] WIRE communications error (");
        // Serial.print(error);
        // Serial.print(") : ");

        switch (error)
        {
        case Rtc_Wire_Error_None:
            // Serial.println("(none?!)");
            break;
        case Rtc_Wire_Error_TxBufferOverflow:
            // Serial.println("transmit buffer overflow");
            break;
        case Rtc_Wire_Error_NoAddressableDevice:
            // Serial.println("no device responded");
            break;
        case Rtc_Wire_Error_UnsupportedRequest:
            // Serial.println("device doesn't support request");
            break;
        case Rtc_Wire_Error_Unspecific:
            // Serial.println("unspecified error");
            break;
        case Rtc_Wire_Error_CommunicationTimeout:
            // Serial.println("communications timed out");
            break;
        }
        return true;
    }
    return false;
}

void changeDisplay ()
{
  Serial.println("Button pressed");
  showReadable = ! showReadable;
  showHappy ++;
}

void setup () 
{
    // init the display
    lmd.setEnabled(true);
    lmd.setIntensity(5);   // 0 = low, 10 = high

    showText();

    // initialize random seed
    randomSeed(analogRead(0));
    
    // initialize sierial
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(interruptPin), changeDisplay, FALLING);

    Rtc.Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // printDateTime(compiled);
    // Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        if (!wasError("setup IsDateTimeValid"))
        {
            // Common Causes:
            //    1) first time you ran and the device wasn't running yet
            //    2) the battery on the device is low or even missing

            //Serial.println("RTC lost confidence in the DateTime!");

            // following line sets the RTC to the date & time this sketch was compiled
            // it will also reset the valid flag internally unless the Rtc device is
            // having an issue

            Rtc.SetDateTime(compiled);
        }
    }

    if (!Rtc.GetIsRunning())
    {
        if (!wasError("setup GetIsRunning"))
        {
            // Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("setup GetDateTime"))
    {
        if (now < compiled)
        {
            // Serial.println("RTC is older than compile time, updating DateTime");
            Rtc.SetDateTime(compiled);
        }
        else if (now > compiled)
        {
            // Serial.println("RTC is newer than compile time, this is expected");
        }
        else if (now == compiled)
        {
            // Serial.println("RTC is the same as compile time, while not expected all is still fine");
        }
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
    wasError("setup SetSquareWavePin");
}

void loop () 
{
    updateTime();
    delay(500);
}

void updateTime()
{
  if (!Rtc.IsDateTimeValid()) 
  {
      if (!wasError("loop IsDateTimeValid"))
      {
          // Common Causes:
          //    1) the battery on the device is low or even missing and the power line was disconnected
          // Serial.println("RTC lost confidence in the DateTime!");
          text = "RTC error!";

          showText();
      }
  } else 
  {
    RtcDateTime now = Rtc.GetDateTime();

    if (now != lastUpdate) 
    {
      lastUpdate = now;

      if (showReadable)
      {
        if ((now.Second() % 15 == 0) || (showReadable != oldShowReadable))
        {
          oldShowReadable = showReadable;
          setNormalTime(now);
        }
      } else
      {
        if ((now.Second() % 15 == 0) || (showReadable != oldShowReadable))
        {
          oldShowReadable = showReadable;
          setTimeCalc(now);
        }
      }

      if (line1.length() == 4)
      {
        line1 = " " + line1;
      }

      if (line2.length() == 4)
      {
        line2 = " " + line2;
      }
          
      if((now.Month() == birthMonth) && (now.Day() == birthDay) && (showHappy % 10 < 5))
      {
        line1 = "HAPPY";
        line2 = String(now.Year() - 2012) +" J.";
      }

      if ((line1 != oldLine1) || (line2 != oldLine2)) 
      {
          oldLine1 = line1;
          oldLine2 = line2;

          Serial.println(line1);
          Serial.println(line2);

          text = line2 + line1;

          showText();
      }
    }
  }
}

// This is the font definition. You can use http://gurgleapps.com/tools/matrix to create your own font or sprites.
// If you like the font feel free to use it. I created it myself and donate it to the public domain.
byte font[95][8] = { {0,0,0,0,0,0,0,0}, // SPACE
                     {0x10,0x18,0x18,0x18,0x18,0x00,0x18,0x18}, // EXCL
                     {0x28,0x28,0x08,0x00,0x00,0x00,0x00,0x00}, // QUOT
                     {0x00,0x0a,0x7f,0x14,0x28,0xfe,0x50,0x00}, // #
                     {0x10,0x38,0x54,0x70,0x1c,0x54,0x38,0x10}, // $
                     {0x00,0x60,0x66,0x08,0x10,0x66,0x06,0x00}, // %
                     {0,0,0,0,0,0,0,0}, // &
                     {0x00,0x10,0x18,0x18,0x08,0x00,0x00,0x00}, // '
                     {0x02,0x04,0x08,0x08,0x08,0x08,0x08,0x04}, // (
                     {0x40,0x20,0x10,0x10,0x10,0x10,0x10,0x20}, // )
                     {0x00,0x10,0x54,0x38,0x10,0x38,0x54,0x10}, // *
                     {0x00,0x08,0x08,0x08,0x7f,0x08,0x08,0x08}, // +
                     {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x08}, // COMMA
                     {0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00}, // -
                     {0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06}, // DOT
                     {0x00,0x04,0x04,0x08,0x10,0x20,0x40,0x40}, // /
                     {0x00,0x38,0x44,0x4c,0x54,0x64,0x44,0x38}, // 0
                     {0x00,0x04,0x0c,0x14,0x04,0x04,0x04,0x04}, // 1
                     {0x00,0x30,0x48,0x04,0x04,0x38,0x40,0x7c}, // 2
                     {0x00,0x38,0x04,0x04,0x18,0x04,0x44,0x38}, // 3
                     {0x00,0x04,0x0c,0x14,0x24,0x7e,0x04,0x04}, // 4
                     {0x00,0x7c,0x40,0x40,0x78,0x04,0x04,0x38}, // 5
                     {0x00,0x38,0x40,0x40,0x78,0x44,0x44,0x38}, // 6
                     {0x00,0x7c,0x04,0x04,0x08,0x08,0x10,0x10}, // 7
                     {0x00,0x3c,0x44,0x44,0x38,0x44,0x44,0x78}, // 8
                     {0x00,0x38,0x44,0x44,0x3c,0x04,0x04,0x78}, // 9
                     {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // :
                     {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x08}, // ;
                     {0x00,0x10,0x20,0x40,0x80,0x40,0x20,0x10}, // <
                     {0x00,0x00,0x7e,0x00,0x00,0xfc,0x00,0x00}, // =
                     {0x00,0x08,0x04,0x02,0x01,0x02,0x04,0x08}, // >
                     {0x00,0x38,0x44,0x04,0x08,0x10,0x00,0x10}, // ?
                     {0x00,0x30,0x48,0xba,0xba,0x84,0x78,0x00}, // @
                     {0x00,0x1c,0x22,0x42,0x42,0x7e,0x42,0x42}, // A
                     {0x00,0x78,0x44,0x44,0x78,0x44,0x44,0x7c}, // B
                     {0x00,0x3c,0x44,0x40,0x40,0x40,0x44,0x7c}, // C
                     {0x00,0x7c,0x42,0x42,0x42,0x42,0x44,0x78}, // D
                     {0x00,0x78,0x40,0x40,0x70,0x40,0x40,0x7c}, // E
                     {0x00,0x7c,0x40,0x40,0x78,0x40,0x40,0x40}, // F
                     {0x00,0x3c,0x40,0x40,0x5c,0x44,0x44,0x78}, // G
                     {0x00,0x42,0x42,0x42,0x7e,0x42,0x42,0x42}, // H
                     {0x00,0x7c,0x10,0x10,0x10,0x10,0x10,0x7e}, // I
                     {0x00,0x7e,0x02,0x02,0x02,0x02,0x04,0x38}, // J
                     {0x00,0x44,0x48,0x50,0x60,0x50,0x48,0x44}, // K
                     {0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x7c}, // L
                     {0x00,0x82,0xc6,0xaa,0x92,0x82,0x82,0x82}, // M
                     {0x00,0x42,0x42,0x62,0x52,0x4a,0x46,0x42}, // N
                     {0x00,0x3c,0x42,0x42,0x42,0x42,0x44,0x38}, // O
                     {0x00,0x78,0x44,0x44,0x48,0x70,0x40,0x40}, // P
                     {0x00,0x3c,0x42,0x42,0x52,0x4a,0x44,0x3a}, // Q
                     {0x00,0x78,0x44,0x44,0x78,0x50,0x48,0x44}, // R
                     {0x00,0x38,0x40,0x40,0x38,0x04,0x04,0x78}, // S
                     {0x00,0x7e,0x90,0x10,0x10,0x10,0x10,0x10}, // T
                     {0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x3e}, // U
                     {0x00,0x42,0x42,0x42,0x42,0x44,0x28,0x10}, // V
                     {0x80,0x82,0x82,0x92,0x92,0x92,0x94,0x78}, // W
                     {0x00,0x00,0x42,0x24,0x18,0x24,0x42,0x00}, // X
                     {0x00,0x44,0x44,0x28,0x10,0x10,0x10,0x10}, // Y
                     {0x00,0x7c,0x04,0x08,0x7c,0x20,0x40,0xfe}, // Z
                      // (the font does not contain any lower case letters. you can add your own.)
                  };    // {}, //

/**
 * This function draws a string of the given length to the given position.
 */
void drawString(char* text, int len, int x, int y )
{
  for( int idx = 0; idx < len; idx ++ )
  {
    int c = text[idx] - 32;

    // stop if char is outside visible area
    if( x + idx * 8  > LEDMATRIX_WIDTH )
      return;

    // only draw if char is visible
    if( 8 + x + idx * 8 > 0 )
      drawSprite( font[c], x + idx * 8, y, 8, 8 );
  }
}

void showText()
{
  // Draw the text to the current position
  int lineLength  = text.length() + 1;
  char line[lineLength];

  text.toCharArray(line, lineLength);
  int len = strlen(line);
  drawString(line, len, x, 0);
  // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background...

  // Toggle display of the new framebuffer
  lmd.display();

}

/**
 * This draws a sprite to the given position using the width and height supplied (usually 8x8)
 */
void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  // The mask is used to get the column bit from the sprite row
  byte mask = B10000000;

  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      lmd.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));

      // shift the mask by one pixel to the right
      mask = mask >> 1;
    }

    // reset column mask
    mask = B10000000;
  }
}

void setNormalTime(RtcDateTime now)
{
  char timeString[16];
  char datestring[16];


  snprintf_P(timeString, 
          countof(timeString),
          PSTR("%02u:%02u"),
          now.Hour(),
          now.Minute());

  snprintf_P(datestring, 
          countof(datestring),
          PSTR("%02u-%02u"),
          now.Month(),
          now.Day() );

  line1 = String (timeString);
  line2 = String (datestring);
}

void setTimeCalc(RtcDateTime now)
{
  randomNumber = random(2);

  //Serial.println(String(randomNumber));
  if (randomNumber == 0)
  {
    // Serial.println("Random 0");
    set2NumberCalc(now.Hour(), line1);
    set3NumberCalc(now.Minute(), line2); 
  } else
  {
    // Serial.println("Random 1");
    set3NumberCalc(now.Hour(), line1);
    set2NumberCalc(now.Minute(), line2);
  }
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
  Serial.println(randomNumber);
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
            line = String(i) + "+" + String(j) + "+" + String(k);
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
            line = String(i) + "+" + String(j) + "-" + String(k);
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
            line = String(i) + "+" + String(j) + "X" + String(k);
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
            line = String(i) + "X" + String(j) + "+" + String(k);
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
            line = String(i) + "X" + String(j) + "-" + String(k);
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
              line = String(i) + "-" + String(j) + "+" + String(k);
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
              line = String(i) + "-" + String(j) + "-" + String(k);
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
            line = String(i) + "-" + String(j) + "X" + String(k);
            return;
          }
        }
      }
}

void set2NumberCalc(const int targetNumber, const String& line)
{
  int totalFound = 0;
  int result;

  for (int i = 0; i <= 68; i ++)
    for (int j = 0; j<= 68; j ++)
    {
      if ((i <= 9) || (j <= 9))
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
  Serial.println(randomNumber);
  */

  totalFound = 0;

  for (int i = 0; i <= 68; i ++)
    for (int j = 0; j<= 68; j ++)
    {
      if ((i <= 9) || (j <= 9))
      {
        // +
        result = i + j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + "+" + String(j);
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
            line = String(i) + "-" + String(j);
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
            line = String(i) + "X" + String(j);
            return;
          }
        }
      }
    }
}
