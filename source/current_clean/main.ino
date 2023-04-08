#if LWIP_FEATURES && !LWIP_IPV6
 
  #define HAVE_NETDUMP 0
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <lwip/napt.h>
  #include <lwip/dns.h>
  #include <LwipDhcpServer-NonOS.h>
  #include <SPI.h>
  #include <Wire.h>
  #include <WiFiUdp.h>
  #include <ESP8266mDNS.h>

  int numberOfNetworks;
  unsigned long lastAusgabe;
  const unsigned long intervall = 5000;
  const char* dns_name = "blackzrepeater"; // set your mDNS - Name you can connect to ..or exp. http://blackzrepeater.local

  #define NAPT 1000
  #define NAPT_PORT 10


  #if HAVE_NETDUMP
 
  #include <NetDump.h>
 
  void dump(int netif_idx, const char* data, size_t len, int out, int success) {
  (void)success;
    Serial.print(out ? F("out ") : F(" in "));
    Serial.printf("%d ", netif_idx);
 
    // optional filter example: if (netDump_is_ARP(data))
  {
      netDump(Serial, data, len);
      //netDumpHex(Serial, data, len);
  }
  }
  #endif
 


  // MY FUNCTIONS
  bool testwifi() {
    Serial.printf("\nConsole<>> Testing connection with '%s'\n", WiFi.SSID().c_str());
    int count = 0;
    digitalWrite(LED_BUILTIN,LOW);
    while (count < 20) {
      if (WiFi.status() == WL_CONNECTED) {
         Serial.printf("\nSTA: %s (dns: %s / %s)\n", WiFi.localIP().toString().c_str(), WiFi.dnsIP(0).toString().c_str(), WiFi.dnsIP(1).toString().c_str());

  // By default, DNS option will point to the interface IP
  // Instead, point it to the real DNS server.
  // Notice that:
  // - DhcpServer class only supports IPv4
  // - Only a single IP can be set
  auto& server = WiFi.softAPDhcpServer();
  server.setDns(WiFi.dnsIP(0));
        digitalWrite(LED_BUILTIN,HIGH);
        return true;
    }
      Serial.print(".");
      delay(1000);
      count++;
  }
    return false;
  }
 

 // Starts esp8266webserver listening on port 80
  ESP8266WebServer server(80);

// Define html's content as string
  String content;
  String networks[40];
  String msx;
  String msg;


// Function to Scan-WiFi-Networks and print them to serial-mon
void wifi_sercialscan() {
        if (numberOfNetworks <= 0)
      {
        numberOfNetworks = WiFi.scanNetworks();
        Serial.print(numberOfNetworks);
        Serial.println(F(" gefundene Netzwerke"));
      }
      else if (millis() - lastAusgabe > intervall)
      {
        numberOfNetworks--;
  
        Serial.print("Network name: ");
        Serial.println(WiFi.SSID(numberOfNetworks));
        Serial.print("Signal strength: ");
        Serial.println(WiFi.RSSI(numberOfNetworks));
        Serial.println("-----------------------");
        lastAusgabe = millis();
      }
}


