#define READ_FROM_SOUND_SENSOR_FLAG 22
#define SOUND_SENSOR_ANALOG_INPUT 4

#define SAMPLE_TIME 250        // 250 microseconds sampling time

#define ADC_MAX_VALUE 4095.0
#define ADC_MAX_VOLT 3.3

#define TIMEOUT 2000

unsigned long current_time = 0, previous_time = 0, elasped_time;
unsigned long current_timeout_time = 0, previous_timeout_time = 0, elasped_timeout_time;
float calibrated_base_value = 0.0;
int is_first_data = 1;

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

  pinMode(READ_FROM_SOUND_SENSOR_FLAG, INPUT_PULLDOWN);

  calibrated_base_value = calibrate();
  previous_time = micros();
}

void loop() {
  current_time = micros();
  elasped_time = current_time - previous_time;

  if (digitalRead(READ_FROM_SOUND_SENSOR_FLAG)) {
    if (elasped_time > SAMPLE_TIME) {
      float analog_value = analogRead(SOUND_SENSOR_ANALOG_INPUT);
      float analog_equivalent_voltage = (ADC_MAX_VOLT / ADC_MAX_VALUE) * analog_value;

      if (is_first_data) {
        Serial.println("%");        // Delimeter character to signify first data is coming.
        is_first_data = 0;
        previous_timeout_time = millis();
      }

      float ac_signal = analog_equivalent_voltage - calibrated_base_value;
      Serial.println(ac_signal);

      previous_time = current_time;
      previous_timeout_time = millis();
    }
  }

  // Send a data end flag.
  if (!is_first_data) {
    current_timeout_time = millis();
    elasped_timeout_time = current_timeout_time - previous_timeout_time;
    if (elasped_timeout_time > TIMEOUT) {
      Serial.println("%e");
      elasped_timeout_time = 0;
      is_first_data = 1;
    }
  }
}