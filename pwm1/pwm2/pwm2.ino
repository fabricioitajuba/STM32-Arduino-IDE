#include <LiquidCrystal.h> // include the LCD library

const int rs = PA6, en = PA7, d4 = PB0, d5 = PB1, d6 = PB10, d7 = PB11; //mention the pin names to with LCD is connected to 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //Initialize the LCD

void setup() {
  pinMode(0, INPUT_ANALOG); //ADC0
  pinMode(1, PWM);          //ADC1

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PWM=");
    
}

void loop() {
    int sensorValue = analogRead(0);
    int ledFadeValue = map(sensorValue, 0, 4095, 0, 65535);
    int valor = map(ledFadeValue, 0, 65535, 0, 100);
    
    pwmWrite(1, ledFadeValue);

    lcd.setCursor(0, 1);  
    lcd.print("   ");
    lcd.setCursor(0, 1);  
    lcd.print(valor);   
    
    delay(1000); 
}
