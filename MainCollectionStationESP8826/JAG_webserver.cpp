/*
 *  JAG_webserver = library to handle the ESP 8826 web server
 * 
 */

#include "Arduino.h"
#include <ArduinoJson.h>

#include "JAG_webserver.h"
 // initialise

JAG_webserver::JAG_webserver(int port, bool logSerial) {
  port_ = port;
  logSerial_ = logSerial;
  remote1_showing = false;
  remote2_showing = false;
  remote3_showing = false;
  ESP8266WebServer webserver_(port_);
}

// Public Functions

/*
 * This sets upt he web server and make sure the serial console show a message when complete
 * 
 */
void JAG_webserver::setup() {
  setupRoutes();
  webserver.begin();
  if (logSerial_) {
    Serial.println("HTTP server available on port:");
    Serial.println(port_);
  }
}

// we always need to know when a client has connected.

void JAG_webserver::runServerInLoop() {
  webserver.handleClient();
}



/* PAGE NOT FOUND */

String JAG_webserver::get404PageContents() {
  String message = "Resource Not Found<br />";
  message += "URI:<br /> ";
  message += webserver.uri();
  message += "<br />Method: ";
  message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "<br />Arguments: ";
  message += webserver.args();
  message += "<br />";
  for (uint8_t i = 0; i < webserver.args(); i++) {
    message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "<br />";
  }
  return message;
}

String JAG_webserver::getRootPageContents() {
  String returnHTML = "";
  returnHTML += "<div style=\"text-align: center; width: 100%\"><h2>Welcome to JAG ESP - IoT Monitoring Base Station</h2></div>";
  returnHTML += "<br />";
  returnHTML += "<br />";
  returnHTML += "<div class=\"container\">";
  returnHTML += "<div class=\"row\">";
  returnHTML += "<div class=\"col-sm border border-primary rounded\">";
  returnHTML += "<br />";
  returnHTML += "<b>Below is the list of the top people that have badged</b><br /><br />";
  returnHTML += createBadgingInfo();
  returnHTML += "</div>";
  returnHTML += "<div style=\"margin-left: 20px\" class=\"col-sm border border-primary rounded \">";
  returnHTML += "<br />";
    returnHTML += "<b>Station Details will appears below as they connect</b><br /><br />";
  returnHTML += createStationInfo("1");
  returnHTML += "<br/><br />";
  returnHTML += createStationInfo("2");
  returnHTML += "<br/><br />";
  returnHTML += createStationInfo("3");
  returnHTML += "<br/><br />";
  returnHTML += "</div>";
  returnHTML += "</div>";
  returnHTML += "</div>";

  returnHTML += "<div class=\"container\" style=\"margin-top: 10px; text-align: right;\">";
  returnHTML += "<div class=\"row\">";
    returnHTML += "<div class=\"col-sm border border-primary rounded\" style=\"padding-bottom: 18px\">";
      returnHTML += "<br />";
      returnHTML += "<button style=\"margin-right: 10px\" class=\"btn btn-primary\" onclick=\"refreshPage()\">Manual Refresh</button><b>Page last Updated <span id=\"up_time\">0</span>s ago";
    returnHTML += "</div>";
  returnHTML += "</div>";
  returnHTML += "</div>";

  return returnHTML;
}

// Private Functions

