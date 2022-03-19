#include <Arduino.h>
#line 1 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"

#include <Adafruit_GFX.h>	 // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

/* LIBRARY ARDUINO DEFINE  */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> /* Get data Weather - http */
#include <ESP8266httpUpdate.h> /* Updated OTA */
#include <time.h>
#include <EEPROM.h>
#include <ArduinoJson.h> /* ARDUINOJSON_VERSION "5.13.5" */

#define TFT_RST -1
#define TFT_CS D4
#define TFT_DC D6
#define TFT_MOSI D7 // Data out
#define TFT_SCLK D5 // Clock out

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

/* USER DEFINE  */
#include "Macro_define.h" /* LCD2004 - CHARACTER LCD */

#line 26 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void setup(void);
#line 145 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void loop();
#line 154 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void testdrawtext(int x, int y, char *text, int z, uint16_t color);
#line 164 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void Check_OTA_Weekly();
#line 183 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
bool bool_Test_Wifi(void);
#line 242 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void update_FOTA();
#line 347 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void Setup_Localtime_NTP();
#line 403 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void Load_Local_RealTime();
#line 575 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void Weather_Online_sever();
#line 668 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void Call_Weather_Every_10Min();
#line 26 "d:\\Git_NDB\\SmartClock_OLED\\Smartclock_oled\\Smartclock_oled.ino"
void setup(void)
{
	Serial.begin(115200);
	tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
	Serial.println(F("Initialized"));
	tft.fillScreen(ST77XX_BLACK);
	tft.drawBitmap(0, 0, wifi, 64, 64, ST77XX_CYAN);
	tft.drawBitmap(64, 0, alarm, 64, 64, ST77XX_CYAN);
	tft.drawBitmap(0, 64, ota, 64, 64, ST77XX_CYAN);
	tft.drawBitmap(64, 64, weather, 64, 64, ST77XX_CYAN);
	delay(2000);
	tft.fillScreen(ST77XX_BLACK);
	testdrawtext(0, 0, "SmartClock", 2, ST77XX_YELLOW);

	/* Initialize eeprom */
	EEPROM.begin(512);
	Serial.println("\n\n>>>>>>>>>> Read EEPROM \n");
	Serial.printf("Status Alarm: %d (1: ON - 0: OFF) \n", status_Mode_Alarm);
	/* Lấy giờ hẹn từ EEPROM */
	hen_gio = EEPROM.read(index_eeprom_hengio);
	Serial.printf("Var hen_gio: %d \n", hen_gio);

	/* Lấy phút hẹn từ EEPROM */
	hen_phut = EEPROM.read(index_eeprom_henphut);
	Serial.printf("Var hen_phut: %d \n", hen_phut);

	/* Lấy vị trí từ EEPROM */
	value_Location_EEPROM = EEPROM.read(index_eeprom_location_eeprom);
	Serial.printf("Var value_Location_EEPROM: %d \n\n", value_Location_EEPROM);

	Serial.println(">>>>>>>>>> READ SSID PASS EEROM  \n");
	testdrawtext(0, 30, "->>Found saved wifi.", 1, ST77XX_GREEN);

	/* read data Wifi from eeprom */
	/* for ssid and pass */
	Serial.println("Reading EEPROM ssid ......");
	String esid = "";
	for (int i = 0; i < index_eeprom_SSID; ++i)
	{
		esid += char(EEPROM.read(i));
	}
	Serial.print(">>>>> SSID: ");
	Serial.println(esid);

	tft.setCursor(5, 40);
	tft.println("ID Wifi:");
	tft.setCursor(10, 50);
	tft.println(esid);

	Serial.println("\nReading EEPROM pass ......");
	String epass = "";
	for (int i = 32; i < index_eeprom_PASS; ++i)
	{
		epass += char(EEPROM.read(i));
	}
	Serial.print(">>>>> PASS: ");
	Serial.println(epass);

	tft.setCursor(5, 60);
	tft.println("PASS Wifi:");
	tft.setCursor(10, 70);
	tft.println(epass);
	tft.setCursor(5, 80);
	tft.print("-------------------");
	/* nho check lai dieu kien cho nay khi < 1 */
	if (esid.length() > ESP_NB_ONE)
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(esid.c_str(), epass.c_str());

		if (bool_Test_Wifi()) /* Nếu kết nối được wifi */
		{
			Serial.println("");
			Serial.println("Wifi ConnectionSuccessful!!!");

			tft.setCursor(5, 100);
			tft.print("ConnectionSuccessful");
			tft.setCursor(5, 110);
			tft.print("Start connecting!");
		}
		else //! KHÔNG THỂ KẾT NỐI
		{
			tft.setCursor(5, 100);
			tft.print("WL_CONNECT_FAILED");
			tft.setCursor(5, 110);
			tft.println("Pls Open EspTouch");
			/* vào tìm kết nối wifi */
			Serial.println("");
			Serial.println("Connect timed out, opening AP");
			Serial.println("WL_CONNECT_FAILED !!!");
			Serial.println("Yêu cầu sử dụng App EspTouch !!!");
			// smartConfig_ndb();
		}
	}
	/* nếu SSID nhỏ hơn 0 (tên wifi không đúng)... thì Yêu cầu sử dụng App EspTouch */
	else
	{
		tft.setCursor(5, 100);
		tft.print("WL_CONNECT_FAILED");
		tft.setCursor(5, 110);
		tft.println("Pls Open EspTouch");
		Serial.println("Tên Wifi không đúng hoặc không hợp lệ !!!");
		Serial.println("Yêu cầu sử dụng App EspTouch !!!");
		// smartConfig_ndb();
	}
	WiFi.mode(WIFI_STA);
	Serial.println("");
	WiFi.printDiag(Serial);

	/* Check firmware coi có cập nhật không?  */
	update_FOTA();

	/* Cập nhật thời gian từ sever vn.pool.ntp.org */
	Setup_Localtime_NTP();

	/* Cập nhật thời gian từ sever vn.pool.ntp.org */
	Weather_Online_sever();
}

