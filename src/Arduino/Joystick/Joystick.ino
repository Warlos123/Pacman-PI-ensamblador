const int VRX = A0;
const int VRY = A1;
const int DEADZONE = 200; // 0-1023, centro = 512

void setup() {
    Serial.begin(9600);
}

void loop() {
    int x = analogRead(VRX); // 0=izq, 1023=der
    int y = analogRead(VRY); // 0=arriba, 1023=abajo

    int dx = x - 512;
    int dy = y - 512;

    char dir = 'N';

    if (abs(dx) > abs(dy)) {
        if      (dx >  DEADZONE) dir = 'R';
        else if (dx < -DEADZONE) dir = 'L';
    } else {
        if      (dy >  DEADZONE) dir = 'D';
        else if (dy < -DEADZONE) dir = 'U';
    }

    Serial.write(dir);
    delay(50); // ~20 Hz, más que suficiente para PACMAN_STEP_MS=150
}