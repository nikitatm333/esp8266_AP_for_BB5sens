#ifndef WEB_HANDLERS_H
#define WEB_HANDLERS_H

#include <ESP8266WebServer.h>

void initWebHandlers(ESP8266WebServer &server);
void handleRegPage();
void handleSetAP();

#endif // WEB_HANDLERS_H
