#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <Arduino.h>
#include <TimerOne.h>
#include <LiquidCrystal.h>
#include <Wire.h>

// ----SEÑAL CUADRADA---
// Variables de la señal cuadrada
int outputPin = 11; // Pin number where the signal is generated
unsigned long frequency = 95; // Frequency of the square wave in Hz
int period = 1000000 / frequency; // Period of the square wave in microseconds
int halfPeriod = period / 2; // Half period of the square wave in microseconds
int prescaler = 8; // Prescaler value for Timer/Counter 1
int OCRValue = (F_CPU / (2 * prescaler * frequency)) - 1; // OCR value for CTC mode

// ----LCD---
// Declarar constantes para usar puertos específicos
const int rs = A3, en = A5, d4 = A9, d5 = A10, d6 = A11, d7 = A12;
// Iniciar el objeto del LCD con los puertos específicos
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ----BOTONES---
// Pins para los botones del control de frecuencia
const int freqIncreasePin = 2;
const int freqDecreasePin = 3;
const int freqChangePin = 4;
// Variables para los botones del control de frecuencia
bool freqIncreasePressed = false;
bool freqDecreasePressed = false;
bool freqChangePressed = false;
unsigned long freqChangeAmount = 1;
// Función de actualización de la frecuencia de la señal
void updateFrequency() {
  if (frequency < 17) {
    frequency = 17;
  } else if (frequency > 1000000) {
    frequency = 1000000;
  }
  period = 1000000 / frequency;
  halfPeriod = period / 2;
  OCRValue = (F_CPU / (2 * prescaler * frequency)) - 1;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    OCR1A = OCRValue;
  }
}

void setup() {
  // Establecer los valores de las interrupciones
  Serial.begin(115200);
  noInterrupts();
  pinMode(outputPin, OUTPUT);
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1A |= (1 << COM1A0);
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);
  OCR1A = OCRValue;

  // Iniciar la pantalla LCD
  pinMode(A14,OUTPUT);
  pinMode(A13,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A1,OUTPUT);
  digitalWrite(A14,LOW); 
  digitalWrite(A13,HIGH); 
  digitalWrite(A4,LOW); 
  digitalWrite(A0,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(A1,HIGH);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Frequencia:");
  lcd.setCursor(0, 1);
  lcd.print(frequency);
  lcd.print(" Hz");

}

void loop() {
  // Leer el estado del botón de aumento de frecuencia y elimina el rebote
  bool currentFreqIncreasePressed = digitalRead(freqIncreasePin) == LOW;
  if (currentFreqIncreasePressed && !freqIncreasePressed) {
    freqIncreasePressed = true;
    frequency += freqChangeAmount;
    updateFrequency();
  } else if (!currentFreqIncreasePressed) {
    freqIncreasePressed = false;
  }
  
  // Leer el estado del botón de disminución de frecuencia y elimina el rebote
  bool currentFreqDecreasePressed = digitalRead(freqDecreasePin) == LOW;
  if (currentFreqDecreasePressed && !freqDecreasePressed) {
    freqDecreasePressed = true;
    frequency -= freqChangeAmount;
    updateFrequency();
  } else if (!currentFreqDecreasePressed) {
    freqDecreasePressed = false;
  }

  // Leer el estado del botón de cambio de frecuencia y elimina el rebote
  bool currentFreqChangePressed = digitalRead(freqChangePin) == LOW;
  if (currentFreqChangePressed && !freqChangePressed) {
    freqChangePressed = true;
    freqChangeAmount *= 10;
    if (freqChangeAmount > 1000001) {
      freqChangeAmount = 1;
    }
  } else if (!currentFreqChangePressed) {
    freqChangePressed = false;
  }


  // Generar la señal cuadrada con la frecuencia actualizada
  digitalWrite(outputPin, HIGH);
  delayMicroseconds(halfPeriod);
  digitalWrite(outputPin, LOW);
  delayMicroseconds(halfPeriod);

  // Salida por serial para ver la frecuencia
  //Serial.print("Frequencia: ");
  //Serial.print(frequency);
  //Serial.println(" Hz");

  // Actualizarr el valor actual de frecuencia en a pantalla LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Frec.: ");
  lcd.print(frequency);
  lcd.print(" Hz");
  lcd.setCursor(0, 1);
  lcd.print("Salto: 10^");
  lcd.print(int(log10(freqChangeAmount)));
  delay(100); // Delay para prevenir parpadeo

}


