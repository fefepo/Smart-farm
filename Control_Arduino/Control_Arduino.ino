#include <Wire.h>                  // I2C 통신 라이브러리
#include <LiquidCrystal_I2C.h>     // I2C LCD 라이브러리
#include <SoftwareSerial.h>        // 아두이노 간 SoftwareSerial 통신 라이브러리

// ===== 릴레이 연결 핀 설정 =====
#define HEATING_PAD_PIN 3      // 열선패드 제어 릴레이 핀
#define FAN_PIN 4              // 냉각 팬 제어 릴레이 핀
#define HUMIDIFIER_PIN 5       // 가습기 제어 릴레이 핀
#define LIGHT_PIN 6            // LED 조명 제어 릴레이 핀

// ===== 릴레이 동작 방식 설정 =====
// 대부분의 릴레이 모듈은 LOW 입력 시 동작하는 Active LOW 방식
#define RELAY_ACTIVE_LOW true

#if RELAY_ACTIVE_LOW
  #define RELAY_ON LOW       // LOW 신호일 때 릴레이 ON
  #define RELAY_OFF HIGH     // HIGH 신호일 때 릴레이 OFF
#else
  #define RELAY_ON HIGH      // HIGH 신호일 때 릴레이 ON
  #define RELAY_OFF LOW      // LOW 신호일 때 릴레이 OFF
#endif

// ===== LCD 객체 생성 =====
// 주소 0x27, 16칸 2줄 LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== SoftwareSerial 설정 =====
// SoftwareSerial(RX, TX)
// Arduino 1에서 보낸 센서 데이터를 수신하는 통신용 포트
SoftwareSerial farmSerial(10, 11);

// ===== 릴레이 제어 함수 =====
// pin : 릴레이 핀 번호
// on  : true면 ON, false면 OFF
void setRelay(int pin, bool on) {
  digitalWrite(pin, on ? RELAY_ON : RELAY_OFF);   // 릴레이 상태 출력
}

void setup() {
  Serial.begin(9600);         // PC 시리얼 모니터 출력용 (디버깅)
  farmSerial.begin(9600);     // Arduino 1과의 통신 시작

  // ----- 릴레이 핀을 출력 모드로 설정 -----
  pinMode(HEATING_PAD_PIN, OUTPUT);     // 열선패드 릴레이 핀 출력 설정
  pinMode(FAN_PIN, OUTPUT);             // 팬 릴레이 핀 출력 설정
  pinMode(HUMIDIFIER_PIN, OUTPUT);      // 가습기 릴레이 핀 출력 설정
  pinMode(LIGHT_PIN, OUTPUT);           // LED 릴레이 핀 출력 설정

  // ----- 모든 장치 초기 OFF 상태 설정 -----
  setRelay(HEATING_PAD_PIN, false);   // 열선패드 OFF
  setRelay(FAN_PIN, false);           // 팬 OFF
  setRelay(HUMIDIFIER_PIN, false);    // 가습기 OFF
  setRelay(LIGHT_PIN, false);         // LED OFF

  // ----- LCD 초기화 -----
  lcd.init();                       // LCD 초기화
  lcd.backlight();                  // LCD 백라이트 ON

  // ----- 시작 화면 표시 -----
  lcd.setCursor(0, 0);               // 1행 1열로 커서 이동
  lcd.print("Smart Farm");           // 제목 출력
  lcd.setCursor(0, 1);               // 2행 1열로 커서 이동
  lcd.print("System Start");         // 시작 메시지 출력
  delay(2000);                       // 2초 대기
  lcd.clear();                       // LCD 화면 지우기

  Serial.println("Control Arduino Start"); // 시작 로그 출력
}

void loop() {
  // ----- SoftwareSerial로 수신된 데이터가 있는지 확인 -----
  if (farmSerial.available()) {

    // 한 줄 단위로 센서 데이터 읽기
    String sensorData = farmSerial.readStringUntil('\n');  // 줄바꿈 문자 전까지 읽기
    sensorData.trim();                                     // 앞뒤 공백 제거

    float temperature = 0.0;   // 수신한 온도 저장 변수
    float humidity = 0.0;      // 수신한 습도 저장 변수
    int lightLevel = 0;        // 수신한 조도 저장 변수

    // ----- "온도,습도,조도" 형식의 문자열 파싱 -----
    int result = sscanf(sensorData.c_str(), "%f,%f,%d",
                        &temperature, &humidity, &lightLevel);

    // 파싱 실패 시 오류 처리
    if (result != 3) {
      Serial.print("Invalid Data: ");   // 잘못된 데이터 로그 출력
      Serial.println(sensorData);
      return;                           // 현재 loop 종료
    }

    // ----- 시리얼 모니터에 수신값 출력 (디버깅용) -----
    Serial.print("Recv -> Temp: ");
    Serial.print(temperature);
    Serial.print(" C, Hum: ");
    Serial.print(humidity);
    Serial.print(" %, Light: ");
    Serial.println(lightLevel);

    // ===============================================
    // 온도 제어 로직
    // 온도 < 18도  -> 열선패드 ON, 팬 OFF
    // 온도 > 25도  -> 팬 ON, 열선패드 OFF
    // 18도 ~ 25도  -> 둘 다 OFF
    // ===============================================
    if (temperature < 18.0) {
      setRelay(HEATING_PAD_PIN, true);    // 열선패드 ON
      setRelay(FAN_PIN, false);           // 팬 OFF
    } else if (temperature > 25.0) {
      setRelay(HEATING_PAD_PIN, false);   // 열선패드 OFF
      setRelay(FAN_PIN, true);            // 팬 ON
    } else {
      setRelay(HEATING_PAD_PIN, false);   // 열선패드 OFF
      setRelay(FAN_PIN, false);           // 팬 OFF
    }

    // ======================================
    // 습도 제어 로직
    // 습도 < 40%  -> 가습기 ON
    // 습도 > 60%  -> 가습기 OFF
    // ======================================
    if (humidity < 40.0) {
      setRelay(HUMIDIFIER_PIN, true);     // 가습기 ON
    } else if (humidity > 60.0) {
      setRelay(HUMIDIFIER_PIN, false);    // 가습기 OFF
    }

    // =========================================
    // 조도 제어 로직
    // 조도 < 500  -> LED ON
    // 조도 >= 500 -> LED OFF
    // =========================================
    if (lightLevel < 500) {
      setRelay(LIGHT_PIN, true);          // LED ON
    } else {
      setRelay(LIGHT_PIN, false);         // LED OFF
    }

    // ----- LCD 화면 갱신 -----
    lcd.clear();                          // LCD 화면 초기화

    // 1행: 온도 / 습도 표시
    lcd.setCursor(0, 0);                // 1행 시작 위치
    lcd.print("Temp:");
    lcd.print(temperature, 1);          // 온도 소수점 1자리 표시
    lcd.print("C");
    
    lcd.print(" Hum:");
    lcd.print(humidity, 0);       // 습도 (정수)
    lcd.print("%");

    // 2행: 조도 표시
    lcd.setCursor(0, 1);         // 2행 시작 위치
    lcd.print("Light:");
    lcd.print(lightLevel);       // 조도 값 출력
  }
}