// authentication-handler-function used by webpage
bool is_authentified(){
  Serial.println("Console<>> Enter is_authentified");
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Console<>> Authentification Successful");
      delay(3000);
      return true;
    }
  }
  Serial.println("Console<>> Authentification Failed");
  return false;
}


 
void serverconfig() {
  server.begin(); 


// BEGIN OF REPEATER_CONFIGURATION-SITES CONTENT AND CPP-FUNCTIONS WHICH SETS THE RIGHT VARIABLES FOR THE HTML-CONTENT
    int n = WiFi.scanNetworks();
    msx = "<!DOCTYPE html><html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    msx += "<style>head{ background-color: #8F8F8F; color: #FF0000; } body{ background-color: #8F8F8F; color: #00ECFF; } ul { list-style-type: none; margin: 0; padding: 0; overflow: hidden; background-color: #333; } li { float: left; border-right:1px solid #bbb; } li:last-child { border-right: none; } li a { display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none; } li a:hover:not(.active) { background-color: #111; } .active { background-color: #04AA6D; }</style>";
    msx += "<head><title>ESP8266 Configuration Page</title></head>";
    msx += "<body><ul><li><a href='/'>Home</a></li><li><a class='active' href='/repeater'>Repeater</a></li><li style='float:right'><a href='/reboot'>Reboot</a></li><li><a href='/wifiscan'>WiFi-Scan</a></li></ul>";  
    if (WiFi.status() != WL_CONNECTED) {
      msx += "<center><div>currently not connected</div></center>";
      }
    else {
      msx += "<center><div>connected to: ";
      msx += WiFi.SSID();
      msx += " IP: ";
      msx += WiFi.localIP().toString();
      msx += "</div></center>";
    }
    msx += "<center><div>";
    if (n == 0) {
      msx += "<center><h1>No wireless networks found</h1></center>";
  }
    else {
      msx += "<center><h1>Wireless Station Settings</h1></center>";
      msx += "<form method='post'>";
      for (int i = 0; i < n; ++i) {
        networks[i] = WiFi.SSID(i);
        msx += "<div>";
        msx += "<center><input type=\"radio\" id=\"";
        msx += String(i);
        msx += "\" ";
        msx += "name=\"SSIDs\" value=\"";
        msx += String(i);
        msx += "\"></center>";
        msx += "<center><label for=\"";
        msx += String(i);
        msx += "\"";
        msx += ">";
        msx += String(i + 1);
        msx += ": ";
        msx += WiFi.SSID(i);
        msx += " (";
        msx += String(WiFi.RSSI(i));
        msx += ")";
        msx += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
        msx += "</label></center>";
        msx += "</div>";
        delay(10);
    }
      msx += "<center><label>Password:</label></center><br><input type='password' placeholder='********' name='stapsk' minlength=8 maxlength=63><br><center><small>Must be at least 8 characters or blank!</small></center><br><br>";
      msx += "<center><button type='submit' formaction='stasettings'>Save Permanently</button></center><center><button type='submit' formaction='tempstasettings'>Save Temporarily (Until Reboot)</button></center>";
      msx += "</form>";  
      msx += "<center><h1>Wireless Access Point Settings</h1></center>";
      msx += "<form method='post'>";
      msx += "<center><label>SSID:</label></center><br><input name='apssid' placeholder='";
      msx += WiFi.softAPSSID();
      msx += "' length=32><br>";
      msx += "<center><label>Password:</label></center><br><input type='password' placeholder='";
      msx += WiFi.softAPPSK();
      msx += "' name='appsk' minlength=8 maxlength=63><br><center><small>Must be at least 8 characters or blank!</small></center><br><br>";
      msx += "<center><button type='submit' formaction='apsettings'>Save Permanently</button></center><center><button type='submit' formaction='tempapsettings'>Save Temporarily (Until Reboot)</button></center>";
      msx += "</form>";
  }
    msx += "<center><h1>Miscellaneous</h1></center>";
    msx += "<form method='get' action='reboot'><input type='submit' value='Reboot'></form>";
    msx += "<div>";    

// ESPWEBSERVER URI HANDLER
 server.on("/login", [](){
    String msgd;
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Console<>> Disconnection");
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("REPEATER")){
    Serial.println("Console<>> REPEATER.");
    server.sendHeader("Location","/repeater");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=2");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "root" &&  server.arg("PASSWORD") == "toor" ){
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      return;
    }
  msgd = "<style>p1 { color: red; }</style><center><p1>Wrong username/password! try again.</p1></center>";
  Serial.println("Console<>> Log in Failed");
  }
  String content = "<html><body><style>p1 { color: red; } body { background-color: grey; color: green; } head { background-color: grey; color: green; }</style><center><form action='/login' method='POST'><center><p1>To log in, please use : root/toor</p1></center><br>";
  content += "<center><p1>User:</p1></center><center><input type='text' name='USERNAME' placeholder='user name'></center><br>";
  content += "<center><p1>Password:</p1></center><input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<center><input type='submit' name='SUBMIT' value='Submit'></center></form></center>" + msgd + "<br>";
  content += "<center><p1>You also can go</p1></center><center><a href='/inline'><p1>here</p1></a></center></body></html>";
  server.send(200, "text/html", content);
});

