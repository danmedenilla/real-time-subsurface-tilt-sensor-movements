void init_can(){
  if (VERBOSE == 1) { Serial.println("init_can()"); }
  pinMode(50,OUTPUT);                       //Due Specific
  if (Can0.begin(40000,50)){                // also resets numBusError
    if(VERBOSE == 1) {Serial.println("Can0 - initialization completed.");}
  }
  Can0.watchFor();                          // allow all can traffic through sabi sa quick guide.
  Can0.mailbox_set_mode(0,CAN_MB_RX_MODE);  // Set mailbox0 as receiver
  Can0.mailbox_set_id(0, 0, true);          // Set mailbox0 receive extendedID formats
  Can0.mailbox_set_accept_mask(0,0,true);   // receive everything. // aralin yung mask

  Can0.mailbox_set_mode(1, CAN_MB_TX_MODE); // Set mailbox1 as transmitter
  Can0.mailbox_set_id(1,1, true);           // Set mailbox1 transfer ID to 1 extended id
}
