//Digital Piano Project

#define LCD_I2C 0x27

#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);  

int Lcd_control =0;
WiFiClient myTCPClient; //WiFiClient Class 객체=변수
PubSubClient myMQTTClient; // PubSubClient Class 객체, 변수
ESP8266WebServer myHttpServer(80);

String stringTwo;

#define NOTE_C5 2093 //도(7옥타브 음계 데이터)
#define NOTE_D5 2349 //레
#define NOTE_E5 2637 //미
#define NOTE_F5 2793 //파
#define NOTE_G5 3135 //솔
#define NOTE_A5 3520 //라
#define NOTE_B5 3951 //시

byte b[] = {B10001,B10001,B10001,B11111,B10001,B10001,B10001,B11111}; 
byte bi[] = {B00100,B00100,B00100,B00100,B00100,B00100,B00100,B00100};
byte h[] = {B00100,B00100,B11111,B00100,B01010,B10001,B01010,B00100}; 
byte han[] = {B10001,B10001,B10001,B11111,B10001,B10001,B10001,B10001};
byte hang[] = {B00100,B01010,B10001,B10001,B01010,B00100,B00000,B00000};
byte k[] = {B11111,B00001,B00001,B00001,B00001,B00001,B00001,B00001};
byte note[] = {B00100,B00110,B00101,B00110,B00101,B00100,B01110,B00100};

byte a[] = {B10000,B10000,B10000,B11100,B10000,B10000,B10000,B10000};//ㅏ
byte ko[] = {B11111,B00001,B00001,B00001,B00000,B01010,B01010,B11111};//교
byte jo[] = {B11111,B00100,B01010,B10001,B00100,B00100,B11111,B00000};//조

char buffer[100] = {0};

const int meloytPin[] = {10,3,13,12,14,2,0}; //스위치 버튼
const int meloyttone[] = {NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_A5,NOTE_B5};
const int tonepin = 15; //피에조 부저
int noteDurations = 50; //톤 길이

String IPaddress;

int schoolbell[] = {NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,NOTE_G5,NOTE_E5,0,
                    NOTE_G5,NOTE_G5,NOTE_E5,NOTE_E5,NOTE_D5,0,0,
                    NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,NOTE_G5,NOTE_E5,0,
                    NOTE_G5,NOTE_E5,NOTE_D5,NOTE_E5,NOTE_C5}; //솔솔라라솔솔미 솔솔미미레 솔솔라라솔솔미 솔미레미도
int airplane[] = {NOTE_A5,NOTE_G5,NOTE_F5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_A5,0,
                  NOTE_G5,NOTE_G5,NOTE_G5,0,NOTE_A5,NOTE_A5,NOTE_A5,0,
                  NOTE_A5,NOTE_G5,NOTE_F5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_A5,0,
                  NOTE_G5,NOTE_G5,NOTE_A5,NOTE_G5,NOTE_F5}; //라솔파솔라라라 솔솔솔라라라 라솔파솔라라라 솔솔라솔파


void fnRoot(void){

}


void fnInput(void){

}


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
		
		sprintf(buffer, "Site %s\n", WiFi.localIP().toString().c_str());
		Serial.println(buffer);
		break;    }
		else{
			delay(100); 
			Serial.printf("Connection Failed, Reason=%d",connectResult);
		}
	} 
  
	//LCD
	lcd.init();                 
	lcd.backlight();
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	
	//mqtt
  
	myMQTTClient.setClient(myTCPClient);
	myMQTTClient.setServer("test.mosquitto.org",1883);
	myMQTTClient.setCallback(MQTTcb); //토픽 구독용 Callback함수 등록
  
	int mqttConnectResult = myMQTTClient.connect("IoT_DigitalPiano");
	Serial.printf("MQTT Connectrion Result : %d\r\n",mqttConnectResult);
	myMQTTClient.subscribe("MJU/IOT/DigitalPiano");

	//HTTP
	
	myHttpServer.on("/",fnRoot);
    myHttpServer.on("/input",fnInput);
	myHttpServer.begin();

	pinMode(tonepin, OUTPUT);
	pinMode(meloytPin[0], INPUT_PULLUP);
	pinMode(meloytPin[1], INPUT_PULLUP);
	pinMode(meloytPin[2], INPUT_PULLUP);
	pinMode(meloytPin[3], INPUT_PULLUP);
	pinMode(meloytPin[4], INPUT_PULLUP);
	pinMode(meloytPin[5], INPUT_PULLUP);
	pinMode(meloytPin[6], INPUT_PULLUP);
}


