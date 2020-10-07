#include <Servo.h>

char h = HIGH; // simplifies HIGH to h
char l = LOW; // simplifies LOW to l
char i = INPUT; // simplifies INPUT to i
char o = OUTPUT; // simplifies OUTPUT to o

int arm_up; // pin for anticlockwise travel of overbody motor
int arm_down; // pin for clockwise travel of overbody motor
int arm_out; // pin for anticlockwise travel of underbody motor
int arm_in; // pin for clockwise travel of underbody motor
int arm_left; // pin for anticlockwise travel of base motor
int arm_right; // pin or clockwise travel of base motor
int pingPin; // pin for output sound of sensor
int inputPin; // pin for input of sound from sensor
Servo grab_servo; // declaration of servo variable
//grab_servo.attach(); // pin for controlling grab

double distance_to_control_point_1; // distance from overbody threaded bar junction with forearm to junction of bicep to forearm
double distance_to_control_point_2; // distance from underbody threaded bar junction with forearm to junction of bicep to forearm
double distance_to_tip_of_arc; // distance from junction of bicep to forearm to tip of fully extended grab
double vertical_travel; // height at which the arm needs to move
double horizontal_travel; // reach at which the arm needs to move

double base_angle; // angle of rotation of base of arm from beginning point
double motor_angle; // angle of rotation of motor for base from beginning point
int teeth_motor; // number of teeth on the motor gear
int teeth_base; // number of teeth on the base

int motor_a_clockwise; // clockwise travel control variable
int motor_a_anticlockwise; // anticlockwise travel control variable
int motor_b_clockwise; // clockwise travel control variable
int motor_b_anticlockwise; // anticlockwise travel control variable
int motor_c_clockwise; // clockwise travel control variable
int motor_c_anticlockwise; // anticlockwise travel control variable

double base_start; // base beginning position container
double base_current = base_angle; // current base angle container
double height_start; // arm height beginning position container
double height_current = height_start + vertical_travel; // current arm height container
double reach_start; // arm reach beginning position container
double reach_current = reach_start + horizontal_travel; // current arm reach container

int motor_a_start; // virtual stopper container
int motor_a_end; // virtual stopper container
int motor_b_start; // virtual stopper container
int motor_b_end; // virtual stopper container

// FUNCTIONS USED IN CONTROL OF ARM MOVEMENT //
/*Function number 1*/
double bar_travel_overbody(int vertical_travel){ /* vertical_travel must be in centimeters */
  double distance_to_control_point_a = distance_to_control_point_1;
  double distance_to_tip_of_arc_1 = distance_to_tip_of_arc;
  double rear_travel = (vertical_travel*distance_to_tip_of_arc_1)/distance_to_control_point_a; // calculates rear bar travel
  return rear_travel;
}


/*Function number 2*/
double bar_travel_underbody(int horizontal_travel){ /* Horizontal_travel must be in centimeters */
  double distance_to_control_point_b = distance_to_control_point_2;
  double distance_to_tip_of_arc_2 = distance_to_tip_of_arc;
  double forward_travel = (horizontal_travel*distance_to_tip_of_arc_2)/distance_to_control_point_b; // calculates forward bar travel
  return forward_travel;
}


/*Function number 3*/
double base_degrees(double base_angle){
  int teeth_motor = teeth_motor;
  int teeth_base = teeth_base;
  double motor_angle = base_angle*(teeth_base/teeth_motor); // calculates current motor angle
  return motor_angle;
}


/*Function number 4*/
double rotational_speed_of_base(int voltage){
  int teeth_motor = teeth_motor;
  int teeth_base = teeth_base;
  double rps = map(voltage, 0, 9, 0, 255);
  rps = map(rps, 0, 255, 0, 91);
  rps = rps/100;
  double speed_of_base = (teeth_motor/teeth_base)*rps; // calculates speed of base
  return speed_of_base;
}

double rotational_speed_of_motor(int voltage){
  double motor_speed = map(voltage, 0, 9, 0, 91); 
  motor_speed = motor_speed/100; // calculates speed of motor
  return motor_speed;
}


