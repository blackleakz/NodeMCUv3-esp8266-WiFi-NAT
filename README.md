 ```
 Version: 0.18a
 Author: BlackLeakz
 Github-repo: https://github.com/blackleakz/NodeMCUv3-esp8266-WiFi-Repeater-Range-extender 
 Beta-Channel: https://github.com/blackleakz/NodeMCUv3-esp8266-WiFi-NAT 



Resources:
- OLED_SSD1306(Adafruit)(128 x64/128/32) x 1
- Breadbord x 1
- ICR-18650 20F Cell Samsung x 1
- NodeMCUv3-ESP8266  x 1

```

Type of Program: NAT_WiFi-Router 
build for NodeMCU - esp8266 , 
(for esp32 u need 2 edit the dhcpSoftAP.dhcp.set.dns(0, WiFi.setDnsIP(0)) part)

Resources-Datasheets&Pinouts:
 - NodeMCUv3-esp8266 - ['https://components101.com/sites/default/files/component_pin/NodeMCU-ESP8266-Pinout.jpg'] (URL)
 - Samsung Accu Cell - ['https://secondlifestorage.com/index.php?threads/samsung-icr18650-20f-cell-specifications.1757/'] (URL)
 - Oled SSD1306 Display 128x64 ['https://www.rajguruelectronics.com/Product/1145/OLED%204%20Pin%20128x64%20Display%20module%200.96%20inch%20blue%20color.pdf'] (URL)
```
Libarys:
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <lwip/napt.h>
  #include <lwip/dns.h>
  #include <LwipDhcpServer-NonOS.h>
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <Adafruit_SSD1306.h>
```
  
  SOURCE:
  
  - ['https://lastminuteengineers.com/creating-esp8266-web-server-arduino-ide/'] (URL: Basic WebServer) - State:(modified using)# server on_handle functions could be declared by using the server.on(400
  
 - ['https://github.com/esp8266/ESPWebServer/blob/master/examples/SimpleAuthentification/SimpleAuthentification.ino'] (URL: Simple Login-Auth-System for WebServer by headers&coockies) - State:(using)
 - ['https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/HttpBasicAuth/HttpBasicAuth.ino'] (URL: HttpBasicAuth) - State:(could be used instead)
  
 - ['https://www.az-delivery.de/en/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/wifi-extender-mit-esp8266-und-esp32'] (URL: nat-router-esp8266 #nodemcu #example) - State:(using)
  
 - ['https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/'] (URL: OLED_SSD1306 128x64 or 128x32 DISPLAY) - State:(using)
  
  - ['https://secondlifestorage.com/index.php?threads/samsung-icr18650-20f-cell-specifications.1757/'] (URL: ACCU Samsung) - State:(using)
  
  - ['https://itler.net/esp8266-wifi-netze-auf-bestimmtes-wlan-scannen/'] (URL: WiFi-Scan function) - State:(could be interesting for understanding the current WiFi-Scan function, which prints Results to oled display and website)
  - ['https://techtutorialsx.com/2017/02/25/esp8266-scanning-wifi-networks/'] (URL: Sample WiFi-Scan FUNCTION) - State:(using)
  
  - ['https://www.az-delivery.de/en/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/ota-over-the-air-esp-programmieren-uber-wlan-entwurf'] (URL: OTA Update) - State:(using)
  
  
  
  - ['https://gist.github.com/idreamsi/f3caa557fb55e4fd1c69045de8c24719#file-esp8266-softap-webserver-ino'] (URL: SoftAP- example) - State:(interesting)
  
  - ['https://randomnerdtutorials.com/esp8266-0-96-inch-oled-display-with-arduino-ide/'] (URL: OLED SSD1306 128x64 Display Adafruit) - State:(using)
  
  ```
  
  To install this firmware you can copy the code to your Arduino IDE or PlatformIO IDE and flash , otherwise you can download the compiledbinary and flash it with the esp8266-flash tool.
  
  
  