void Lcd_print(){
	if(Lcd_control==0){
		lcd.init();
		lcd.setCursor(1, 0);
		lcd.print("Digital Piano");
		lcd.setCursor(0, 1);
		lcd.print(buffer);
	}
	else if(Lcd_control==1){
		lcd.init();      
		lcd.createChar(0,b); lcd.createChar(1,bi); //비
		lcd.createChar(2,h); lcd.createChar(3,han); lcd.createChar(4,hang); //행
		lcd.createChar(5,k); //ㄱ
		lcd.createChar(6,note);	//음표
		delay(50);
		lcd.setCursor(3, 0); lcd.write(0); //ㅂ
		lcd.setCursor(4, 0); lcd.write(1); //ㅣ  
		lcd.setCursor(5, 0); lcd.write(2); //ㅎ  
		lcd.setCursor(6, 0); lcd.write(3); //ㅐ
		lcd.setCursor(5, 1); lcd.write(4); //ㅇ  
		lcd.setCursor(7, 0); lcd.write(5); //ㄱ 
		lcd.setCursor(8, 0); lcd.write(1); //ㅣ      
		lcd.setCursor(9, 0); lcd.write(6); //음표   
		lcd.setCursor(10, 0); lcd.write(6); //음표      
		lcd.setCursor(11, 0); lcd.write(6); //음표 		
	}
	else if(Lcd_control==2){
		lcd.init();    
		lcd.createChar(3,han);
		lcd.createChar(5,k); //ㄱ
		lcd.createChar(6,note);	//음표
		lcd.createChar(7,a);  //ㅏ
		lcd.createChar(8,ko); //교
		lcd.createChar(9,jo); //조
		delay(50);
		lcd.setCursor(3, 0); lcd.write(2); //ㅎ
		lcd.setCursor(4, 0); lcd.write(7); //ㅏ
		lcd.setCursor(3, 1); lcd.write(5); //ㄱ
		lcd.setCursor(5, 0); lcd.write(8); //교
		lcd.setCursor(6, 0); lcd.write(9); //조
		lcd.setCursor(6, 1); lcd.write(4); //ㅇ
		lcd.setCursor(7, 0); lcd.write(6);
		lcd.setCursor(8, 0); lcd.write(6);
		lcd.setCursor(9, 0); lcd.write(6);
    }
}

void MQTTcb(const char topic[], byte *data, unsigned int length){
	char tempbuffer[20] = "";
	memcpy(tempbuffer, data,(length>19?19:length));
	//if(length>19) 19 else length
	Serial.printf("New data comes : %s\r\n",tempbuffer);  

	stringTwo=tempbuffer;	
}


void play_sing(){
	for(int t=0; t<7; t++){ //7개 건반을 빠르게 체크
		if(digitalRead(meloytPin[t])==LOW){ //내부풀업스위치 번튼 누르면...
			Serial.printf("%d 입력!\r\n",meloyttone[t]);
			tone(tonepin, meloyttone[t], noteDurations); //해당 스위치 버튼 음 출력
			delay(noteDurations); //음길이 최소
			noTone(tonepin); //음 중단
		}
	}	
	if (stringTwo.equals("비행기")){
		Lcd_control = 1;
		myMQTTClient.publish("MJU/IOT/DigitalPiano","재생중...!"); 
		Lcd_print();
		for (int i=0; i< sizeof(airplane)/sizeof(airplane[0]);i++){
			tone(tonepin, airplane[i], noteDurations); //해당 스위치 버튼 음 출력
			delay(500); //음길이 최소
			noTone(tonepin); //음 중단
			delay(100);
		}
		myMQTTClient.publish("MJU/IOT/DigitalPiano","노래 끝!"); 	
	}
	else if(stringTwo.equals("학교종")){
		Lcd_control = 2;
		myMQTTClient.publish("MJU/IOT/DigitalPiano","재생중...!"); 
		Lcd_print();
		for (int i=0; i< sizeof(schoolbell)/sizeof(schoolbell[0]);i++){
			tone(tonepin, schoolbell[i], noteDurations); //해당 스위치 버튼 음 출력
			delay(500); //음길이 최소
			noTone(tonepin); //음 중단
			delay(100);
		}
		myMQTTClient.publish("MJU/IOT/DigitalPiano","노래 끝!"); 	
	}

}

#define DELAY_MS 1500
unsigned long long lastMs=0;//64

void loop(){
	if(millis() - lastMs >= DELAY_MS){
		lastMs= millis();
		millis(); // ESP8266이 켜지고나서 몇ms가 지났는가??    
		if (stringTwo.equals("설명서")){
			myMQTTClient.publish("MJU/IOT/DigitalPiano","1.설명서입니다.");  
			delay(DELAY_MS);
			myMQTTClient.publish("MJU/IOT/DigitalPiano","2.토픽입력창에 원하는 곡을 입력하시면 재생됩니다.");  
			delay(DELAY_MS);
			myMQTTClient.publish("MJU/IOT/DigitalPiano","3.가능한 곡 리스트 : 비행기, 학교종");  
			delay(DELAY_MS);		
		}

	}
	
	Lcd_control=0;
	play_sing();
	Lcd_print();
	myMQTTClient.loop();
	myHttpServer.handleClient();
}
