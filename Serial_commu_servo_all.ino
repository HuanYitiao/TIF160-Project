#include <Arduino.h>
#include <Servo.h>

//Servos
Servo body;
Servo headPan;
Servo headTilt;
Servo shoulder;
Servo elbow;
Servo gripper;

//Init position of all servos
const int servo_pins[] = {3, 5, 6, 9, 10, 11};

const int pos_init[] = {1700, 1500, 2000, 2200, 1650, 1600};
int curr_pos[6];
int new_servo_val[6];

const int pos_min[] = {560, 550, 950, 750, 550, 550};
const int pos_max[] = {2330, 2340, 2400, 2200, 2400, 2150};

// The logic is that, the servo should not move excel the limit, 
// if the input number excel the range, it will remain the last angle

int old_ser_body = pos_init[0];
int angle_body = old_ser_body;

int old_ser_headPan = pos_init[1];
int angle_headPan = old_ser_headPan;

int old_ser_headTilt = pos_init[2];
int angle_headTilt = old_ser_headTilt;

int old_ser_shoulder = pos_init[3];
int angle_shoulder = old_ser_shoulder;

int old_ser_elbow = pos_init[4];
int angle_elbow = old_ser_elbow;

int old_ser_gripper = pos_init[5];
int angle_gripper = old_ser_gripper;

//Servo update function
void servo_body_ex(const int new_pos) {

  int diff, steps, now, CurrPwm, NewPwm, delta = 6;

  //current servo value
  now = curr_pos[0];
  CurrPwm = now;
  NewPwm = new_pos;

  /* determine interation "diff" from old to new position */
  diff = (NewPwm - CurrPwm)/abs(NewPwm - CurrPwm); // Should return +1 if NewPwm is bigger than CurrPwm, -1 otherwise.
  steps = abs(NewPwm - CurrPwm);
  delay(10);

  for (int i = 0; i < steps; i += delta) {
    now = now + delta*diff;
    body.writeMicroseconds(now);
    delay(20);
  }
  curr_pos[0] = now;
  delay(10);
}

//Servo update function
void servo_neck_pan(const int new_pos) {

  int diff, steps, now, CurrPwm, NewPwm, delta = 6;

  //current servo value
  now = curr_pos[1];
  CurrPwm = now;
  NewPwm = new_pos;

  /* determine interation "diff" from old to new position */
  diff = (NewPwm - CurrPwm)/abs(NewPwm - CurrPwm); // Should return +1 if NewPwm is bigger than CurrPwm, -1 otherwise.
  steps = abs(NewPwm - CurrPwm);
  delay(10);

  for (int i = 0; i < steps; i += delta) {
    now = now + delta*diff;
    headPan.writeMicroseconds(now);
    delay(20);
  }
  curr_pos[1] = now;
  delay(10);
}

//Servo update function
void servo_neck_tilt(const int new_pos) {

  int diff, steps, now, CurrPwm, NewPwm, delta = 6;

  //current servo value
  now = curr_pos[2];
  CurrPwm = now;
  NewPwm = new_pos;

  /* determine interation "diff" from old to new position */
  diff = (NewPwm - CurrPwm)/abs(NewPwm - CurrPwm); // Should return +1 if NewPwm is bigger than CurrPwm, -1 otherwise.
  steps = abs(NewPwm - CurrPwm);
  delay(10);

  for (int i = 0; i < steps; i += delta) {
    now = now + delta*diff;
    headTilt.writeMicroseconds(now);
    delay(20);
  }
  curr_pos[2] = now;
  delay(10);
}

//Servo update function
void servo_shoulder(const int new_pos) {

  int diff, steps, now, CurrPwm, NewPwm, delta = 6;

  //current servo value
  now = curr_pos[3];
  CurrPwm = now;
  NewPwm = new_pos;

  /* determine interation "diff" from old to new position */
  diff = (NewPwm - CurrPwm)/abs(NewPwm - CurrPwm); // Should return +1 if NewPwm is bigger than CurrPwm, -1 otherwise.
  steps = abs(NewPwm - CurrPwm);
  delay(10);

  for (int i = 0; i < steps; i += delta) {
    now = now + delta*diff;
    shoulder.writeMicroseconds(now);
    delay(20);
  }
  curr_pos[3] = now;
  delay(10);
}

//Servo update function
void servo_elbow(const int new_pos) {

  int diff, steps, now, CurrPwm, NewPwm, delta = 6;

  //current servo value
  now = curr_pos[4];
  CurrPwm = now;
  NewPwm = new_pos;

  /* determine interation "diff" from old to new position */
  diff = (NewPwm - CurrPwm)/abs(NewPwm - CurrPwm); // Should return +1 if NewPwm is bigger than CurrPwm, -1 otherwise.
  steps = abs(NewPwm - CurrPwm);
  delay(10);

  for (int i = 0; i < steps; i += delta) {
    now = now + delta*diff;
    elbow.writeMicroseconds(now);
    delay(20);
  }
  curr_pos[4] = now;
  delay(10);
}

