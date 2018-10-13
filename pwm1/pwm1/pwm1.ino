void setup() {
    pinMode(0, INPUT_ANALOG); //ADC0
    pinMode(1, PWM);          //ADC1
}

void loop() {
    int sensorValue = analogRead(0);
    int ledFadeValue = map(sensorValue, 0, 4095, 0, 65535);
    pwmWrite(1, ledFadeValue);
}
