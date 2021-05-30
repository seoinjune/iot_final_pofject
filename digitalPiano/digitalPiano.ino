//Digital Piano Project

#define LCD_I2C 0x27

#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>


LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);  

int Lcd_control =0;
WiFiClient myTCPClient; //WiFiClient Class 객체=변수
PubSubClient myMQTTClient; // PubSubClient Class 객체, 변수

String stringTwo;

#define NOTE_C5 523 //도(5옥타브 음계 데이터)
#define NOTE_D5 587 //레
#define NOTE_E5 659 //미
#define NOTE_F5 698 //파
#define NOTE_G5 784 //솔
#define NOTE_A5 880 //라
#define NOTE_B5 988 //시
#define NOTE_C6 1047 //도

const byte meloytPin[] = {9,8,7,6,5,4,3,2}; //스위치 버튼
const byte tonepin = 12; //피에조 부저
const int melody[] = {NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_A5,NOTE_B5,NOTE_C6}; //도레미파솔라시도
int noteDurations = 50; //톤 길이

void setup() {
	Serial.begin(74880);
	delay(1000);
	//WiFi
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
  
	//LCD
	lcd.init();                 
	lcd.backlight();
  
	//mqtt
  
	myMQTTClient.setClient(myTCPClient);
	myMQTTClient.setServer("test.mosquitto.org",1883);
	myMQTTClient.setCallback(MQTTcb); //토픽 구독용 Callback함수 등록
  
	int mqttConnectResult = myMQTTClient.connect("IoT_DigitalPiano");
	Serial.printf("MQTT Connectrion Result : %d\r\n",mqttConnectResult);
	myMQTTClient.subscribe("MJU/IOT/DigitalPiano");

 for(int i=0; i<8; i++){
  pinMode(meloytPin[i], INPUT_PULLUP); //내부풀업스위치 지정
 }
}


void Lcd_print(){
	if(Lcd_control==0){
		lcd.setCursor(0, 0);
		lcd.print("Digital Piano");
		lcd.setCursor(0,1); 
	}
	else if(Lcd_control==1){
    
	}
}

void MQTTcb(const char topic[], byte *data, unsigned int length){
	char tempbuffer[20] = "";
	memcpy(tempbuffer, data,(length>19?19:length));
	//if(length>19) 19 else length
	Serial.printf("New data comes : %s\r\n",tempbuffer);  

	stringTwo=tempbuffer;	
}

#define DELAY_MS 2000
unsigned long long lastMs=0;//64

void loop(){
	if(millis() - lastMs >= DELAY_MS){
		lastMs= millis();
		millis(); // ESP8266이 켜지고나서 몇ms가 지났는가??

   for(int i=0; i<8; i++){ //8개 건반을 빠르게 체크
    if(digitalRead(meloytPin[1])==LOW){ //내부풀업스위치 번튼 누르면...
      tone(tonepin, meloytPin[1], noteDurations); //해당 스위치 버튼 음 출력
      delay(noteDurations); //음길이 최소
      noTone(tonepin); //음 중단
    }
   }
    
		if (stringTwo.equals("설명서")){
			myMQTTClient.publish("MJU/IOT/DigitalPiano","1.설명서입니다.");  
			delay(1500);
			myMQTTClient.publish("MJU/IOT/DigitalPiano","2.토픽입력창에 원하는 곡을 입력하시면 재생됩니다.");  
			delay(1500);
			myMQTTClient.publish("MJU/IOT/DigitalPiano","3.가능한 곡 리스트 : 비행기, 학교종, 산토끼");  
			delay(1500);
			
		}

	}
	myMQTTClient.loop();
}
