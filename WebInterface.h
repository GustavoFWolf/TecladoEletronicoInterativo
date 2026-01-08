#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include "Config.h"

extern const char index_html[] PROGMEM;

void handleRoot();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

void setupWebInterface();
void loopWebInterface();

#endif
