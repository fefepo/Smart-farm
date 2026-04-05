#include <DHT.h>                // DHT 센서 라이브러리
#include <SoftwareSerial.h>     // 아두이노 간 SoftwareSerial 통신 라이브러리

// ===== DHT 센서 설정 =====
#define DHTPIN 2                // DHT22 데이터 핀
#define DHTTYPE DHT22           // DHT 센서 타입 설정

DHT dht(DHTPIN, DHTTYPE);       // DHT 객체 생성

int lightPin = A0;              // 조도 센서 아날로그 입력 핀

// ===== SoftwareSerial 설정 =====
// SoftwareSerial(RX, TX)
// Arduino 1이 Arduino 2로 데이터를 보내는 통신용 포트
SoftwareSerial farmSerial(10, 11);

void setup() {
  Serial.begin(9600);           // PC 시리얼 모니터 출력용 (디버깅)
  farmSerial.begin(9600);       // Arduino 2와의 통신 시작

  dht.begin();                  // DHT22 센서 초기화

  Serial.println("Sensor Arduino Start");  // 시작 메시지 출력
}

void loop() {
  // ----- 온도 / 습도 데이터 읽기 -----
  float temperature = dht.readTemperature();   // 현재 온도 읽기
  float humidity = dht.readHumidity();         // 현재 습도 읽기

  // ----- 조도 데이터 읽기 -----
  int lightLevel = analogRead(lightPin);       // 조도 센서 값 읽기

  // ----- 센서 값 유효성 검사 -----
  if (isnan(temperature) || isnan(humidity)) { // 온도 또는 습도 값이 비정상이면
    Serial.println("Failed to read from DHT sensor!"); // 오류 메시지 출력
    delay(2000);                               // 2초 대기
    return;                                    // 현재 loop 종료
  }

  // ----- Arduino 2로 전송할 문자열 생성 -----
  // 형식: 온도,습도,조도
  String data = String(temperature, 1) + "," + String(humidity, 1) + "," + String(lightLevel);

  // ----- 제어 아두이노로 데이터 전송 -----
  farmSerial.println(data);                    // SoftwareSerial로 한 줄 전송

  // ----- 시리얼 모니터 디버깅 출력 -----
  Serial.print("Send -> ");                    // 전송 로그 표시
  Serial.println(data);                        // 실제 전송 데이터 출력

  delay(2000);                                 // 2초 간격으로
}
