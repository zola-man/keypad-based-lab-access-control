#include <Keypad.h>
#include <Servo.h>

// ─── Pin Definitions ───────────────────────────────────────
#define GREEN_LED  12
#define RED_LED    11
#define BUZZER     13
#define SERVO_PIN  10

// ─── Keypad Configuration ──────────────────────────────────
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ─── Servo ─────────────────────────────────────────────────
Servo lockServo;

// ─── Authorized UIDs ───────────────────────────────────────
const String AUTHORIZED_UIDS[] = {"1234", "5678"};
const int NUM_AUTHORIZED = 2;

// ─── State Variables ───────────────────────────────────────
String inputUID = "";
bool doorOpen   = false;

// ─── Function Prototypes ───────────────────────────────────
bool checkUID(String uid);
void grantAccess();
void denyAccess();
void lockDoor();
void beepGrant();
void beepDeny();

// ───────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED,   OUTPUT);
  pinMode(BUZZER,    OUTPUT);
  
  lockServo.attach(SERVO_PIN);
  lockServo.write(0);
  
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED,   LOW);
  
  Serial.println("=================================");
  Serial.println("  RFID Lab Access Control System ");
  Serial.println("=================================");
  Serial.println("Enter UID and press # to authenticate");
  Serial.println("Press * to clear input");
  Serial.println("---------------------------------");
}

// ───────────────────────────────────────────────────────────
void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (key == '*') {
      inputUID = "";
      Serial.println("Input cleared. Enter UID:");
      return;
    }
    
    if (key == '#') {
      Serial.print("Scanning UID: ");
      Serial.println(inputUID);
      
      if (checkUID(inputUID)) {
        grantAccess();
      } else {
        denyAccess();
      }
      
      inputUID = "";
      return;
    }
    
    if (key == 'D' && doorOpen) {
      lockDoor();
      return;
    }
    
    inputUID += key;
    Serial.print("Input: ");
    Serial.println(inputUID);
  }
}

// ─── Check UID ─────────────────────────────────────────────
bool checkUID(String uid) {
  for (int i = 0; i < NUM_AUTHORIZED; i++) {
    if (uid == AUTHORIZED_UIDS[i]) {
      return true;
    }
  }
  return false;
}

// ─── Access Granted ────────────────────────────────────────
void grantAccess() {
  Serial.println("---------------------------------");
  Serial.println("ACCESS GRANTED");
  Serial.println("Door unlocking...");
  Serial.println("---------------------------------");
  
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED,   LOW);
  
  beepGrant();
  
  lockServo.write(90);
  doorOpen = true;
  
  // Green LED stays on for 5 seconds
  delay(5000);
  
  lockDoor();
}

// ─── Access Denied ─────────────────────────────────────────
void denyAccess() {
  Serial.println("---------------------------------");
  Serial.println("ACCESS DENIED");
  Serial.println("Unauthorized UID");
  Serial.println("---------------------------------");
  
  digitalWrite(RED_LED,   HIGH);
  digitalWrite(GREEN_LED, LOW);
  
  beepDeny();
  
  // Red LED stays on for 4 seconds after beeps finish
  delay(4000);
  
  digitalWrite(RED_LED, LOW);
  
  Serial.println("Enter UID and press # to authenticate");
}

// ─── Lock Door ─────────────────────────────────────────────
void lockDoor() {
  lockServo.write(0);
  doorOpen = false;
  
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED,   LOW);
  
  Serial.println("Door locked.");
  Serial.println("Enter UID and press # to authenticate");
}

// ─── Grant Beep: single clean tone ─────────────────────────
void beepGrant() {
  // One clean high tone: 1000Hz for 500ms
  tone(BUZZER, 1000, 500);
  delay(600);
  noTone(BUZZER);
}

// ─── Deny Beep: three low tones ────────────────────────────
void beepDeny() {
  // Three short low tones: 300Hz
  tone(BUZZER, 300, 300);
  delay(400);
  noTone(BUZZER);
  
  tone(BUZZER, 300, 300);
  delay(400);
  noTone(BUZZER);
  
  tone(BUZZER, 300, 300);
  delay(400);
  noTone(BUZZER);
}