void loop()
{
	/* Check thời tiết trươc khi load màn hình */
	Call_Weather_Every_10Min();
	/* Load thời gian màn hình */
	Load_Local_RealTime();
	Check_OTA_Weekly();
}

void testdrawtext(int x, int y, char *text, int z, uint16_t color)
{
	tft.setCursor(x, y);
	tft.setTextSize(z);
	tft.setTextColor(color);
	tft.setTextWrap(true);
	tft.print(text);
}

/* Check cập nhật OTA mỗi tuàn */
void Check_OTA_Weekly()
{
	if ((thu == 2) && (gio == 1) && (phut == 0) && (giay < 5)) // 10 phut/ lan
	{
		update_FOTA();
	}
}

/*
 ███████████                   █████       █████   ███   █████  ███     ██████   ███
░█░░░███░░░█                  ░░███       ░░███   ░███  ░░███  ░░░     ███░░███ ░░░
░   ░███  ░   ██████   █████  ███████      ░███   ░███   ░███  ████   ░███ ░░░  ████
	░███     ███░░███ ███░░  ░░░███░       ░███   ░███   ░███ ░░███  ███████   ░░███
	░███    ░███████ ░░█████   ░███        ░░███  █████  ███   ░███ ░░░███░     ░███
	░███    ░███░░░   ░░░░███  ░███ ███     ░░░█████░█████░    ░███   ░███      ░███
	█████   ░░██████  ██████   ░░█████        ░░███ ░░███      █████  █████     █████
   ░░░░░     ░░░░░░  ░░░░░░     ░░░░░          ░░░   ░░░      ░░░░░  ░░░░░     ░░░░░

*/
bool bool_Test_Wifi(void)
{
	int c = 0;
	Serial.println("");
	Serial.println("Waiting for Wifi to connect");
	Serial.println("=========  Note =========");
	Serial.println("WL_NO_SHIELD        = 255");
	Serial.println("WL_IDLE_STATUS      = 0");
	Serial.println("WL_NO_SSID_AVAIL    = 1");
	Serial.println("WL_SCAN_COMPLETED   = 2");
	Serial.println("WL_CONNECTED        = 3");
	Serial.println("WL_CONNECT_FAILED   = 4");
	Serial.println("WL_CONNECTION_LOST  = 5");
	Serial.println("WL_WRONG_PASSWORD   = 6");
	Serial.println("WL_DISCONNECTED     = 7");
	Serial.println("========================");
	while (c < 40)
	{
		tft.setCursor(5, 90);
		tft.setTextColor(ST7735_BLUE);
		tft.print("Checking Wifi ...");
		delay(100);
		tft.setCursor(5, 90);
		tft.setTextColor(ST7735_CYAN);
		tft.print("Checking Wifi ...");
		delay(100);
		tft.setCursor(5, 90);
		tft.setTextColor(ST7735_ORANGE);
		tft.print("Checking Wifi ...");
		delay(100);
		if (WiFi.status() == WL_CONNECTED)
		{
			/* Chuông báo ok */
			digitalWrite(PIN_signal_Bell, ESP_NB_ON);
			delay(300);
			digitalWrite(PIN_signal_Bell, ESP_NB_OFF);
			return true;
		}

		Serial.print(WiFi.status());
		Serial.print(" -> ");
		c++;
	}
	return false;
}

