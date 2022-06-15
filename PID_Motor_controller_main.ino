#include "ACS712.h"
#include <LiquidCrystal.h>
int encoder_pulse_DI_pin = 12;
int encoder_pulse_DO_pin = 13;
int PID_PWM_DO_pin = 11;
//int interruptPin = 2;

ACS712 ACS(A0, 5.0, 1023, 100);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

bool _running = false; 

class Display{
    public:
      Display(){
        lcd.begin(16,2);
        }
      void refresh_display(float sma, float smr){
        lcd.setCursor(0,0);
        lcd.print("Speed feed:");
        lcd.print(sma);
        lcd.setCursor(0,1);
        lcd.print("Speed ref:");
        lcd.print(smr);   
        }
  };

class PID: public Display{
  public:
    int _interval_pid; 
    float Kp, Ki, Kd; 
    float Pout, Iout, Dout; 
    float analogPWM; 
    float error, error_last; 
    float error_sum; 
    float error_Kd; 
    unsigned long Millis_PID;
    PID(int _int, float _p, float _i, float _d) {
      PID:: Display();
      pinMode(PID_PWM_DO_pin, OUTPUT);
      _interval_pid = _int;
      Kp = _p;
      Ki = _i;
      Kd = _d;
      analogPWM = 0.0;
      error = 0.0;
      error_last = 0.0;
      error_sum = 0.0;
      }
    float cumpute_PID(float SMR, float SMA){
      if ((millis()-Millis_PID) >= _interval_pid){
        error = SMR - SMA;
        error_sum = error_sum + ((error + error_last)*0.5);
        error_Kd = error - error_last;
  
        Pout = Kp * error;
        Iout = Ki * error_sum;
        Dout = Kd * error_Kd;
  
        if (Iout > 255){Iout = 255;}
        else if (Iout < 0){Iout = 0;}
  
        analogPWM = Pout + Iout + Dout;
   
        if (analogPWM > 255){analogPWM = 255;}
        else if (analogPWM < 0){analogPWM = 0;} 
        analogWrite(PID_PWM_DO_pin, analogPWM);
        Serial.print(SMA); Serial.print(",");
        Serial.println(SMR);

 
        error_last = error;
        PID::refresh_display(SMA, SMR);
        Millis_PID = millis();
        }
      }
  
  };

class Encoder{
  public:
    String _name;
    int _interval_e;
    int rev_encoder;
    int pulse_couter;
    float speed_encoder;
    bool pulse_trigger_once;
    unsigned long Millis_Encoder;

    Encoder(int _int){
      pinMode(encoder_pulse_DI_pin, INPUT);
      pinMode(encoder_pulse_DO_pin, OUTPUT);
      //pinMode(interruptPin, INPUT_PULLUP);
      //attachInterrupt(digitalPinToInterrupt(interruptPin), digital_pulse_counter_with_interruptions(), RISING);
      digitalWrite(encoder_pulse_DO_pin, LOW);
      _name = "27-P720R.25/200";
      pulse_trigger_once = true;
      speed_encoder = 0.0;
      pulse_couter = 0;
      rev_encoder = 200;
      _interval_e = _int;
      }
    void encoder_rotation_measurement(){
      digital_pulse_counter();
      if ((millis()-Millis_Encoder) >= _interval_e){   
        speed_encoder = (1000.0/(float)_interval_e)*(float)pulse_couter*60.0/(float)rev_encoder; //1/min
        pulse_couter = 0;
        Millis_Encoder = millis();
        }
      }
    void digital_pulse_counter(){
      if(digitalRead(encoder_pulse_DI_pin) == HIGH and pulse_trigger_once){
        pulse_couter++;
        pulse_trigger_once = !pulse_trigger_once;
        }
      else if(digitalRead(encoder_pulse_DI_pin) == LOW and !pulse_trigger_once){
        pulse_trigger_once = !pulse_trigger_once;
        }
      if (digitalRead(encoder_pulse_DI_pin) == HIGH){       
        digitalWrite(encoder_pulse_DO_pin, HIGH);
        }
      else{digitalWrite(encoder_pulse_DO_pin, LOW);}
      }
    
    void digital_pulse_counter_with_interruptions(){
        pulse_couter++;
      }
    
    float get_speed_encoder(){return speed_encoder;}
};

class Motor: public Encoder, public PID{
  public:
    String _name;
    float current;
    float voltage;
    float speed_motor_actual; //Motor Speed Feedback
    float speed_motor_reference; //Motor Speed Reference
    float gear_ratio;
    int speed_reference;
    int _interval_m;
    int _interval_r;
    unsigned long Millis_Motor_Parameters;
    unsigned long Millis_Random;

    Motor(): Encoder(300), PID(300, 3.0, 0.5, 0){
      _name = "Allied Motion 9904 120 52605 12VDC";
      _interval_m = 500;
      _interval_r = 20000; 
      speed_motor_reference = 50.0;
      gear_ratio = 1.0;
      Millis_Motor_Parameters = millis();
      Millis_Random = millis();
      
      }
    void calculate_speed_motor_actual(){
      speed_motor_actual = Encoder::get_speed_encoder()*gear_ratio;

      }
    void voltage_current_measurement(){
      if ((millis()-Millis_Motor_Parameters) >= _interval_m){              
          current = ACS.mA_DC();
          voltage = analogRead(A1) * (25/1024.0);
          Millis_Motor_Parameters = millis();
        }
      }
    void random_speed(){
          if ((millis()-Millis_Random) >= _interval_r){
            speed_motor_reference = (float)(random(20, 60));
            Millis_Random = millis();
            }
      }

    void loops(){
      random_speed();
      voltage_current_measurement();
      calculate_speed_motor_actual();
      encoder_rotation_measurement();
      cumpute_PID(speed_motor_reference, speed_motor_actual);
      }
  };
 
    
Motor m1;

void setup() {
  Serial.begin(9600);
  _running = true;
}

void loop() {
  if (_running) {m1.loops();}
}