void JAG_webserver::setupRoutes() {
  webserver.on("/", [ & ]() {
    pageLastUpdate = millis();
    webserver.send(200, "text/html", createHTMLpageWithContent_(getRootPageContents()));
  });

  webserver.on("/update", [ & ]() {
    if (webserver.hasArg("plain") == false) { //Check if body received
      webserver.send(200, "text/plain", "Body not received");
      return;
    }
    String message = webserver.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, message);
      processReceivedStrings(doc);
    webserver.send(200, "text/plain", message);

  });

    webserver.on("/userBadged", [ & ]() {
    if (webserver.hasArg("plain") == false) { //Check if body received
      webserver.send(200, "text/plain", "Body not received");
      return;
    }
    String message = webserver.arg("plain");
  DynamicJsonDocument doc(1024);
      deserializeJson(doc, message);

      String username = doc["username"];
    
  

    bool userFound = false;
    int userFoundAt = 0;

    for (int i=0; i < 99; i++) {
      if (badgingUser[i] != "") {
        String only_user = badgingUser[i].substring(0,badgingUser[i].indexOf("_"));
        if (username == only_user) {
          userFound = true;
          userFoundAt = i;
          break; 
        }
      } 
    }

    if (userFound) {
     
     String only_user = badgingUser[userFoundAt].substring(0,badgingUser[userFoundAt].indexOf("_"));
     String user_count = String(badgingUser[userFoundAt].substring(badgingUser[userFoundAt].indexOf("_") + 1));
     user_count = String(user_count.toInt() + 1);
     badgingUser[userFoundAt] = only_user + "_" + user_count;
    } else {
      for (int i=0; i < 99; i++) {
        if (badgingUser[i] == "") {
          badgingUser[i] = username + "_1";
          break; 
        }
      }
    }
    
    for(int i = 0; i < 99; i++)
    {
      if (badgingUser[i] !="") {
  
        Serial.println(String(i) + " : " + badgingUser[i] );
      }
      
    }
     webserver.send(200, "text/plain", username);

  });

  webserver.onNotFound([ & ]() {
    webserver.send(404, "text/html", createHTMLpageWithContent_(get404PageContents()));
  });
}

/*
 * Process a JSON string that we have recieved from the remote monitoring station
 * 
 * 
 * */
void JAG_webserver::processReceivedStrings(DynamicJsonDocument doc) {

  String item_number = doc["station_id"];
  String item_name = doc["station_name"];
  String item_type = doc["sensor_type"];
  String item_value = doc["sensor_value"];

  if (item_number == "1") {
    remote1_name = item_name;
    remote1_lastconnect = millis();
    if (item_type == "TEMP") {
      remote1_temp = item_value;
      addTempValueToArray(item_value, item_number);
    } else if (item_type == "HUMIDITY") {
      remote1_humidity = item_value;
      addHumidityValueToArray(item_value, item_number);
    } else if (item_type == "BADGE") {
      remote1_lastbadge = item_value;
    } else if (item_type == "GAS") {
      remote1_GAS = item_value;
      addGasValueToArray(item_value, item_number);
    }
  } else if (item_number == "2") {
    remote2_name = item_name;
    remote2_lastconnect = millis();
    if (item_type == "TEMP") {
      remote2_temp = item_value;
      addTempValueToArray(item_value, item_number);
    } else if (item_type == "HUMIDITY") {
      remote2_humidity = item_value;
      addHumidityValueToArray(item_value, item_number);
    } else if (item_type == "BADGE") {
      remote2_lastbadge = item_value;
    } else if (item_type == "GAS") {
      remote2_GAS = item_value;
     addGasValueToArray(item_value, item_number);
    }
  } else if (item_number == "3") {
    remote3_name = item_name;
    remote3_lastconnect = millis();
    if (item_type == "TEMP") {
      remote3_temp = item_value;
      addTempValueToArray(item_value, item_number);
    } else if (item_type == "HUMIDITY") {
      remote3_humidity = item_value;
      addHumidityValueToArray(item_value, item_number);
    } else if (item_type == "BADGE") {
      remote3_lastbadge = item_value;
    } else if (item_type == "GAS") {
      remote3_GAS = item_value;
      addGasValueToArray(item_value, item_number);
    }
  } else {

  }
}

/*
 * Create the HTML to shows the sensor values on the page
 * 
 * 
 * */