/* Cập nhật OTA
 █████  █████               █████            █████                   ███████    ███████████   █████████
░░███  ░░███               ░░███            ░░███                  ███░░░░░███ ░█░░░███░░░█  ███░░░░░███
 ░███   ░███  ████████   ███████   ██████   ███████    ██████     ███     ░░███░   ░███  ░  ░███    ░███
 ░███   ░███ ░░███░░███ ███░░███  ░░░░░███ ░░░███░    ███░░███   ░███      ░███    ░███     ░███████████
 ░███   ░███  ░███ ░███░███ ░███   ███████   ░███    ░███████    ░███      ░███    ░███     ░███░░░░░███
 ░███   ░███  ░███ ░███░███ ░███  ███░░███   ░███ ███░███░░░     ░░███     ███     ░███     ░███    ░███
 ░░████████   ░███████ ░░████████░░████████  ░░█████ ░░██████     ░░░███████░      █████    █████   █████
  ░░░░░░░░    ░███░░░   ░░░░░░░░  ░░░░░░░░    ░░░░░   ░░░░░░        ░░░░░░░       ░░░░░    ░░░░░   ░░░░░
			  ░███
			  █████
			 ░░░░░
*/
void update_FOTA()
{
	Serial.println("\n>>>>>>>>>>> Update FOTA \n");
	Serial.println("Check firmware coi có bản cập nhật không? ");

	tft.fillScreen(ST77XX_BLACK);
	tft.drawBitmap(32, 0, ntp, 64, 64, ST77XX_CYAN);
	testdrawtext(5, 65, "Update OTA", 2, ST77XX_YELLOW);
	tft.setTextSize(1);

	/* màn hình hiển thị trên LCD 2004 */
	tft.setCursor(0, 85);
	tft.print("Version Firmware: \nv");
	tft.print(Version);
	tft.print(" - ");
	tft.println(CHIPID);
	tft.printf(">>> Device: %d MHz \n", ESP.getCpuFreqMHz());
	tft.printf(">>> Free mem: %d \n", ESP.getFreeHeap());
	delay(1500);
	/* hiển thị loading . . . */
	Serial.printf(">>> Device: %d MHz \n", ESP.getCpuFreqMHz());
	Serial.printf(">>> Version Firmware: v%s (OTADrive) \n", Version);
	Serial.printf(">>> ID ESP: ");
	Serial.println(CHIPID);
	Serial.printf(">>> Boot Mode: %d \n", ESP.getBootMode());
	Serial.printf(">>> Free mem: %d \n", ESP.getFreeHeap());
	/* biến Check_OTA kiểm tra có coi bản cập nhật OTA nào hay không? */
	bool Check_OTA = true;
	int count_Check_OTA = 0;
	while (Check_OTA)
	{
		/* sever chưa tệp BIN */
		String url = "http://otadrive.com/DeviceApi/update?";
		WiFiClient client;
		url += "&s=" + String(CHIPID);
		url += MakeFirmwareInfo(ProductKey, Version);

		t_httpUpdate_return ret = ESPhttpUpdate.update(client, url, Version);

		switch (ret)
		{
		case HTTP_UPDATE_FAILED:
			count_Check_OTA++;
			Check_OTA = true;
			Serial.println(">>> Please waiting ...");
			// clear một phần màn hình
			tft.fillRect(0, 85, 128, 128 - 85, ST7735_BLACK);
			tft.setCursor(0, 85);
			tft.setTextSize(1);
			tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
			tft.println(">>> Please waiting...");
			tft.setCursor(50, 95);
			tft.setTextSize(3);
			tft.print((51 - count_Check_OTA) / 10 % 10);
			tft.print((51 - count_Check_OTA) / 1 % 10);
			if (count_Check_OTA > 50)
			{
				Check_OTA = false;
				Serial.println(">>> Sever OTADrive bị nghẻn, quá tải...");
				Serial.println(">>> Hoặc thiết bị của bạn chưa được cho phép cập nhật trên hệ thống...");
				Serial.println(">>> Check cập nhật ở thời điểm khác...");
				Serial.printf(">>> Phiên bản hiện tại là v%s \n", Version);
				tft.fillRect(0, 85, 128, 128 - 85, ST7735_BLACK);
				tft.setCursor(0, 85);
				tft.setTextSize(1);
				tft.println(">>> Skip updated...");
				delay(2000);
			}
			break;

		case HTTP_UPDATE_NO_UPDATES:
			Check_OTA = false;
			Serial.println(">>> HTTP_UPDATE_NO_UPDATES");
			Serial.println(">>> The current version is the latest.");
			// clear một phần màn hình
			tft.fillRect(0, 85, 128, 128 - 85, ST7735_BLACK);
			tft.setCursor(0, 85);
			tft.setTextColor(ST7735_GREEN);
			tft.print("Version Firmware: \nv");
			tft.print(Version);
			tft.print(" - ");
			tft.println(CHIPID);
			tft.print("\nThe current version  is the latest.");
			delay(1000);
			break;

		case HTTP_UPDATE_OK:
			Serial.println(">>> HTTP_UPDATE_OK"); // may not called we reboot the ESP
			Check_OTA = false;
			break;
		}
	}
	Serial.println("\n<<<<<<<<<< Done Check FOTA \n");
	delay(1000);
	tft.fillScreen(ST77XX_BLACK);
}