server.on("/", [](){
  String header;
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
  String content;
  content = "<!DOCTYPE html><html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>ul { list-style-type: none; margin: 0; padding: 0; overflow: hidden; background-color: #333; } li { float: left; border-right:1px solid #bbb; } li:last-child { border-right: none; } li a { display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none; } li a:hover:not(.active) { background-color: #111; } .active { background-color: #04AA6D; } head { background-color: #8F8F8F; color: #FF0000; } body{ background-color: #8F8F8F; color: #00ECFF; } h2 { color: green; }</style><head><center><h2>You successfully connected to blackleakz.local</h2></center>";
  if (server.hasHeader("User-Agent")){
    content += "<center><p1>the user agent used is : </p1></center><center>" + server.header("User-Agent") + "</center><br><br>";
  }
  content += "<center><p1>You can access this page until you</p1></center><center><a href=\"/login?DISCONNECT=YES\">disconnect</a></center><br></head>";
  content += "<body><ul><li><a class='active' href='/'>Home</a></li><li><a href='/repeater'>Repeater</a></li><li><a href='/otaupdate'>OTA-Update</a></li><li style='float:right'><a href='/reboot'>Reboot</a></li><li><a href='/wifiscan'>WiFi-Scan</a></li></ul>";
  server.send(200, "text/html", content);
});


// WiFi-Scan function, connected by html button defined in index_html
  server.on("/wifiscan", []() {
    int n = WiFi.scanNetworks();
    msg = "<!DOCTYPE html><html><title>BlackLeakz-Repeater | WiFi-Scan</title><style> head { background-color: grey; color: green; } body { background-color= grey; color: green; } ul { list-style-type: none; margin: 0; padding: 0; overflow: hidden; background-color: #333; } li { float: left; border-right:1px solid #bbb; } li:last-child { border-right: none; } li a { display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none; } li a:hover:not(.active) { background-color: #111; } .active { background-color: #04AA6D; }</style><head><center><h1>BlackLeakz-Repeater | WiFi-Scan</h1></center></head><body><ul><li><a href='/'>Home</a></li><li><a href='/repeater'>Repeater</a></li><li><a href='/otaupdate'>OTA-Update</a></li><li style='float:right'><a href='/reboot'>Reboot</a></li><li><a class='active' href='/wifiscan'>WiFi-Scan</a></li></ul>";
    for (int i = 0; i < n; ++i) {
      networks[i] = WiFi.SSID(i);
      msg += "<center><div>";
      msg += "<input type=\"radio\" id=\"";
      msg += String(i);
      msg += "\" ";
      msg += "name=\"SSIDs\" value=\"";
      msg += String(i);
      msg += "\">";
      msg += "<label for=\"";
      msg += String(i);
      msg += "\"";
      msg += ">";
      msg += String(i + 1);
      msg += ": ";
      msg += WiFi.SSID(i);
      msg += " (";
      msg += String(WiFi.RSSI(i));
      msg += ")";
      msg += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
      msg += "</label>";
      msg += "</div></body></html>";
      delay(10);
      wifi_sercialscan();

      }
      server.send(200, "text/html", msg);
  });
  
// Site not found
    server.onNotFound([]() {
      server.send(404, "text/plain", "How the heck did you get here?");
  });

// Button-connected redirect function to redirect to Repeater-config site
 server.on("/repeater", []() {
      server.send(200, "text/html", msx); //  msx is the repeater sites html-content string , executed by this command
  });

    server.on("/stasettings", []() {
      String temp = server.arg("SSIDs");
      int number = temp.toInt();
      String stassid = networks[number];
      String stapsk = server.arg("stapsk");
      if (stassid.length() > 0) {
        server.send(200, "text/plain", "Settings Recieved");
        Serial.printf("\n\nAttempting to connect to '%s' using password '%s' \n", stassid.c_str(), stapsk.c_str());
        WiFi.persistent(true);
        WiFi.begin(stassid, stapsk);
        testwifi();
    }
  });

 // Temporary SSID AND PASSPHRASE TO ROUTER 2 connect to
  server.on("/tempstasettings", []() {
    String temp = server.arg("SSIDs");
    int number = temp.toInt();
    String stassid = networks[number];
    String stapsk = server.arg("stapsk");
    if (stassid.length() > 0) {
      String cat;
      cat = "<DOCTYPE html><html><title>Settings recieved</title></html>";
      cat += "<head><h1>Settings recieved.</h1></head></html>";
      server.send(200, "text/html", cat);
      Serial.printf("\n\nConsole<>> Attempting to connect to '%s' using password '%s' \n", stassid.c_str(), stapsk.c_str());
      WiFi.persistent(false);
      WiFi.begin(stassid, stapsk);
      testwifi();
    }
  });
 

 // Permanent Repeater-Config
  server.on("/apsettings", []() {
    String apssid = server.arg("apssid");
    String appsk = server.arg("appsk");
    if (apssid.length() > 0) {
      server.send(200, "text/plain", "Settings Recieved");
      Serial.printf("\n\nConsole<>> Setting AP Credentials \nSSID: %s \nPassword: %s \n", apssid.c_str(), appsk.c_str());
      WiFi.persistent(true);
      WiFi.softAP(apssid, appsk);
    }
  });
 

 // Temporary Repeater-AP-SSID setting
  server.on("/tempapsettings", []() {
    String apssid = server.arg("apssid");
    String appsk = server.arg("appsk");
    if (apssid.length() > 0) {
      String cat;
      cat = "<DOCTYPE html><html><title>Settings recieved</title>";
      cat += "<head><h1>Settings recieved.</h1></head></html>";
      server.send(200, "text/html", cat);
      Serial.printf("\n\nConsole<>> Setting Temporary AP Credentials \nSSID: %s \nPassword: %s \n", apssid.c_str(), appsk.c_str());
      WiFi.persistent(false);
      WiFi.softAP(apssid, appsk);
    }
  });


 // Reboot function 
  server.on("/reboot", []() {
    String cat;
      cat = "<DOCTYPE html><html><title>REBOOT</title>";
      cat += "<head><h1>Rebooting...</h1></head></html>";

      server.send(200, "text/html", cat);
    delay(5000);
    ESP.reset();
  });
 
