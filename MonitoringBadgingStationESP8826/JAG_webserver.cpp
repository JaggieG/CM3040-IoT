#include "Arduino.h"
#include "JAG_webserver.h"

// initialise

JAG_webserver::JAG_webserver(int port, bool logSerial)
{
  port_ = port;
  logSerial_ = logSerial;
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
   String message = "<h2>Welcome to JAG ESP - IoT Monitroing and Badging Station</h2>";
        message += "Waiting for a card to be presented<br />"; 
        message += "<br />";
        message += "<br />";
        message += "Local Station Details<br />"; 
  return message;
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
      HTML_Content += "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">";
      HTML_Content += "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p\" crossorigin=\"anonymous\"></script>";
    HTML_Content += "</head>";
    HTML_Content += "<body style=\"padding:10px\">";
      HTML_Content += theContent;
    HTML_Content += "</body>";

    HTML_Content += "</html>";
    return HTML_Content;
}