/*
██╗      ██████╗  ██████╗ █████╗ ██╗  ████████╗██╗███╗   ███╗███████╗    ███╗   ██╗████████╗██████╗
██║     ██╔═══██╗██╔════╝██╔══██╗██║  ╚══██╔══╝██║████╗ ████║██╔════╝    ████╗  ██║╚══██╔══╝██╔══██╗
██║     ██║   ██║██║     ███████║██║     ██║   ██║██╔████╔██║█████╗      ██╔██╗ ██║   ██║   ██████╔╝
██║     ██║   ██║██║     ██╔══██║██║     ██║   ██║██║╚██╔╝██║██╔══╝      ██║╚██╗██║   ██║   ██╔═══╝
███████╗╚██████╔╝╚██████╗██║  ██║███████╗██║   ██║██║ ╚═╝ ██║███████╗    ██║ ╚████║   ██║   ██║
╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝    ╚═╝  ╚═══╝   ╚═╝   ╚═╝
*/
void Setup_Localtime_NTP()
{
	tft.fillScreen(ST77XX_BLACK);
	tft.drawBitmap(32, 0, ntp, 64, 64, ST77XX_CYAN);
	testdrawtext(5, 60, "ConfigTime", 2, ST77XX_YELLOW);
	tft.setTextSize(1);
	configTime(7 * 3600, 0, "vn.pool.ntp.org", "time.nist.gov");
	Serial.println("Update Time <<< - >>> vn.pool.ntp.org \n");
	while (!time(nullptr))
	{
		Serial.println("\nWaiting for time");
		Serial.print(".");

		tft.setCursor(5, 80);
		tft.setTextColor(ST7735_BLUE, ST7735_BLACK);
		tft.print("Connecting to NTP.");
		delay(200);
		tft.setCursor(5, 80);
		tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
		tft.print("Connecting to NTP..");
		delay(200);
		tft.setCursor(5, 80);
		tft.setTextColor(ST7735_ORANGE, ST7735_BLACK);
		tft.print("Connecting to NTP...");
		delay(200);
		yield(); // disble Soft WDT reset - NodeMCU
	}
	tft.setCursor(5, 80);
	tft.setTextColor(ST7735_ORANGE, ST7735_BLACK);
	tft.print("Connected to NTP...");
	delay(1000);
	uint16_t time_ntp = millis();
	while ((millis() - time_ntp) < 5000)
	{
		tft.setTextSize(1);

		tft.setCursor(5, 80);
		tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
		tft.printf("Setting time %d s   \n", 6 - (millis() - time_ntp) / 1000);

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer_GIO_PHUT_GIAY, 80, "%H:%M:%S", timeinfo);
		strftime(buffer_N_T_N, 80, "%d-%b-%y", timeinfo);

		tft.setTextSize(2);
		tft.setCursor(15, 90);
		tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
		tft.println(buffer_GIO_PHUT_GIAY);
		tft.setCursor(10, 110);
		tft.println(buffer_N_T_N);
	}
	tft.fillScreen(ST77XX_BLACK);
}

