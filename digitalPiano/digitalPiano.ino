//Digital Piano Project

#define LCD_I2C 0x27

#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);  

int Lcd_control =0;

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



void loop(){

}

