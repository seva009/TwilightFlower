#include "esp_pkt_hdl.h"
#include "pkt_io_hdl.h"
#include <EEPROM.h>
#include "eeprom/eeprom_mem_helper.h"

//esp_pkt_hdl pkt_hdl;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  EEPROM.begin(1024);

}

uint8_t* buf = (uint8_t*)calloc(512, 1);
size_t buf_sz = 0;

uint8_t stage = 0;
uint32_t ll = TRS_SIG; //idk 
packets::hello hello;
packets::cts cts;
packets::req req;
packets::resp_s resp_s;

void loop() {
  delay(500);
  if (Serial.available() > 0) {
    digitalWrite(2, HIGH);
	  switch (stage) {
      case 0:
        goto stage_0;
        delay(100);
        break;
      case 1:
        goto stage_1;
        delay(100);
        break;
      case 2:
      end_stage_1:
      delay(100);
        goto stage_2;
        delay(100);
        break;
      case 3:
        goto stage_3;
        delay(100);
        break;
      case 4:
        end_stage_3:
        delay(100);
        goto stage_4;
        delay(100);
        break;
    }
  }
  return;
stage_0:
    digitalWrite(2, HIGH);
    buf_sz = Serial.available();
    Serial.read(buf, Serial.available());
    memcpy(&cts, buf, CTS_PKT_SZ);
    memset(buf, 0, buf_sz);
    cts.type = CTS;
    delay(500);
    Serial.write((uint8_t*)&cts, CTS_PKT_SZ);
    stage++;
    return;
stage_1:
    
    Serial.write((uint8_t*)&hello, HELLO_PKT_SZ);
    stage++;
    goto end_stage_1;
    return;
stage_2:
    delay(100);
    req.type = REQ_KEY;
    Serial.write((uint8_t*)&req, REQ_PKT_SZ);
    stage++;
    return;
stage_3:
    Serial.read(buf, Serial.available());
    //spkt_hdl.handle(buf, buf_sz);
    stage++;
    goto end_stage_3;
    return;
stage_4:
    digitalWrite(LED_BUILTIN, HIGH);
    delay(600);
    resp_s.resp = STATUS_RESP_OK;
    Serial.write((uint8_t*)&resp_s, RESP_STATUS_PKT_SZ);
    return;
}