void Load_Local_RealTime()
{
	/* Biến load_bitmap chỉ cho phep load 1 lần hình ảnh */
	static bool load_bitmap = true;
	static bool var_hpbd = true;
	Serial.printf("\nload_bitmap %d \n", load_bitmap);

	if ((thang == MON_BangNguyen) && (ngay == DAY_BangNguyen))
	{
		if (var_hpbd)
		{
			tft.fillScreen(ST77XX_BLACK);
			static int age_of_mrbang;
			age_of_mrbang = nam - 1994;
			tft.setTextSize(2);
			tft.setCursor(10, 15);
			tft.printf("%dY", age_of_mrbang);
			/* Load bitmap của ngày sinh nhật */
			tft.drawBitmap(32, 0, hpbd, 64, 64, ST77XX_YELLOW);
			testdrawtext(5, 65, "---HPDB---", 2, ST77XX_YELLOW);
			var_hpbd = false;
		}
		else
		{
			/**/
		}
	}
	else if (load_bitmap)
	{
		tft.fillScreen(ST77XX_BLACK);
		/* Load bitmap của ngày bình thường */
		tft.drawBitmap(32, 0, ntp, 64, 64, ST77XX_CYAN);
		testdrawtext(5, 65, "-RealTime-", 2, ST77XX_YELLOW);
		load_bitmap = false;
		Serial.printf("\nload_bitmap %d \n", load_bitmap);
	}
	else
	{
		var_hpbd = false;
	}

	/* Mỗi năm mới sẽ reload lại thời gian tren NTP một lần */
	if ((thang == 12) && (ngay == 31))
	{
		if ((gio == 12) && (phut == 00))
		{
			if (giay < 10)
			{
				Serial.println("\nCập nhật thời gian mỗi năm.");
				Setup_Localtime_NTP();
			}
		}
	}
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer_NAM, 80, "%Y", timeinfo);
	strftime(buffer_THANG, 80, "%m", timeinfo);
	strftime(buffer_NGAY, 80, "%d", timeinfo);
	strftime(buffer_THU, 80, " %A ", timeinfo);
	strftime(buffer_thu_be, 80, " %w ", timeinfo);
	strftime(buffer_GIO_PHUT_GIAY, 80, "%H:%M:%S", timeinfo);
	strftime(buffer_N_T_N, 80, "%d-%b-%y", timeinfo);
	strftime(buffer_GIO, 80, "%H", timeinfo);
	strftime(buffer_PHUT, 80, "%M", timeinfo);
	strftime(buffer_GIAY, 80, "%S", timeinfo);
	strftime(buffer_sent_serial, 80, "%H:%M:%S %d-%b-%y", timeinfo);

	/* Tách ra để làm các tính toàn nhiệm vụ riêng */
	nam = atoi(buffer_NAM);
	thang = atoi(buffer_THANG);
	ngay = atoi(buffer_NGAY);
	thu = atoi(buffer_thu_be);
	gio = atoi(buffer_GIO);
	phut = atoi(buffer_PHUT);
	giay = atoi(buffer_GIAY);

	if (nam < 2022) /*neu nam duoi 2022 cua nghia la time chua dc update */
	{
		/* lcd hien thi chu UPDATE TIME */
		Serial.println("\nThoi gian chua update.");
		Serial.println("\nHien thi UPDATE TIME trên LCD.");
		// clear một phần màn hình
		tft.fillRect(0, 85, 128, 128 - 85, ST7735_BLACK);
		delay(50);
		tft.setTextSize(2);
		tft.setCursor(0, 85);
		tft.setTextColor(ST7735_BLUE);
		tft.println("Updating      time...");
		delay(100);
		tft.setCursor(0, 85);
		tft.setTextColor(ST7735_CYAN);
		tft.println("Updating      time...");
		delay(100);
		tft.setCursor(0, 85);
		tft.setTextColor(ST7735_GREEN);
		tft.println("Updating      time...");
		delay(100);
	}
	else /* lam viec binh thuong */
	{
		tft.setTextSize(2);
		tft.setCursor(15, 85);
		tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
		tft.println(buffer_GIO_PHUT_GIAY);
		tft.setCursor(10, 105);
		tft.println(buffer_N_T_N);

		if (temp == 0)
		{
			/* */
			tft.setTextSize(1);
			tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
			tft.setCursor(0, 32);
			tft.printf("Temp:\nNone\n", temp);
			tft.setCursor(96, 32);
			tft.printf("Humi:\nNone\n", humidity);
		}
		else
		{
			// if (value_Location_EEPROM == 0)
			// {
			// 	tft.println("Location_TPHCM"); /* chuyen thanh location */
			// }
			// else if (value_Location_EEPROM == 1)
			// {
			// 	tft.println("Location_TPHCM"); /* chuyen thanh location */
			// }
			// else if (value_Location_EEPROM == 2)
			// {
			// 	tft.println("Location_TPHCM"); /* chuyen thanh location */
			// }
			// else if (value_Location_EEPROM == 3)
			// {
			// 	tft.println("Location_TPHCM"); /* chuyen thanh location */
			// }
			tft.setTextSize(1);
			tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
			tft.setCursor(5, 32);
			tft.printf("Temp");
			tft.setCursor(5, 42);
			tft.printf("%.2f\n", temp);
			tft.setCursor(96, 32);
			tft.printf("Humi", humidity);
			tft.setCursor(96, 42);
			tft.printf("%d %%\n", humidity);
		}

		/* Qua khung giờ mới là tingting  */
		if ((phut == 0) && (giay < 3))
		{
			digitalWrite(PIN_signal_Bell, ESP_NB_ON);
		}
		else
		{
			digitalWrite(PIN_signal_Bell, ESP_NB_OFF);
		}
		/* Báo thức */
		if ((hen_gio == gio) && (hen_phut == phut) &&
			(giay < 3) && (status_Mode_Alarm == ESP_NB_ON))
		{
			// Active_Alarm();
			status_Mode_Alarm = ESP_NB_OFF;
		}
		else
		{
			digitalWrite(PIN_signal_Bell, ESP_NB_OFF);
			/* Bật trang thai báo thức của Active_Alarm */
			status_Mode_Alarm = ESP_NB_ON;
		}
	}
}

