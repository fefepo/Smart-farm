#define HEATER_PIN 3       // PTC 히터 릴레이 핀 설정
#define FAN_PIN 4          // 냉각 팬 릴레이 핀 설정
#define HUMIDIFIER_PIN 5   // 가습기 릴레이 핀 설정
#define LIGHT_PIN 6        // LED 조명 릴레이 핀 설정

void setup() {
  Serial.begin(9600);      // 시리얼 통신 시작

  pinMode(HEATER_PIN, OUTPUT);       // PTC 히터 릴레이 핀 출력 설정
  pinMode(FAN_PIN, OUTPUT);          // 냉각 팬 릴레이 핀 출력 설정
  pinMode(HUMIDIFIER_PIN, OUTPUT);   // 가습기 릴레이 핀 출력 설정
  pinMode(LIGHT_PIN, OUTPUT);        // LED 조명 릴레이 핀 출력 설정
}

void loop() {
  if (Serial.available() > 0) {
    // 시리얼로 받은 데이터 읽기
    String sensorData = Serial.readStringUntil('\n');  // 개행 문자까지 읽기

    // 데이터 처리 (온도, 습도, 조도 순으로 나누기)
    float temperature, humidity;
    int lightLevel;

    // 문자열을 ',' 기준으로 분리하여 각 변수에 값 저장
    sscanf(sensorData.c_str(), "%f,%f,%d", &temperature, &humidity, &lightLevel);

    // 센서 값 출력 (디버깅용)
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" C, ");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %, ");
    Serial.print("Light Level: ");
    Serial.println(lightLevel);

    // 온도에 따른 제어
    if (temperature < 18) {
      digitalWrite(HEATER_PIN, HIGH);  // 히터 켜기 (릴레이 스위치 ON)
      digitalWrite(FAN_PIN, LOW);      // 팬 끄기 (릴레이 스위치 OFF)
    } else if (temperature > 25) {
      digitalWrite(FAN_PIN, HIGH);     // 팬 켜기 (릴레이 스위치 ON)
      digitalWrite(HEATER_PIN, LOW);   // 히터 끄기 (릴레이 스위치 OFF)
    } else {
      digitalWrite(HEATER_PIN, LOW);   // 히터 끄기 (릴레이 스위치 OFF)
      digitalWrite(FAN_PIN, LOW);      // 팬 끄기 (릴레이 스위치 OFF)
    }

    // 습도에 따른 제어
    if (humidity < 40) {
      digitalWrite(HUMIDIFIER_PIN, HIGH);  // 가습기 켜기 (릴레이 스위치 ON)
    } else if (humidity > 60) {
      digitalWrite(HUMIDIFIER_PIN, LOW);   // 가습기 끄기 (릴레이 스위치 OFF)
    }

    // 조도에 따른 제어
    if (lightLevel < 500) {
      digitalWrite(LIGHT_PIN, HIGH);  // LED 켜기 (릴레이 스위치 ON)
    } else {
      digitalWrite(LIGHT_PIN, LOW);   // LED 끄기 (릴레이 스위치 OFF)
    }
  }
}