String JAG_webserver::createStationInfo(String stationNumber) {
  String returnHTML = "";
  if (stationNumber == "1") {
    remote1_showing = false;
    if (remote1_name != "") {
      remote1_showing = true;
      returnHTML += "<div class=\"card\" style=\"width: 100%;\">";
      returnHTML += "<div class=\"card-body\">";
      returnHTML += "<h5 class=\"card-title\">" + remote1_name + "</h5>";
      returnHTML += "<h6 class=\"card-subtitle mb-2 text-muted\"></h6>";
      returnHTML += "<p class=\"card-text\">";
      returnHTML += "Current Temp: " + remote1_temp + char(176) + " C<br / >";
      returnHTML += "<div><canvas id=\"temp_1_chart\" width=\"600\" height=\"400\"></canvas></div>";
      returnHTML += "Current Humidity: " + remote1_humidity + " %<br / >";
      returnHTML += "<div><canvas id=\"humidity_1_chart\" width=\"600\" height=\"400\"></canvas></div>";
      returnHTML += "Current Gas: " + remote1_GAS + " ppm<br / >";
      returnHTML += "<div><canvas id=\"gas_1_chart\" width=\"600\" height=\"400\"></canvas></div>";
      returnHTML += "Last Badge: " + remote1_lastbadge + "<br / >";
      returnHTML += "</p>";
     returnHTML += "<a href=\"http://" + IpAddress2String(webserver.client().remoteIP()) + "\" class=\"card-link\">Station Web Interface</a>";
      returnHTML += "<br />";
     returnHTML += "Updated " + showNiceLastConnect(remote1_lastconnect);
      returnHTML += "</div>";

    
      returnHTML += "</div>";
    }
  } else if (stationNumber == "2") {
    remote2_showing = false;
     if (remote2_name != "") {
      remote2_showing = true;
        returnHTML += "<div class=\"card\" style=\"width: 100%;\">";
        returnHTML += "<div class=\"card-body\">";
        returnHTML += "<h5 class=\"card-title\">" + remote2_name + "</h5>";
        returnHTML += "<h6 class=\"card-subtitle mb-2 text-muted\"></h6>";
        returnHTML += "<p class=\"card-text\">";
        returnHTML += "Current Temp: " + remote2_temp + char(176) + " C<br / >";
         returnHTML += "<div><canvas id=\"temp_2_chart\" width=\"600\" height=\"400\"></canvas></div>";
        returnHTML += "Curent Humidity: " + remote2_humidity + "<br / >";
        returnHTML += "<div><canvas id=\"humidity_2_chart\" width=\"600\" height=\"400\"></canvas></div>";
        returnHTML += "Current Gas: " + remote2_GAS + " ppm<br / >";
         returnHTML += "<div><canvas id=\"gas_2_chart\" width=\"600\" height=\"400\"></canvas></div>";
        returnHTML += "Last Badge: " + remote2_lastbadge + "<br / >";
     
        returnHTML += "</p>";
       returnHTML += "<a href=\"http://" + IpAddress2String(webserver.client().remoteIP()) + "\" class=\"card-link\">Station Web Interface</a>";
        returnHTML += "<br />";
     returnHTML += "Updated " + showNiceLastConnect(remote2_lastconnect);
        returnHTML += "</div>";
        returnHTML += "</div>";
     }
  } else if (stationNumber == "3") {
    remote3_showing = false;
     if (remote3_name != "") {
      remote3_showing = true;
        returnHTML += "<div class=\"card\" style=\"width: 100%;\">";
        returnHTML += "<div class=\"card-body\">";
        returnHTML += "<h5 class=\"card-title\">" + remote3_name + "</h5>";
        returnHTML += "<h6 class=\"card-subtitle mb-2 text-muted\"></h6>";
        returnHTML += "<p class=\"card-text\">";
        returnHTML += "Current Temp: " + remote3_temp + char(176) + " C<br / >";
          returnHTML += "<div><canvas id=\"temp_3_chart\" width=\"600\" height=\"400\"></canvas></div>";
        returnHTML += "Current Humidity: " + remote3_humidity + "<br / >";
           returnHTML += "<div><canvas id=\"humidity_3_chart\" width=\"600\" height=\"400\"></canvas></div>";
        returnHTML += "Current Gas: " + remote3_GAS + " ppm<br / >";
        returnHTML += "<div><canvas id=\"gas_3_chart\" width=\"600\" height=\"400\"></canvas></div>";
        returnHTML += "Last Badge: " + remote3_lastbadge + "<br / >";
     
        returnHTML += "</p>";
         returnHTML += "<a href=\"http://" + IpAddress2String(webserver.client().remoteIP()) + "\" class=\"card-link\">Station Web Interface</a>";
          returnHTML += "<br />";
     returnHTML += "Updated " + showNiceLastConnect(remote3_lastconnect);
        returnHTML += "</div>";
        returnHTML += "</div>";
     }
  } else {
    
  }
  
return returnHTML;
  
}


/*
 * 
 * Show the badging information in HTML
 * 
 * */