void Weather_Online_sever()
{
	tft.drawBitmap(32, 0, weather, 64, 64, ST77XX_CYAN);
	delay(100);
	tft.setTextSize(2);
	tft.setCursor(8, 60);
	tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
	tft.println("-Weather-");
	tft.setTextSize(1);
	tft.setCursor(0, 80);
	tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
	if (value_Location_EEPROM == 0)
	{
		Location = Location_TPHCM;
		Serial.println("\nĐịa điểm lấy data thời tiết: Location_TPHCM\n");
		tft.println("Location_TPHCM");
	}
	else if (value_Location_EEPROM == 1)
	{
		Location = Location_VUTAU;
		Serial.println("\nĐịa điểm lấy data thời tiết: Location_VUTAU\n");
		tft.println("Location_VUTAU");
	}
	else if (value_Location_EEPROM == 2)
	{
		Location = Location_DALAT;
		Serial.println("\nĐịa điểm lấy data thời tiết: Location_DALAT\n");
		tft.println("Location_DALAT");
	}
	else if (value_Location_EEPROM == 3)
	{
		Location = Location_TPHUE;
		Serial.println("\nĐịa điểm lấy data thời tiết: Location_TPHUE\n");
		tft.println("Location_TPHUE");
	}
	else // Nếu lần đầu tiên khởi động chưa setup vị trí thì mặc định là TPHCM
	{
		Location = Location_TPHCM;
		Serial.println("\nĐịa điểm lấy data thời tiết: Location_TPHCM\n");
		tft.println("Location_TPHCM");
	}

	if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
	{
		HTTPClient http; // Declare an object of class HTTPClient

		// specify request destination
		http.begin(client, "http://api.openweathermap.org/data/2.5/weather?id=" + Location + "&APPID=" + APIKey_openweather);

		int httpCode = http.GET(); // send the request

		if (httpCode > 0) // check the returning code
		{
			String payload = http.getString(); // Get the request response payload

			DynamicJsonBuffer jsonBuffer(512);

			// Parse JSON object
			JsonObject &root = jsonBuffer.parseObject(payload);
			if (!root.success())
			{
				Serial.println(F("Parsing failed !"));
			}

			temp = (float)(root["main"]["temp"]) - 273.15;		 // get temperature in °C
			humidity = root["main"]["humidity"];				 // get humidity in %
			pressure = (float)(root["main"]["pressure"]) / 1000; // get pressure in bar
			wind_speed = root["wind"]["speed"];					 // get wind speed in m/s
			wind_degree = root["wind"]["deg"];					 // get wind degree in °

			// print data
			Serial.printf("Temperature = % .2f°C\n", temp);
			Serial.printf("Humidity = % d % %\n", humidity);
			Serial.printf("Pressure = % .3f bar\n", pressure);
			Serial.printf("Wind speed = % .1f m / s\n", wind_speed);
			Serial.printf("Wind degree = % d°\n\n", wind_degree);
		}
		http.end(); // Close connection
		yield();	// disble Soft WDT reset - NodeMCU
	}
	else
	{
		Serial.println("Wifi không có Internet !\n");
	}
	tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
	tft.setCursor(0, 90);
	tft.setTextSize(2);
	tft.printf("Humi:%d %%\n", humidity);
	tft.printf("Temp:%.2f\n", temp);
	delay(2000);
}