void ping_function(double sped){
  for(int i = 0; i < sped/3; i++){
    digitalWrite(pingPin, l);
    delayMicroseconds(sped/6);
    digitalWrite(pingPin, h);
    delayMicroseconds(sped/6);
    long double duration = pulseIn(inputPin, h);
//    double cm = microsecondstoCentimeters(duration);
    Serial.print("Distance from arm: ");
  //  serial.print(cm);
    Serial.println(" centimeters.");
  }
}


void arm_height_up(int distance /*Centimeters*/, int rpm /*In RPM*/){
  long int threads = (distance*(0/*threads per centimeter should be entered here*/));
  rpm = rpm/60;
  long double wait = (rpm*1000);
  motor_a_clockwise = 1;
  motor_a_anticlockwise = 0;
  for(int i = 0; i <= threads; i++){
    if(motor_a_clockwise == 1){
      digitalWrite(arm_out, l);
      digitalWrite(arm_in, l);
      digitalWrite(arm_down, l);
      digitalWrite(arm_up, h);
    }
    delay(wait);
  }
  digitalWrite(arm_up, l);
  motor_a_clockwise = 0;
}


void arm_height_down(int distance /*Centimeters*/, int rpm /*In RPM*/){
  long int threads = (distance*(0/*threads per centimeter should be entered here*/));
  rpm = rpm/60;
  long double wait = (rpm*1000);
  motor_a_clockwise = 0;
  motor_a_anticlockwise = 1;
  for(int i = 0; i <= threads; i++){
    if(motor_a_anticlockwise == 1){
      digitalWrite(arm_out, l);
      digitalWrite(arm_in, l);
      digitalWrite(arm_down, h);
      digitalWrite(arm_up, l);
    }
    delay(wait);
  }
  digitalWrite(arm_down, l);
  motor_a_anticlockwise = 0;
}


void arm_reach_out(int distance /*Centimeters*/, int rpm /*In RPM*/){
  long int threads = (distance*(0/*threads per centimeter should be entered here*/));
  rpm = rpm/60;
  long double wait = (rpm*1000);
  motor_b_clockwise = 1;
  motor_b_anticlockwise = 0;
  for(int i = 0; i <= threads; i++){
    if(motor_b_clockwise == 1){
      digitalWrite(arm_out, h);
      digitalWrite(arm_in, l);
      digitalWrite(arm_down, l);
      digitalWrite(arm_up, l);
    }
    delay(wait);
  }
  digitalWrite(arm_out, l);
  motor_b_clockwise = 0;
}


void arm_reach_in(int distance /*Centimeters*/, int rpm /*In RPM*/){
  long int threads = (distance*(0/*threads per centimeter should be entered here*/));
  rpm = rpm/60;
  long double wait = (rpm*1000);
  motor_b_clockwise = 0;
  motor_b_anticlockwise = 1;
  for(int i = 0; i <= threads; i++){
    if(motor_b_anticlockwise == 1){
      digitalWrite(arm_out, l);
      digitalWrite(arm_in, h);
      digitalWrite(arm_down, l);
      digitalWrite(arm_up, l);
    }
    delay(wait);
  }
  digitalWrite(arm_in, l);
  motor_b_anticlockwise = 0;
}


void arm_turn_right(int DEG /*Degrees of rotation required*/, double rpm /*In RPM*/){
  int speed = map(rpm, 0, 54, 0, 255);
  int deg_per_sec = rpm/60;
  deg_per_sec = deg_per_sec/360;
  motor_c_clockwise = 1;
  motor_c_anticlockwise = 0;
  for(double i = base_current; i < DEG; i+deg_per_sec){
    analogWrite(arm_right, speed);
    digitalWrite(arm_left, l);
    delay(1000);
  }
  motor_c_clockwise = 0;
  digitalWrite(arm_right, l);
  base_current++;
}


void arm_turn_left(int DEG /*Degrees of rotation required*/, double rpm /*In RPM*/){
  int speed = map(rpm, 0, 54, 0, 255);
  int deg_per_sec = rpm/60;
  deg_per_sec = deg_per_sec/360;
  motor_c_clockwise = 0;
  motor_c_anticlockwise = 1;
  for(int i = base_current; i > DEG; i-deg_per_sec){
    analogWrite(arm_left, speed);
    digitalWrite(arm_right, l);
    delay(1000);
  }
  motor_c_anticlockwise = 0;
  digitalWrite(arm_left, l);
  base_current--;
}

void setup(){}
void loop(){}
