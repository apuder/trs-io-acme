#include <iostream>
#include "trsio.h"

using namespace std;

/*
 * The sample sequence below represents bytes sent by the TRS-80 over the
 * expansion board. A total of four commands are sent (one command per line).
 * The first byte in a command is always 0x00 which will target the ACME
 * module. The next byte represents the command that corresponds to method
 * that will be invoked. The remaining bytes represent the parameter. Refer
 * to the signature of methods cmd1(), cmd2(), cmd3() and cmd4() of the ACME
 * module.
 */
static uint8_t sample_seq[] = {0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
                               0x00, 0x01, 1, 0, 42, 2, 0, 0, 0, 5, 0, 1,
                               0x00, 0x02, 1, 65, 66, 67, 0, 2,
                               0x00, 0x03, 3, 0, 1, 2, 3, 4};
static uint8_t read_byte() {
    static uint16_t i = 0;

    return sample_seq[i++];
}

static void io_task(int n) {
    for (int i = 0; i < n; i++) {
        TrsIO *module = TrsIO::getModule(read_byte());
        while (TrsIO::consumeNextByteFromZ80(read_byte()));
        module->process();
    }
}

int main() {
    TrsIO::initModules();
    io_task(4);
    return 0;
}