//here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
//ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );   
// redirect to repeater-settings site, connected by html button defined in index_html

}

 // SETUP
  void setup() {
    // Begin of serial-comunication with a baudrate of 115200
    Serial.begin(115200);
    Serial.println("Console<>> Console started."); // prints smth  to serial.com
    // Define BUILTIN-LED to LED_BUILTIN, OUTPUT and let it blink
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(400);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Console<>> Initialize display.");
 
    

  // Register the dns_name set at the begin of the code
    if (MDNS.begin(dns_name)) {
    Serial.println("DNS gestartet, erreichbar unter: ");
    Serial.println("http://" + String(dns_name) + ".local/");
  }
  

    delay(1000);
    Serial.printf("\n\nConsole<>> NAPT Range extender\n");
    Serial.printf("Console<>> Heap on start: %d\n", ESP.getFreeHeap());

  #if HAVE_NETDUMP
    phy_capture = dump;
  #endif
  //ESP8266WiFi SETTINGS
    WiFi.setPhyMode(WIFI_PHY_MODE_11N); // Set radio type to N
    WiFi.mode(WIFI_AP_STA); // SET MODE TO both AP+Client named AP_STA
    WiFi.persistent(false);
    WiFi.begin(); // Use stored credentials to connect to network
    testwifi(); // Starts TESTFUNCTION
    WiFi.softAPConfig(  // Set IPv4 Address, Gateway and Subnet
      IPAddress(192, 168, 4, 1), 
      IPAddress(192, 168, 4, 1),
      IPAddress(255, 255, 255, 0)); 
      WiFi.softAP(WiFi.softAPSSID(), WiFi.softAPPSK()); // Use stored credentials to create AP
    
// Print SSID AND PSK to serial-mon
    Serial.println("Console<>> SSID: " + WiFi.softAPSSID() + "\n\nPASSPHRASE: " + WiFi.softAPPSK());
    delay(100);
    Serial.printf("Console<>> Heap before: %d\n", ESP.getFreeHeap());
    err_t ret = ip_napt_init(NAPT, NAPT_PORT);
    Serial.printf("Console<>> ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);

    if (ret == ERR_OK) {
      ret = ip_napt_enable_no(SOFTAP_IF, 1);
      Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
      if (ret == ERR_OK) {
    }
  }
    Serial.printf("Console<>> Heap after napt init: %d\n", ESP.getFreeHeap());
    if (ret != ERR_OK) {
      Serial.printf("Console<>> NAPT initialization failed\n");
    }
    serverconfig();
  }
 
  #else
  void setup() {
    Serial.begin(115200);
    Serial.printf("\n\nConsole<>> NAPT not supported in this configuration\n");
  }
  #endif
 
  void loop() {
    server.handleClient();
    MDNS.update();
    
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
  }
    else {
      digitalWrite(LED_BUILTIN, HIGH);
  }
 }
  