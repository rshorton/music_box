#include "util.hpp"

int hexCharToInt(char c1)
{
  if (c1 >= '0' && c1 <= '9') {
    return c1 - '0';
  } else if (c1 >= 'a' && c1 <= 'f') {
    return c1 - 'a' + 10;
  }
  return 0;
}

int hexToInt(char c1, char c2)
{
  return (hexCharToInt(c1) << 4) | hexCharToInt(c2);
}
