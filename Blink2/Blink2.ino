// Set the correct led builtin gpio pin for esp32c3 supermini
const int LED = 8;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(250);                      // wait for a quarter second
  digitalWrite(LED, LOW);   // turn the LED off by making the voltage LOW
  delay(250);                      // wait for a quarter second
}
