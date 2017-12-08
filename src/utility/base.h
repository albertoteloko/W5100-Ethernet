#ifndef BASE_H
#define BASE_H


class Base {
public:
  uint8_t* rawIPAddress(IPAddress addr) {
    return (uint8_t*)&addr.raw().ipv4;
  };
};


#endif