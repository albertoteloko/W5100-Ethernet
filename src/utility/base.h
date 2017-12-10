#ifndef BASE_H
#define BASE_H


class Base {
public:
  void rawIPAddress(IPAddress address, uint8_t *dst) {
    uint8_t* src =  (uint8_t*)&address.raw().ipv4;

    uint8_t size = 4;
    for ( uint8_t i = 0; i < size; i++ ) {
        *(dst + i) = *(src + (size - i  - 1));
    }
  };
};

#endif