/* 
 *  original from https://www.edgemicrotech.com/timer-with-oled-display-encoder-esp8266/
 *  v.1.0 - changed by niq_ro (Nicu Florica) to Arduino board from ESP8266
 *  v.1.1 - corrected dispay info for time longer than 1 minute, also in serial monitor inf updated at every seconds
*/

#include <U8g2lib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS 0x3C  // Replace with the correct I2C address

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const int CLK = 2; // D6;
const int DT = 3; // D7;
const int SW = 4; // D4;
#define led 13

int encoderValue = 0;
int lastCLKState;
int lastDTState;
bool buttonPressed = false;
bool timerRunning = false;
unsigned long countdownStartTime;
unsigned long countdownDuration = 0;
unsigned long tpafisare = 0;
unsigned long remainingTime1;

void setup() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  lastCLKState = digitalRead(CLK);
  lastDTState = digitalRead(DT);

  Serial.begin(115200);

  // Initialize OLED display
  u8g2.begin();
  //u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.clearBuffer();
  u8g2.setCursor(15, 15);
  u8g2.print("Select the Time:");
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(45, 35);
  u8g2.print("0 min");
  u8g2.sendBuffer();
}

void loop() {
  int currentStateCLK = digitalRead(CLK);
  int currentStateDT = digitalRead(DT);

  if (currentStateCLK != lastCLKState) {
    if (currentStateCLK == HIGH) {
      // On rising edge of CLK
      if (currentStateDT == LOW) {
        // Clockwise rotation
        encoderValue++;
      } else {
        // Counter-clockwise rotation
        encoderValue--;
      }

      // Ensure encoderValue is not negative
      if (encoderValue < 0) {
        encoderValue = 0;
      }

      // Limit encoderValue to 60 minute
      if (encoderValue > 60) {
        encoderValue = 60;
      }

      Serial.print("Encoder Value: ");
      Serial.println(encoderValue);

      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.clearBuffer();
      u8g2.setCursor(15, 15);
      u8g2.print("Select the Time:");
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.setCursor(45, 35);
      u8g2.print(encoderValue);
      u8g2.print(" min");
      u8g2.sendBuffer();

      // Convert encoder value to secunde (adjust the scale as needed)
      countdownDuration = encoderValue * 60;
    }
  }

  lastCLKState = currentStateCLK;
  lastDTState = currentStateDT;

  if (timerRunning) {
    unsigned long elapsedTime = millis() - countdownStartTime;

    if (elapsedTime < countdownDuration * 1000) 
    {
      unsigned long remainingTime = (countdownDuration * 1000) - elapsedTime;
      remainingTime1 = remainingTime/1000;
      int minuti = remainingTime1 / 60;
      int secunde = remainingTime1 % 60;
      digitalWrite(led, 1);
      if (millis() - tpafisare > 1000)
      {
      Serial.print("Remaining Time: ");
      Serial.print(minuti);
      Serial.print(" minute(s) and ");
      Serial.print(secunde);
      Serial.println(" seconds");
      tpafisare = millis();
      }
      // Update OLED display during countdown
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setCursor(15, 15);
      u8g2.print("Remaining Time: ");
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.setCursor(35, 35);
      u8g2.print(minuti/10);
      u8g2.print(minuti%10);
      u8g2.print("m ");
      u8g2.print(secunde/10);
      u8g2.print(secunde%10);
      u8g2.print("s");
      u8g2.sendBuffer();
    } else {
      Serial.println("Countdown complete");
      digitalWrite(led, 0);
      timerRunning = false;
      Serial.println("Input enabled");

      // Display a message indicating countdown completion
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setCursor(0, 10);
      u8g2.print("Countdown Complete!");
      digitalWrite(led, 0);
      u8g2.sendBuffer();
    }
  }

  if (digitalRead(SW) == LOW && !buttonPressed && !timerRunning) {
    buttonPressed = true;
    timerRunning = true;
    countdownStartTime = millis();

    Serial.print("Countdown started for ");
    Serial.print(countdownDuration / 60);
    Serial.print(" minute(s) and ");
    Serial.print(countdownDuration % 60);
    Serial.println(" seconds");
  } else 
  if (digitalRead(SW) == HIGH && buttonPressed) 
  {
    buttonPressed = false;
  }

  delay(1);
}
