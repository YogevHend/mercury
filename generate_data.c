volatile boolean f_US;
volatile long start_time;
volatile long stop_time;

double g_dist;

void generate_40kHz_pulse() {
  cli();
  PORTB = PORTB & B11111011;      // digitalWrite(10, LOW);  // SWITCH
  delayMicroseconds(25);

  start_time = micros();

  PORTB |= 1;
  delayMicroseconds(12);
  for (int i = 0; i < 30; i++) {
    PORTB ^= 3;
    delayMicroseconds(12);
  }
  
  PORTB &= B11111100;

  delayMicroseconds(25);

  PORTB = PORTB | B00000100; // digitalWrite(10, HIGH);  // SWITCH
  sei();
}

void IRQ_handler() {
  if (f_US == false) {            // In this sample code, just take 1st reflection.
    stop_time = micros();
    if(stop_time - start_time > 2560) {
      g_dist = (stop_time - start_time) * 340 / 2 / 1000; //unit : mm
      f_US = true;
    }
  }
}

void setup() {

  Serial.begin(9600);
  //!!!
  attachInterrupt(0, IRQ_handler, RISING);
}

void print_shock_data(int shock_index, int time_when_received, int shock_value){
  Serial.print("SHOCK DATA: ");
  Serial.print(shock_index, DEC);
  Serial.print(",");
  Serial.print(time_when_received, DEC);
  Serial.print(",");
  Serial.print(shock_value, DEC);
  Serial.print("\n");
}

void print_ultrasonic_data(int ultrasonic_index, int time_when_receieved, int ultrasonic_value){
  Serial.print("ULTRASONIC DATA:");
  Serial.print(ultrasonic_index, DEC);
  Serial.print(",");
  Serial.print(time_when_receieved, DEC);
  Serial.print(",");
  Serial.print(ultrasonic_value, DEC);
  Serial.print("\n"); 
}

int get_shock_data(){
  return analogRead(0);
}

double get_ultrasound_data(){
  if(f_US){
    return g_dist;
  }
}
void loop() {
    int ultrasonic_index = 0;
    int shock_index = 0;
    int new_millis = 0;
    int new_ultrasound_millis = 0;
    int new_shock_millis = 0;
    int last_shock_millis = 0;
    int last_ultrasound_millis = 0;
    while(true){
      // put your main code here, to run repeatedly:
      // index of shock, millis, value
      new_millis = millis();
      new_ultrasound_millis = new_millis;
      new_shock_millis = new_millis;
      if(new_shock_millis - last_shock_millis > 10){
        last_shock_millis = new_shock_millis;
        print_shock_data(shock_index, new_millis, get_shock_data());
        shock_index++;
      }
      if(new_ultrasound_millis - last_ultrasound_millis > 15){
        last_ultrasound_millis = new_ultrasound_millis;
        print_ultrasonic_data(ultrasonic_index, new_millis, get_ultrasound_data());
        generate_40kHz_pulse();
        ultrasonic_index++;
      }
      delay(5);
    }
}
