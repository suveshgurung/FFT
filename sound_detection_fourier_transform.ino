#define SOUND_SENSOR_DIGITAL_INPUT 22
#define SOUND_SENSOR_ANALOG_INPUT 4

#define SAMPLING_FREQUENCY 500
#define SAMPLE_TIME 2000        // 2500 microseconds sampling time

#define ADC_MAX_VALUE 4095.0
#define ADC_MAX_VOLT 3.3

unsigned long current_time = 0, previous_time = 0, elasped_time;
unsigned long number_of_samples = 0;
float calibrated_base_value = 0.0;

float calibrate() {
  float average_voltage = 0.0;
  for (int i = 0; i < 1000; i++) {
    float analog_value = analogRead(SOUND_SENSOR_ANALOG_INPUT);
    float analog_equivalent_voltage = (ADC_MAX_VOLT / ADC_MAX_VALUE) * analog_value;

    average_voltage += analog_equivalent_voltage;
  }
  average_voltage /= 1000;
  Serial.print("Calibrated average voltage: ");
  Serial.println(average_voltage);

  return average_voltage;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(SOUND_SENSOR_DIGITAL_INPUT, INPUT_PULLDOWN);

  calibrated_base_value = calibrate();
  previous_time = micros();
}

void loop() {
  current_time = micros();
  elasped_time = current_time - previous_time;

  if (elasped_time > SAMPLE_TIME) {
    float analog_value = analogRead(SOUND_SENSOR_ANALOG_INPUT);
    float analog_equivalent_voltage = (ADC_MAX_VOLT / ADC_MAX_VALUE) * analog_value;
    analog_value = ADC_MAX_VALUE;
    if (analog_equivalent_voltage < calibrated_base_value) {
      number_of_samples++;
      analog_equivalent_voltage = calibrated_base_value - analog_equivalent_voltage;
      Serial.println(analog_equivalent_voltage);
      analog_equivalent_voltage = calibrated_base_value;
    }
    previous_time = current_time;
  }

  int digital_value = digitalRead(SOUND_SENSOR_DIGITAL_INPUT);

  if (digital_value) {
    Serial.println(number_of_samples);
  }
}
