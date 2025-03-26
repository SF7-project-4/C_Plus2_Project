#include <Servo.h>

#define DC_MOTOR_PIN 5      // DC 모터 제어용 핀
#define SERVO_PIN 9     // 서보 모터 제어용 핀

Servo myServo;
String inputString = "";    // 수신된 문자열 저장용
bool stringComplete = false;

void setup() {
  Serial.begin(9600);

  pinMode(DC_MOTOR_PIN, OUTPUT);
  digitalWrite(DC_MOTOR_PIN, LOW);  // 초기에는 꺼짐 상태

  myServo.attach(SERVO_PIN);
  myServo.write(90); // 초기 위치 (중간값)

  inputString.reserve(50); // 메모리 확보
}

void loop() {
  // 시리얼 명령 처리
  if (stringComplete) {
    inputString.trim();  // 앞뒤 공백 제거

    if (inputString == "MOVE_ON") {
      digitalWrite(DC_MOTOR_PIN, 50);
      Serial.println("작동");
    }
    else if (inputString == "MOVE_OFF") {
      digitalWrite(DC_MOTOR_PIN, 0);
    }
    else if (inputString == "GRAB") {
      myServo.write(30); // 집는 위치
    }
    else if (inputString == "RELEASE") {
      myServo.write(100); // 놓는 위치
    }

    // 처리 후 초기화
    inputString = "";
    stringComplete = false;
  }
}

// 시리얼 이벤트 콜백 (문자열 수신 완료 시 호출됨)
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}