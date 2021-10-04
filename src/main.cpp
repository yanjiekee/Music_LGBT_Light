#include <Arduino.h>
#include <math.h>
#include <arduinoFFT.h>
#include <Adafruit_NeoPixel.h>

#define ANALOG_IN_PIN A0
#define SAMPLE_SIZE 64
#define SAMPLING_FREQ 100000
#define PREXRES 32
#define XRES 3

//DEBUG
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03
//DEBUG

#define LED_PIN    6
#define LED_COUNT  120

double vReal[SAMPLE_SIZE];
double vImag[SAMPLE_SIZE];
double freqSpec[PREXRES];
double freqSpecTwo[XRES];
int colorSpec[XRES];
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLE_SIZE, SAMPLING_FREQ);

uint32_t colorArray[LED_COUNT];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Function Declaration
void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType);
void pushColor(uint8_t r, uint8_t g, uint8_t b);

void setup() {
    Serial.begin(9600);

    analogReference(AR_INTERNAL2V4);

    strip.begin();
    strip.show();
    strip.setBrightness(75);

    delay(100);
}

void loop() {

    for(int i = 0; i < SAMPLE_SIZE; i++) {
        // 493 is equivalent to 1.59V, which was introduced before ADC
        int input = analogRead(ANALOG_IN_PIN) - 493;

        vReal[i] = input / 8;
        vImag[i] = 0;
    }

    FFT.DCRemoval();
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();

    int step = (SAMPLE_SIZE / 2) / PREXRES;
    for(int i = 0; i < (SAMPLE_SIZE / 2); i += step)
    {
        int c = i / step;
        freqSpec[c] = 0;
        for (int k = 0 ; k < step ; k++) {
            freqSpec[c] += vReal[i+k];
        }
        freqSpec[c] = freqSpec[c] / step;
    }

    // Frequency is divided into PREXRES steps from 0Hz - 48000Hz (1500Hz each step) in the above loop, giving freqSpec[PREXRES]
    // Its then further divided below into XRES section from 1500Hz - 19500Hz (6000Hz), giving freqSpecTwo[XRES]
    // Then, each section is feed as the R,G,B parameter to make up a color
    for(int i = 1; i < 13; i += 4)
    {
        int c = (i - 1) / 4;
        freqSpecTwo[c] = 0;
        for (int k = 0; k < 4; k++)
        {
            freqSpecTwo[c] += freqSpec[i+k];
        }
        // 5 is chosen arbritarily to scale down the magnitude representing each frequency sections to fit 0-255
        colorSpec[c] = (int)(freqSpecTwo[c] / 5);
    }
    pushColor(colorSpec[0], colorSpec[1], colorSpec[2]);

    //DEBUG
    Serial.print(colorSpec[0]);
    Serial.print(", ");
    Serial.print(colorSpec[1]);
    Serial.print(", ");
    Serial.println(colorSpec[2]);
    //DEBUG
}

    void pushColor(uint8_t r, uint8_t g, uint8_t b) {
    for(int i = LED_COUNT - 1; i > 0; i--) {
        colorArray[i] = colorArray[i-1];
        strip.setPixelColor(i, colorArray[i]);
    }

    colorArray[0] = strip.Color(r, g, b);
    strip.setPixelColor(0, colorArray[0]);
    strip.show();
    }

//DEBUG
void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
    for (uint16_t i = 0; i < bufferSize; i++)
    {
        double abscissa;
        /* Print abscissa value */
        switch (scaleType)
        {
            case SCL_INDEX:
                abscissa = (i * 1.0);
                break;
            case SCL_TIME:
                abscissa = ((i * 1.0) / SAMPLING_FREQ);
                break;
            case SCL_FREQUENCY:
                abscissa = ((i * 1.0 * SAMPLING_FREQ) / SAMPLE_SIZE);
                break;
        }
        Serial.print(abscissa, 6);
        if(scaleType == SCL_FREQUENCY)
            Serial.print("Hz");
        Serial.print(" ");
        Serial.println(vData[i], 4);
    }
    Serial.println();
}
//DEBUG