String JAG_webserver::createBadgingInfo() {
  String returnHTML = "<table class=\"table\">";
    returnHTML += "<thead>";
     returnHTML += " <tr>";
       returnHTML += " <th scope=\"col\">User Name</th>";
        returnHTML += "<th scope=\"col\">Badge Amount</th>";
      returnHTML += "</tr>";
    returnHTML += "</thead>";
    returnHTML += "<tbody>";
    bool usersFound = false;
for (int i=0; i < 99; i++) {
  String only_user = badgingUser[i].substring(0,badgingUser[i].indexOf("_"));
     String user_count = String(badgingUser[i].substring(badgingUser[i].indexOf("_") + 1));
      if (badgingUser[i] != "") {
        usersFound = true;
        returnHTML += "<tr>";
        returnHTML += "<td>";
          returnHTML += only_user;
        returnHTML += "</td>";

        returnHTML += "<td>";
          returnHTML += user_count;
        returnHTML += "</td>";
        returnHTML += "</tr>";
        
      }
     }

     if (!usersFound) {
        returnHTML += "<tr>";
        returnHTML += "<td>";
          returnHTML += "Oops, nobody has badged recently!";
        returnHTML += "</td>";

        returnHTML += "<td>";
       
        returnHTML += "</td>";
        returnHTML += "</tr>";
     }
   returnHTML += "</tbody>";
     returnHTML += "</table>";
  return returnHTML;

}

String JAG_webserver::IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}


/*
 * 
 * Shows a nice time since last update for the web interface
 * 
 * */
String JAG_webserver::showNiceLastConnect(unsigned long last_connect) {
  String returnHTML = "";
  unsigned long difference  = millis() - last_connect;
  returnHTML += String(difference / 1000)+  "s ago";
  return returnHTML;
}

/*
 * 
 * Main function that parses the sensor values and shows them on the web interface
 * 
 * */

