#include <due_can.h>
#define VERBOSE 1
#define LED1 48
#define RELAYPIN 44
#define BAUDRATE 9600
int delay1 = 800;
CAN_FRAME can_buffer[100];
int expected_frame = 2;
char temp_t[1000];

int sensor_version = 41; 
//v5 = 51/52?, v4 = 41, v3 = 11

void setup() {
  
  // put your setup code here, to run once:
  init_can();
  Serial.begin(BAUDRATE);
  pinMode (LED1, OUTPUT);
  pinMode(RELAYPIN, OUTPUT);
  delay(5000);
}

void loop() {

  String serial_line;
  get_data();
  digitalWrite(LED1,LOW);
  //delay(delay1);
  clear_can_buffer(can_buffer);
  Serial.println(temp_t);
  temp_t[0] = '\0';

  if (Serial.available()) {
    do{
      serial_line = Serial.readStringUntil('\r\n');
        if (serial_line.toInt() != -1) {
        delay1 = serial_line.toInt();
        }
    } while(serial_line == "");
  }

}

void ping() {
  String serial_line;
  serial_line = Serial.readString();
  if(serial_line == "ping") {
    get_data();
    digitalWrite(LED1,LOW);
    //delay(delay1);
    clear_can_buffer(can_buffer);
    Serial.println(temp_t);
    temp_t[0] = '\0'; 
    Serial.println("test");
  }
 
}
       

void turn_on_column(){
  digitalWrite(RELAYPIN, HIGH);
  digitalWrite(LED1,HIGH);
}
void turn_off_column(){
  digitalWrite(RELAYPIN, LOW);
  digitalWrite(LED1, LOW);
}

int compute_axis(int low, int high){
  int value = 5000;
  if (high >= 240) {
    high = high - 240;
    value = (low + (high*256)) - 4095;
  } else {
    value = (low + (high*256));
  } 
  return value;
  }

int get_all_frames(CAN_FRAME can_buffer[]) {
  int timestart = millis();
  int a = 0, i = 0;
  CAN_FRAME incoming;
  do {
    //check_can_status();
    if (Can0.available()){
      Can0.read(incoming);
      can_buffer[i].id = incoming.id;
      can_buffer[i].data.byte[0] = incoming.data.byte[0];
      can_buffer[i].data.byte[1] = incoming.data.byte[1];
      can_buffer[i].data.byte[2] = incoming.data.byte[2];
      can_buffer[i].data.byte[3] = incoming.data.byte[3];
      can_buffer[i].data.byte[4] = incoming.data.byte[4];
      can_buffer[i].data.byte[5] = incoming.data.byte[5];
      can_buffer[i].data.byte[6] = incoming.data.byte[6];
      can_buffer[i].data.byte[7] = incoming.data.byte[7];
      i++;
      
      interpret_frame(incoming);
    }
  } while (millis() - timestart <= delay1);                          
} 

void interpret_frame(CAN_FRAME incoming){
  int id,d1,d2,d3,d4,d5,d6,d7,d8,x,y,z;
  char temp[6];

  id = incoming.id;
  d1 = incoming.data.byte[0];
  d2 = incoming.data.byte[1];
  d3 = incoming.data.byte[2];
  d4 = incoming.data.byte[3];
  d5 = incoming.data.byte[4];
  d6 = incoming.data.byte[5];
  d7 = incoming.data.byte[6];
  d8 = incoming.data.byte[7];

  x = compute_axis(d2,d3);
  y = compute_axis(d4,d5);
  z = compute_axis(d6,d7);
    
/*  
  Serial.print(id);
  Serial.print(",");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.print(z);
  Serial.println("");
*/
  sprintf(temp, "%d", x);
  strncat(temp_t,temp,strlen(temp)); 
  strncat(temp_t," ",1);
  sprintf(temp, "%d", y);
  strncat(temp_t,temp,strlen(temp)); 
  strncat(temp_t," ",1);
  sprintf(temp, "%d", z);
  strncat(temp_t,temp,strlen(temp)); 
  strncat(temp_t," ",1);

}

void check_can_status(){
  int rx_error_cnt=0,tx_error_cnt =0;
  rx_error_cnt = Can0.get_rx_error_cnt();
  tx_error_cnt = Can0.get_tx_error_cnt();
  if (rx_error_cnt + tx_error_cnt != 0){ 
    if (VERBOSE){
      Serial.print("rx_error : ");
      Serial.print(rx_error_cnt);
      Serial.print("\t tx_error :");
      Serial.println(tx_error_cnt);
    }
  }
  return;
}

void clear_can_buffer(CAN_FRAME can_buffer[]){
  for(int i = 0; i< 100; i++){
    can_buffer[i] = {};
  }
}

void get_data(){
  int count,uid,ic;
  turn_on_column();
  send_command(sensor_version,1);
  get_all_frames(can_buffer);
  turn_off_column();
  
}

void send_command(int command,int transmit_id){
  CAN_FRAME outgoing;
  outgoing.extended = true;
  outgoing.id = transmit_id;
  outgoing.length = 1;
  outgoing.data.byte[0] = command;
  Can0.sendFrame(outgoing);
}

int count_frames(CAN_FRAME can_buffer[]){
  int i = 0,count = 0;
  for (i=0; i< 100; i++){
    if (can_buffer[i].id != 0){
        count++;
    }
  }
  return count;
}
void process_all_frames(CAN_FRAME can_buffer[]){
  int count,i;
  delete_repeating_frames(can_buffer);
}
void delete_repeating_frames(CAN_FRAME can_buffer[]){
  int i0 = 0, i1 = 0;
  int frame_count,i;
  frame_count = count_frames(can_buffer);
  for (i0 = 0; i0 < frame_count; i0++){
    for (i1 = 0; i1 < frame_count; i1++) {
      if ( (can_buffer[i0].id == can_buffer[i1].id) && (i0 != i1) ){
        can_buffer[i1] = {}; // clears the CAN_FRAME struct
      }
    }
  }
}
