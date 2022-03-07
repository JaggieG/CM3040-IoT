#include "Arduino.h"
#include "JAG_webserver.h"

// initialise

JAG_webserver::JAG_webserver(int port, String my_name_, bool logSerial)
{
  port_ = port;
  logSerial_ = logSerial;
  my_showing = false;
  my_name = my_name_;
  my_lastbadge = "";
  ESP8266WebServer webserver_(port_);
}

// Public Functions

void JAG_webserver::setup() {
  setupRoutes();
  webserver.begin();
  if (logSerial_) {
    Serial.println("HTTP server available on port:");
    Serial.println(port_);
  }
  
}

void JAG_webserver::runServerInLoop() {
  webserver.handleClient();
  return;
}

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
  returnHTML += "<div style=\"text-align: center; width: 100%\"><h2>Welcome to JAG ESP - IoT Monitoring Station called " + my_name + "</h2></div>";
  returnHTML += "<br />";
  returnHTML += "<div class=\"container\">";
  returnHTML += "<div class=\"row\">";
  returnHTML += "<div class=\"col-sm border border-primary rounded \">";
  returnHTML += "<br />";
    returnHTML += "<b>Local Station Details</b><br /><br />";
  returnHTML += createStationInfo();
 
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
  webserver.on("/", [&](){
     webserver.send(200, "text/html", createHTMLpageWithContent_(getRootPageContents()));
  });

  webserver.onNotFound([&](){
     webserver.send(404, "text/html", createHTMLpageWithContent_(get404PageContents()));
  });
}

String JAG_webserver::createHTMLpageWithContent_(String theContent) {
  String HTML_Content = "<html>";
  HTML_Content += "<head>";

  HTML_Content += "<script>\r\n";
  
    HTML_Content += "function refreshPage() {\r\n";
    HTML_Content += "   location.reload()\r\n";
    HTML_Content += "}\r\n";
    
    HTML_Content += "\r\n";
    
    HTML_Content += "function onPageLoad() {\r\n";

    //gas my chart info
  
      HTML_Content += " const ctx_gas_my_chart = document.getElementById('gas_my_chart');\r\n";
        HTML_Content += " const chart_gas_1_chart = new Chart(ctx_gas_my_chart, {\r\n";
        HTML_Content += "   type: 'line',\r\n";
        HTML_Content += "   data: GAS_my_data,\r\n";
        HTML_Content += " });\r\n";

        HTML_Content += "\r\n";
    
    

    


          //temp my chart info

      
              HTML_Content += " const ctx_temp_my_chart = document.getElementById('temp_my_chart');\r\n";
              HTML_Content += " const chart_temp_my_chart = new Chart(ctx_temp_my_chart, {\r\n";
                  HTML_Content += "   type: 'line',\r\n";
                  HTML_Content += "   data: TEMP_my_data,\r\n";
                  HTML_Content += " });\r\n";
          
                  HTML_Content += "\r\n";

          

 

          //humidity my chart info
         
              HTML_Content += " const ctx_humidity_my_chart = document.getElementById('humidity_my_chart');\r\n";
              HTML_Content += " const chart_humidity_my_chart = new Chart(ctx_humidity_my_chart, {\r\n";
                  HTML_Content += "   type: 'line',\r\n";
                  HTML_Content += "   data: HUMIDITY_my_data,\r\n";
                  HTML_Content += " });\r\n";
          
                  HTML_Content += "\r\n";

          
        
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
  
    HTML_Content += createJSDataForGAS();
    HTML_Content += "\r\n";
    HTML_Content += createJSDataForTEMP();
    HTML_Content += "\r\n";
    HTML_Content += createJSDataForHUMIDITY();
     HTML_Content += "\r\n";
  HTML_Content += "</script>";
  
  HTML_Content += "</head>";
  HTML_Content += "<body onload=\"onPageLoad()\" style=\"padding:10px\">";
  HTML_Content += theContent;
  HTML_Content += "</body>";

  HTML_Content += "</html>";
  return HTML_Content;
}

