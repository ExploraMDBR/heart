

#include "_custom_SerialMp3.h"

#define TX 11  // collegare a RX
#define RX 10  // collegare a TX
SerialMP3Player mp3(RX, TX);

//#define HEARTBEAT_0dB 0
//#define HEARTBEAT_7dB 1
#define HEARTBEAT_10dB 1
#define DRONE 2


void init_mp3() {
  mp3.begin(9600);                     // start mp3-communication
  delay(500u);                         // wait for init
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);  //select sd-card
  delay(500u);
  mp3.play(HEARTBEAT_10dB);

}

void play_beat() {
  //  long int before = millis();
  mp3.play(HEARTBEAT_10dB);
  //  Serial.println(String("Play delta:" ) + (millis()-before)+ "\n");
}

void play_wait() {
  mp3.play(DRONE);
}

void stop_sounds() {
  mp3.stop();
}
