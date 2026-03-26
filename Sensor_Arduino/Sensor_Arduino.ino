#include <DHT.h>

#define DHTPIN 2         // DHT 센서 핀 설정
#define DHTTYPE DHT22    // DHT 센서 타입 (DHT11 또는 DHT22)

DHT dht(DHTPIN, DHTTYPE);  // DHT 센서 객체 생성

int lightPin = A0;         // 조도 센서 핀 설정

void setup() {
  Serial.begin(9600);      // 시리얼 통신 시작
  dht.begin();             // DHT 센서 초기화
}

void loop() {
  // 온습도 센서 값 읽기
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // 센서 값이 유효한지 확인
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 센서 값 전송 (아두이노 2로 전송)
  Serial.print(temperature);  // 온도
  Serial.print(",");          // 구분자
  Serial.print(humidity);     // 습도
  Serial.print(",");          // 구분자

  // 조도 센서 값 읽기
  int lightLevel = analogRead(lightPin);  // 조도 값 읽기
  Serial.println(lightLevel);  // 조도 값을 전송 후 줄바꿈

  delay(1000);  // 1초마다 센서 값 전송
}