//Servo update function
void servo_gripper_ex(const int new_pos) {

  int diff, steps, now, CurrPwm, NewPwm, delta = 6;

  //current servo value
  now = curr_pos[5];
  CurrPwm = now;
  NewPwm = new_pos;

  /* determine interation "diff" from old to new position */
  diff = (NewPwm - CurrPwm)/abs(NewPwm - CurrPwm); // Should return +1 if NewPwm is bigger than CurrPwm, -1 otherwise.
  steps = abs(NewPwm - CurrPwm);
  delay(10);

  for (int i = 0; i < steps; i += delta) {
    now = now + delta*diff;
    gripper.writeMicroseconds(now);
    delay(20);
  }
  curr_pos[5] = now;
  delay(10);
}

void setup() {

  Serial.begin(57600); // Starts the serial communication

  //Attach each joint servo
  //and write each init position
  body.attach(servo_pins[0]);
  body.writeMicroseconds(pos_init[0]);
  
  headPan.attach(servo_pins[1]);
  headPan.writeMicroseconds(pos_init[1]);
  
  headTilt.attach(servo_pins[2]);
  headTilt.writeMicroseconds(pos_init[2]);

  shoulder.attach(servo_pins[3]);
  shoulder.writeMicroseconds(pos_init[3]);

  elbow.attach(servo_pins[4]);
  elbow.writeMicroseconds(pos_init[4]);
  
  gripper.attach(servo_pins[5]);
  gripper.writeMicroseconds(pos_init[5]);

  //Initilize curr_pos and new_servo_val vectors
  byte i;
  for (i=0; i<(sizeof(pos_init)/sizeof(int)); i++){
    curr_pos[i] = pos_init[i];
    new_servo_val[i] = curr_pos[i];
  }

  delay(2000);
}

void loop() {
  if(Serial.available() > 0)
  {
      char i = Serial.read();
      int ser = Serial.parseInt();//input form example: b220
      // Serial.parseTint() 会解析出串口内的数字
      switch(i)
      {
        case 'b'://body
          if(ser<pos_min[0] || ser>pos_max[0])
          {
            angle_body = old_ser_body;
          }
          else
          {
            angle_body = ser;
            Serial.print("The angle of body servo now is ");
            Serial.println(angle_body);
            servo_body_ex(angle_body);
            old_ser_body = ser;
          }
          delay(15);
          break;

        case 'p'://headPan
          if(ser<pos_min[1] || ser>pos_max[1])
          {
            angle_headPan = old_ser_headPan;
          }
          else
          {
            angle_headPan = ser;
            Serial.print("The angle of head pan servo now is ");
            Serial.println(angle_headPan);
            servo_neck_pan(angle_headPan);
            old_ser_headPan = ser;
          }
          delay(15);
          break;  

        case 't'://headTilt
          if(ser<pos_min[2] || ser>pos_max[2])
          {
            angle_headTilt = old_ser_headTilt;
          }
          else
          {
            angle_headTilt = ser;
            Serial.print("The angle of head tilt now is ");
            Serial.println(angle_headTilt);
            servo_neck_tilt(angle_headTilt);
            old_ser_headTilt = ser;
          }
          delay(15);
          break;
          
        case 's'://shoulder
          if(ser<pos_min[3] || ser>pos_max[3])
          {
            angle_shoulder = old_ser_shoulder;
          }
          else
          {
            angle_shoulder = ser;
            Serial.print("The angle of shoulder servo now is ");
            Serial.println(angle_shoulder);
            servo_shoulder(angle_shoulder);
            old_ser_shoulder = ser;
          }
          delay(15);
          break;
          
        case 'e'://elbow
          if(ser<pos_min[4] || ser>pos_max[4])
          {
            angle_elbow = old_ser_elbow;
          }
          else
          {
            angle_elbow = ser;
            Serial.print("The angle of elbow servo now is ");
            Serial.println(angle_elbow);
            servo_elbow(angle_elbow);
            old_ser_elbow = ser;
          }
          delay(15);
          break;
          
        case 'g'://gripper
          if(ser<pos_min[5] || ser>pos_max[5])
          {
            angle_gripper = old_ser_gripper;
          }
          else
          {
            angle_gripper = ser;
            Serial.print("The angle of gripper servo now is ");
            Serial.println(angle_gripper);
            servo_gripper_ex(angle_gripper);
            old_ser_gripper = ser;
          }
          delay(15);
          break;
          
      }//switch(i)
      
      Serial.println("*********************");
      Serial.print("The angle of body servo now is ");
      Serial.println(angle_body);
      Serial.print("The angle of head pan servo now is ");
      Serial.println(angle_headPan);
      Serial.print("The angle of head tilt servo now is ");
      Serial.println(angle_headTilt);
      Serial.print("The angle of shoulder servo now is ");
      Serial.println(angle_shoulder);
      Serial.print("The angle of elbow servo now is ");
      Serial.println(angle_elbow);
      Serial.print("The angle of gripper servo now is ");
      Serial.println(angle_gripper);
      Serial.println("*********************");
           
   }//if(Serial.available() > 0)
}
