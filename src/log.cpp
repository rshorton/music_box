#include "log.hpp"

void log(const PROGMEM char * fmt, ...)
{
	char strBuffer[250];
	char buf[250];

  strncpy_P(strBuffer, fmt, sizeof(strBuffer));
  strBuffer[sizeof(strBuffer) - 1] = '\0';

  va_list args;
  va_start(args, strBuffer);

  //unsigned long t = millis();
  unsigned long t = micros();
  snprintf(buf, sizeof(buf), "%10lu  ", t);
  int len = strlen(buf);
  vsnprintf(&buf[len], sizeof(buf) - len, strBuffer, args);
  va_end(args);
  Serial.println(buf);
}
