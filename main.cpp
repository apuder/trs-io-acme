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
static uint8_t seq1[] = {0x00, 0x00, 0x01, 0x00, 0x02, 0x00};
static uint8_t seq2[] = {0x00, 0x01, 1, 0, 42, 2, 0, 0, 0, 5, 0, 1};
static uint8_t seq3[] = {0x00, 0x02, 1, 65, 66, 67, 0, 2};
static uint8_t seq4[] = {0x00, 0x03, 3, 0, 1, 2, 3, 4};

static uint8_t* testCases[] = {seq1, seq2, seq3, seq4};
static uint16_t testCasesLen[] = {sizeof(seq1), sizeof(seq2), sizeof(seq3), sizeof(seq4)};

static void runTests() {
    int numTestCases = sizeof(testCases) / sizeof(uint8_t*);
    for (int i = 0; i < numTestCases; i++) {
        uint8_t* nextByte = testCases[i];
        int len = testCasesLen[i] - 2;
        uint16_t n = len;
        TrsIO *module = TrsIO::getModule(*nextByte++);
        while (TrsIO::consumeNextByteFromZ80(*nextByte++)) {
            n--;
        }
        assert(n == 0);
        module->process();
        assert(len == TrsIO::getSendBufferLen());
        uint8_t* orig = testCases[i] + 2;
        uint8_t* marshalled = TrsIO::getSendBuffer();
        for (int i = 0; i < len; i++) {
            assert(*orig++ == *marshalled++);
        }
    }
}

int main() {
    TrsIO::initModules();
    runTests();
    return 0;
}