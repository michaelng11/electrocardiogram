#include <Adafruit_GFX.h>
#include <ArducamSSD1306.h>
#include <Wire.h>

#define OLED_RESET  16  // Pin 15 -RESET digital signal
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

ArducamSSD1306 display(OLED_RESET);

const unsigned long SAMPLING_RATE = 10; // hertz
const uint8_t ECG_INPUT = A0;
const uint8_t SD_CARD = 10;
unsigned long lastSampleTime = 0;

// BPM ALGORITHM VARIABLES
int threshold = 570;
unsigned long lastBeatTime = 0;
const unsigned long MIN_DELAY = 300; // milliseconds

float bpmReadings[10] = {0.0};
float total = 0;
int i = 0;
int beatCount = 0;
int averageBpm = 0;



void setup() {
  pinMode(ECG_INPUT, INPUT);
  pinMode(SD_CARD, OUTPUT);
  Serial.begin(115200);
  
  display.begin();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("EKG Initializing...");
  display.display();
  delay(1000); 
  display.clearDisplay();
  
}

void loop() {
  unsigned long currentTime = micros();
  int currentReading;
  bool read = false;

  if (currentTime - lastSampleTime >= (1000000 / SAMPLING_RATE)) {
    lastSampleTime = currentTime;

    currentReading = analogRead(ECG_INPUT);
    read = true;

    // Serial.println(currentReading); // (for use in serial plotter only)
  }
  
  if (read && currentReading >= threshold && currentTime - lastBeatTime > (MIN_DELAY * 1000)) {
    unsigned long deltaTime = currentTime - lastBeatTime;

    float bpm = 60000000.0 / deltaTime;
    lastBeatTime = currentTime;

    total -= bpmReadings[i]; // delete oldest bpm from total
    bpmReadings[i] = bpm;
    total += bpm;
    i++;
    beatCount++;
    if (i == 10) { i = 0; }

    if (beatCount < 10)
      averageBpm = total / beatCount;
    else
      averageBpm = total / 10;

    Serial.print("BPM: ");
    Serial.println(averageBpm);
    
    display.setTextSize(1);      
    display.setCursor(0, 0);     
    display.print("BPM:");

    display.setTextSize(3);       // 3x scale for visibility
    display.setCursor(10, 20);    // Position text lower down
    display.print(averageBpm);

    display.display();
    
  }
  
  
}
