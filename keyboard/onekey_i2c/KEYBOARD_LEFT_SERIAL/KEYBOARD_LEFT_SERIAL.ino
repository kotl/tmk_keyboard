
#define HWSERIAL Serial2

// number of rows is virtually unlimited
#define ROWS 5
// Maximum columns is 8
#define COLS 8

// How often to print matrix state (100 will mean once every 100 scans)
#define SCAN_PRINT 300

byte matrix[ROWS][COLS];
int scanIteration;

byte rowPins[ROWS] = { 0, 1, 2, 3, 4 };
byte colPins[COLS] = { 14, 15, 16, 17, 18, 19, 20, 21}; 

#define LED_PIN 13

void keypad_init ()
{
  scanIteration = 0;
  memset(matrix, 0, sizeof(matrix));
  
  // set rows to HIGH Z
  for (int i = 0; i < ROWS; i++) {
    pinMode (rowPins[i], INPUT);
    digitalWrite (rowPins[i], LOW);
  }

  // set cols to input with pullups
  for (int j = 0; j < COLS; j++) {
    pinMode (colPins[j], INPUT);
    digitalWrite (colPins[j], HIGH);
  }
}


void setup()
{
  #ifdef DEBUG 
    Serial.begin(115200);
    delay(3000);
  #endif

  pinMode (LED_PIN, OUTPUT);
  digitalWrite (LED_PIN, HIGH);  
  delay(500);
  HWSERIAL.begin(57600  ,SERIAL_8N1);
  keypad_init();
}

void printMatrix() {
  Serial.println(scanIteration/SCAN_PRINT);
  for(int row=0; row<ROWS; row++) {
    for (int col=0; col<COLS; col++) {
      Serial.print(((matrix[row][col] > 0) ? '1' : '0'));
    }
    Serial.println();
  }
  Serial.println();
}

void readRow(int row) {  
    // set row to LOW
    pinMode (rowPins[row], OUTPUT);
    delayMicroseconds(34);  // without this wait read unstable value.
    for (int col = 0; col < COLS; col++) {
      matrix[row][col] = (digitalRead(colPins[col])==LOW) ? 1 : 0;
    }
    // set row to High Z
    pinMode (rowPins[row], INPUT);

}

void loop()
{
  for(int row=0;row < ROWS; row++) {
    readRow(row);
  }
  scanIteration++;
  if (scanIteration % SCAN_PRINT == 0) {
    #ifdef DEBUG
      printMatrix();
    #endif
  }
  // Write to serial: rrder of bytes does not matter on receiving end:
  // each byte carries column number in highest 3 bits and has switches
  // state in the lowest 5 bits for appropriate row.
  while(HWSERIAL.availableForWrite()<COLS) {
  }
  for(int col=0;col < COLS; col++) {
    // Bits 0-4 are key switch states reversed for rows
    // Bits 5-7 are column number, i.e.
    // 0b10001000 will mean that for column 4, row 1 has key pressed
    // 0b11100010 will mean that for column 7, row 3 has key pressed
    byte val = col;
    for (int row=0;row<ROWS; row++) {
      val = val << 1;
      val += matrix[row][col];
    }
    HWSERIAL.write(~val & 0xFF);
  }
}