String JAG_webserver::createHTMLpageWithContent_(String theContent) {
  String HTML_Content = "<html>";
  HTML_Content += "<head>";

  HTML_Content += "<script>\r\n";
  
    HTML_Content += "function refreshPage() {\r\n";
    HTML_Content += "   location.reload()\r\n";
    HTML_Content += "}\r\n";
    
    HTML_Content += "\r\n";
    
    HTML_Content += "function onPageLoad() {\r\n";

    //gas 1 chart info
    if (remote1_showing) {
      HTML_Content += " const ctx_gas_1_chart = document.getElementById('gas_1_chart');\r\n";
        HTML_Content += " const chart_gas_1_chart = new Chart(ctx_gas_1_chart, {\r\n";
        HTML_Content += "   type: 'line',\r\n";
        HTML_Content += "   data: GAS_1_data,\r\n";
        HTML_Content += " });\r\n";

        HTML_Content += "\r\n";
    }
    

    //gas 2 chart info
     if (remote2_showing) {
      HTML_Content += " const ctx_gas_2_chart = document.getElementById('gas_2_chart');\r\n";
      HTML_Content += " const chart_gas_2_chart = new Chart(ctx_gas_2_chart, {\r\n";
          HTML_Content += "   type: 'line',\r\n";
          HTML_Content += "   data: GAS_2_data,\r\n";
          HTML_Content += " });\r\n";
  
          HTML_Content += "\r\n";
     }

    //gas 3 chart info
     if (remote3_showing) {
        HTML_Content += " const ctx_gas_3_chart = document.getElementById('gas_3_chart');\r\n";
        HTML_Content += " const chart_gas_3_chart = new Chart(ctx_gas_3_chart, {\r\n";
            HTML_Content += "   type: 'line',\r\n";
            HTML_Content += "   data: GAS_3_data,\r\n";
            HTML_Content += " });\r\n";
    
            HTML_Content += "\r\n";

     }


          //temp 1 chart info

           if (remote1_showing) {
              HTML_Content += " const ctx_temp_1_chart = document.getElementById('temp_1_chart');\r\n";
              HTML_Content += " const chart_temp_1_chart = new Chart(ctx_temp_1_chart, {\r\n";
                  HTML_Content += "   type: 'line',\r\n";
                  HTML_Content += "   data: TEMP_1_data,\r\n";
                  HTML_Content += " });\r\n";
          
                  HTML_Content += "\r\n";

           }

    //temp 2 chart info
     if (remote2_showing) {
        HTML_Content += " const ctx_temp_2_chart = document.getElementById('temp_2_chart');\r\n";
        HTML_Content += " const chart_temp_2_chart = new Chart(ctx_temp_2_chart, {\r\n";
            HTML_Content += "   type: 'line',\r\n";
            HTML_Content += "   data: TEMP_2_data,\r\n";
            HTML_Content += " });\r\n";
    
            HTML_Content += "\r\n";
     }

    //temp 3 chart info
     if (remote3_showing) {
        HTML_Content += " const ctx_temp_3_chart = document.getElementById('temp_3_chart');\r\n";
        HTML_Content += " const chart_temp_3_chart = new Chart(ctx_temp_3_chart, {\r\n";
            HTML_Content += "   type: 'line',\r\n";
            HTML_Content += "   data: TEMP_3_data,\r\n";
            HTML_Content += " });\r\n";
    
            HTML_Content += "\r\n";

     }



          //humidity 1 chart info
           if (remote1_showing) {
              HTML_Content += " const ctx_humidity_1_chart = document.getElementById('humidity_1_chart');\r\n";
              HTML_Content += " const chart_humidity_1_chart = new Chart(ctx_humidity_1_chart, {\r\n";
                  HTML_Content += "   type: 'line',\r\n";
                  HTML_Content += "   data: HUMIDITY_1_data,\r\n";
                  HTML_Content += " });\r\n";
          
                  HTML_Content += "\r\n";

           }

    //humidity 2 chart info
     if (remote2_showing) {
        HTML_Content += " const ctx_humidity_2_chart = document.getElementById('humidity_2_chart');\r\n";
        HTML_Content += " const chart_humidity_2_chart = new Chart(ctx_humidity_2_chart, {\r\n";
            HTML_Content += "   type: 'line',\r\n";
            HTML_Content += "   data: HUMIDITY_2_data,\r\n";
            HTML_Content += " });\r\n";
    
            HTML_Content += "\r\n";
     }
    //humidity 3 chart info
     if (remote3_showing) {
          HTML_Content += " const ctx_humidity_3_chart = document.getElementById('humidity_3_chart');\r\n";
          HTML_Content += " const chart_humidity_3_chart = new Chart(ctx_humidity_3_chart, {\r\n";
              HTML_Content += "   type: 'line',\r\n";
              HTML_Content += "   data: HUMIDITY_3_data,\r\n";
              HTML_Content += " });\r\n";
      
              HTML_Content += "\r\n";

     }
        
        
    HTML_Content += "setInterval(function () {\r\n";

        HTML_Content +="    var current_sec_div = document.getElementById('up_time')\r\n ";
        HTML_Content +="    var current_sec = parseInt(current_sec_div.innerHTML)\r\n ";
        HTML_Content +="    current_sec_div.innerHTML = current_sec + 1\r\n ";
    
    HTML_Content += "}, 1000);\r\n";
    HTML_Content += "}\r\n";
  HTML_Content += "</script>";
  HTML_Content += "<meta http-equiv=\"refresh\" content=\"5\">";
  HTML_Content += "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">";
  HTML_Content += "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p\" crossorigin=\"anonymous\"></script>";
  HTML_Content += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>";
  HTML_Content += "<script>\r\n";
    HTML_Content += createJSDataForGAS("1");
       HTML_Content += "\r\n";
    HTML_Content += createJSDataForGAS("2");
       HTML_Content += "\r\n";
    HTML_Content += createJSDataForGAS("3");
      HTML_Content += "\r\n";
   

      HTML_Content += createJSDataForTEMP("1");
       HTML_Content += "\r\n";
    HTML_Content += createJSDataForTEMP("2");
       HTML_Content += "\r\n";
    HTML_Content += createJSDataForTEMP("3");
      HTML_Content += "\r\n";


       HTML_Content += createJSDataForHUMIDITY("1");
       HTML_Content += "\r\n";
    HTML_Content += createJSDataForHUMIDITY("2");
       HTML_Content += "\r\n";
    HTML_Content += createJSDataForHUMIDITY("3");
      HTML_Content += "\r\n";
  HTML_Content += "</script>";
  
  HTML_Content += "</head>";
  HTML_Content += "<body onload=\"onPageLoad()\" style=\"padding:10px\">";
  HTML_Content += theContent;
  HTML_Content += "</body>";

  HTML_Content += "</html>";
  return HTML_Content;

}