/* Hàm gọi thời tiết mỗi 10 phút một lần */
void Call_Weather_Every_10Min()
{
	if ((unsigned long)(millis() - time_dem_thoitiet) > 60 * 10 * 1000) // 10 phut/ lan
	{
		time_dem_thoitiet = millis();
		if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
		{
			HTTPClient http; // Declare an object of class HTTPClient

			// specify request destination
			http.begin(client, "http://api.openweathermap.org/data/2.5/weather?id=" + Location + "&APPID=" + APIKey_openweather);

			int httpCode = http.GET(); // send the request

			if (httpCode > 0) // check the returning code
			{
				String payload = http.getString(); // Get the request response payload

				DynamicJsonBuffer jsonBuffer(512);

				// Parse JSON object
				JsonObject &root = jsonBuffer.parseObject(payload);
				if (!root.success())
				{
					Serial.println(F("Parsing failed !"));
				}

				temp = (float)(root["main"]["temp"]) - 273.15;		 // get temperature in °C
				humidity = root["main"]["humidity"];				 // get humidity in %
				pressure = (float)(root["main"]["pressure"]) / 1000; // get pressure in bar
				wind_speed = root["wind"]["speed"];					 // get wind speed in m/s
				wind_degree = root["wind"]["deg"];					 // get wind degree in °

				// print data
				Serial.printf("Temperature = % .2f°C\n", temp);
				Serial.printf("Humidity = % d % %\n", humidity);
				Serial.printf("Pressure = % .3f bar\n", pressure);
				Serial.printf("Wind speed = % .1f m / s\n", wind_speed);
				Serial.printf("Wind degree = % d°\n\n", wind_degree);
			}
			http.end(); // Close connection
			yield();	// disble Soft WDT reset - NodeMCU
		}
		else
		{
			Serial.println("Wifi không có Internet !\n");
		}
	}
}
