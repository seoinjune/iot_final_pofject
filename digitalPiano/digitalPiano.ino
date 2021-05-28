//Digital Piano Project

#include <ESP8266WiFi.h>


void setup() {
	Serial.begin(74880);
	delay(1000);
	WiFi.mode(WIFI_AP_STA);
	WiFi.begin("KT_GiGA_2G_Wave2_80B5","987654321a"); //non-block : WiFi 접속했는지 확인안해
	WiFi.setAutoReconnect(1);
	int connectResult = WiFi.waitForConnectResult(); //따라서 기다려줘야해
	while(1){
		if(connectResult == WL_CONNECTED){
		Serial.printf("Connection OK!\r\n");
		break;    }
    else{
		delay(100); 
		Serial.printf("Connection Failed, Reason=%d",connectResult);
		}
	} 
}  

void loop(){
<<<<<<< HEAD:digitalPiano/digitalPiano.ino
  
}

//확인 완료
=======
}
>>>>>>> bede5b0e3f954db97c506abe9fe984861c5f63ed:digitalPiano.ino