/*
 * 
 * Group of functions that add sensor values to an array so that they can be show in a graph
 * 
 * */

String JAG_webserver::createJSDataForGAS(String stationId) {
  String returnJS = "";
  if (stationId == "1") {
      returnJS += "const GAS_1_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote1_GAS_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'Gas Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote1_GAS_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  } else if (stationId == "2") {
     returnJS += "const GAS_2_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote2_GAS_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'Gas Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote2_GAS_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  } else if (stationId == "3") {
    returnJS += "const GAS_3_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote3_GAS_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'Gas Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote3_GAS_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  }
 
       return returnJS;
}


String JAG_webserver::createJSDataForTEMP(String stationId) {
  String returnJS = "";
  if (stationId == "1") {
      returnJS += "const TEMP_1_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote1_TEMP_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'TEMP Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote1_TEMP_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  } else if (stationId == "2") {
     returnJS += "const TEMP_2_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote2_TEMP_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'TEMP Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote2_TEMP_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  } else if (stationId == "3") {
    returnJS += "const TEMP_3_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote3_TEMP_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'TEMP Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote3_TEMP_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  }
 
       return returnJS;
}



void JAG_webserver::addGasValueToArray(String value, String stationId) {
  if (stationId == "1") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote1_GAS_history[i];
        if (this_value == "") {
          remote1_GAS_history[i] = value;
          break;
        }
      }    
  } else if (stationId == "2") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote2_GAS_history[i];
        if (this_value == "") {
          remote2_GAS_history[i] = value;
          break;
        }
      }    
  } else if (stationId == "3") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote3_GAS_history[i];
        if (this_value == "") {
          remote3_GAS_history[i] = value;
          break;
        }
      }    
  }
}

void JAG_webserver::addTempValueToArray(String value, String stationId) {
   if (stationId == "1") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote1_TEMP_history[i];
        if (this_value == "") {
          remote1_TEMP_history[i] = value;
          break;
        }
      }    
  } else if (stationId == "2") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote2_TEMP_history[i];
        if (this_value == "") {
          remote2_TEMP_history[i] = value;
          break;
        }
      }    
  } else if (stationId == "3") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote3_TEMP_history[i];
        if (this_value == "") {
          remote3_TEMP_history[i] = value;
          break;
        }
      }    
  }
}

void JAG_webserver::addHumidityValueToArray(String value, String stationId) {
   if (stationId == "1") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote1_HUMIDITY_history[i];
        if (this_value == "") {
          remote1_HUMIDITY_history[i] = value;
          break;
        }
      }    
  } else if (stationId == "2") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote2_HUMIDITY_history[i];
        if (this_value == "") {
          remote2_HUMIDITY_history[i] = value;
          break;
        }
      }    
  } else if (stationId == "3") {
      for (int i=0; i < 99; i++) {
        String this_value =  remote3_HUMIDITY_history[i];
        if (this_value == "") {
          remote3_HUMIDITY_history[i] = value;
          break;
        }
      }    
  }
}


String JAG_webserver::createJSDataForHUMIDITY(String stationId) {
  String returnJS = "";
  if (stationId == "1") {
      returnJS += "const HUMIDITY_1_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote1_HUMIDITY_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'HUMIDITY Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote1_HUMIDITY_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  } else if (stationId == "2") {
     returnJS += "const HUMIDITY_2_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote2_HUMIDITY_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);// 
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'HUMIDITY Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote2_HUMIDITY_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  } else if (stationId == "3") {
    returnJS += "const HUMIDITY_3_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  remote3_HUMIDITY_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'HUMIDITY Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  remote3_HUMIDITY_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
          // returnJS = returnJS.substring(0,returnJS.length() -1);
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
  }
 
       return returnJS;
}
/*
 * 
 * Some helper functions
 * 
 * */
void JAG_webserver::printOutGas() {
  Serial.println("Prinitng GAS:");
   for (int i=0; i < 99; i++) {
        String this_value =  remote2_GAS_history[i];
        if (this_value != "") {
          Serial.println(String(i) + " : " + this_value);
        }
        
      }
  }