String JAG_webserver::createJSDataForGAS() {
  String returnJS = "";
 returnJS += "const GAS_my_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  my_GAS_history[i];
            if (this_value != "") {
              returnJS += String(i) + ",";
            }
          }
         
          returnJS += "],\r\n";
          
        returnJS += " datasets: [{\r\n";
          returnJS += "   label: 'Gas Graph',\r\n";
          returnJS += "   data: [";
          for (int i=0; i < 99; i++) {
            String this_value =  my_GAS_history[i];
            if (this_value != "") {
              returnJS += this_value + ",";
            }
          }
         
          returnJS += "],\r\n";
          returnJS += "   fill: true,\r\n";
          returnJS += "   borderColor: 'rgb(75, 192, 192)',\r\n";
          returnJS += "   tension: 0.1,\r\n";
          returnJS += "   showLine: true\r\n";
        returnJS += "}]\r\n";
      returnJS += "};\r\n";
      return returnJS;
}

String JAG_webserver::createJSDataForTEMP() {
  String returnJS = "";
 returnJS += "const TEMP_my_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  my_TEMP_history[i];
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
            String this_value =  my_TEMP_history[i];
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
      return returnJS;
}

String JAG_webserver::createJSDataForHUMIDITY() {
  String returnJS = "";
 returnJS += "const HUMIDITY_my_data = {\r\n";
        returnJS += " labels: [";
         for (int i=0; i < 99; i++) {
            String this_value =  my_HUMIDITY_history[i];
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
            String this_value =  my_HUMIDITY_history[i];
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
      return returnJS;
}

String JAG_webserver::createStationInfo() {
  String returnHTML = "";
      returnHTML += "<div class=\"card\" style=\"width: 100%;\">";
      returnHTML += "<div class=\"card-body\">";
      returnHTML += "<h5 class=\"card-title\">" + my_name + "</h5>";

      
      returnHTML += "<h6 class=\"card-subtitle mb-2 text-muted\"></h6>";
      returnHTML += "<p class=\"card-text\">";
   
            returnHTML += "<h2>Last Badge: ";

            if (my_lastbadge == "") {
              returnHTML += "Never"; 
            } else {
              returnHTML += my_lastbadge;
            }
            
            returnHTML += "</h2><br / >";
      returnHTML += "Current Temp: " + my_TEMP + char(176) + " C<br / >";
      returnHTML += "<div><canvas id=\"temp_my_chart\" width=\"600\" height=\"400\"></canvas></div>";
      returnHTML += "Current Humidity: " + my_HUMIDITY + " %<br / >";
      returnHTML += "<div><canvas id=\"humidity_my_chart\" width=\"600\" height=\"400\"></canvas></div>";
      returnHTML += "Current Gas: " + my_GAS + " ppm<br / >";
      returnHTML += "<div><canvas id=\"gas_my_chart\" width=\"600\" height=\"400\"></canvas></div>";

      returnHTML += "</p>";
     

      returnHTML += "</div>";

    
      returnHTML += "</div>";


 return returnHTML;


  }

  String JAG_webserver::showNiceLastConnect(unsigned long last_connect) {
  String returnHTML = "";
  unsigned long difference  = millis() - last_connect;
  returnHTML += String(difference / 1000)+  "s ago";
  return returnHTML;
}


void JAG_webserver::addGasValueToArray(String value) {
  my_GAS = value;
      for (int i=0; i < 99; i++) {
        String this_value =  my_GAS_history[i];
        if (this_value == "") {
          my_GAS_history[i] = value;
          break;
        }
      }    
}

void JAG_webserver::addTempValueToArray(String value) {
  my_TEMP = value;
      for (int i=0; i < 99; i++) {
        String this_value =  my_TEMP_history[i];
        if (this_value == "") {
          my_TEMP_history[i] = value;
          break;
        }
      }    
}

void JAG_webserver::addHumidityValueToArray(String value) {
      my_HUMIDITY = value;
      for (int i=0; i < 99; i++) {
        String this_value =  my_HUMIDITY_history[i];
        if (this_value == "") {
          my_HUMIDITY_history[i] = value;
          break;
        }
      }    
}

void JAG_webserver::updateLocalLastBadged(String cardUsername) {
  my_lastbadge = cardUsername;
}
