


#include "SerialMP3Player.h"
#define NO_SERIALMP3_DELAY
#define TX 11  // collegare a RX
#define RX 10  // collegare a TX
SerialMP3Player mp3(RX, TX);


void init_mp3() {
  mp3.begin(9600);                     // start mp3-communication
  delay(500u);                         // wait for init
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);  //select sd-card
  delay(500u);
  mp3.play();

}

void play_beat() {
  mp3.play();
}
