#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define IN_BUFFER_SIZE 256 // or whatever size you want

char inputBuffer[IN_BUFFER_SIZE]; // The buffer
int inBufferWrite = 0;            // Where to write to the buffer next
int inBufferRead = 0;             // Where to read from the buffer next

// Function declarations
void writeToBuffer(char c);
char readFromBuffer();
int charactersInBuffer();
String readQRCode();
void displayQRData(String data);

// Define the screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 // Updated height for the 1.3" OLED

#define SDA_PIN 21 // Change SDA_PIN and SCL_PIN to your actual pin numbers
#define SCL_PIN 22

// Define OLED pins
#define OLED_RESET 4

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);

  Serial.begin(9600); // Use Serial for hardware serial communication

  Wire.begin();

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Display initialization message
  display.display();
  delay(2000); // Wait for 2 seconds
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  Serial.println("QR Code Scanner Ready!");
}

void loop()
{
  while (Serial.available() > 0) // Use Serial for reading data
  {
    char c = Serial.read();
    writeToBuffer(c);
  }

  if (charactersInBuffer() > 0)
  {
    String qrData = readQRCode();
    displayQRData(qrData);
    delay(2000); // Display for 2 seconds before clearing the screen
    display.clearDisplay();
  }

  delay(100);
}

void writeToBuffer(char c)
{
  inputBuffer[inBufferWrite] = c;
  inBufferWrite = (inBufferWrite + 1) % IN_BUFFER_SIZE;
}

char readFromBuffer()
{
  char c = inputBuffer[inBufferRead];
  inBufferRead = (inBufferRead + 1) % IN_BUFFER_SIZE;
  return c;
}

int charactersInBuffer()
{
  return (inBufferWrite - inBufferRead + IN_BUFFER_SIZE) % IN_BUFFER_SIZE;
}

String readQRCode()
{
  String qrData = "";
  while (charactersInBuffer() > 0)
  {
    char c = readFromBuffer();
    if (c == '\n' || c == '\r')
    {
      break;
    }
    qrData += c;
  }
  return qrData;
}

void displayQRData(String data)
{
  // Display QR code data on the OLED screen
  display.setCursor(0, 0);
  display.println("QR Code Data:");
  display.println(data);
  display.display(); // Update the OLED display
}
