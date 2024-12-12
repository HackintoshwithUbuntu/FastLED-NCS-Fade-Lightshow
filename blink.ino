// Set the gpio pin used by esp32c3 mini as gpio pin
const int LED = 8;

void setup () {
    // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
}