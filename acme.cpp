#include <iostream>
#include "trsio.h"

using namespace std;


#define ACME_MODULE_ID 0

/*
 * Class ACMEModule demonstrates how to implement a new module for TRS-IO.
 * The ACME_MODULE_ID is a unique, one-byte module identifier. The first byte sent by
 * the TRS-80 will determine to which module a request is dispatched. A module can have
 * one or more so-called commands. Each command represents a unique action that is performed
 * by the module (e.g., open or close a file, etc). Each command maps to a static method
 * of the class. The second byte sent by the TRS-80 identifies the command. Commands are
 * numbered starting from 0. New commands have to be registered in the constructor of a
 * module via the addCommand() method. The first parameter to addCommand() is the function
 * pointer to the static method. The second parameter is a string that represents the
 * commands signature. The signature defines the parameters that the TRS-80 is expected to
 * send. The signature is a list of mandatory parameters where each parameter is represented
 * by a letter denoting the type:
 *   B: one byte
 *   I: 16-bit integer
 *   L: 32-bit integer
 *   S: Zero terminated string
 *   Z: variable length binary blob
 * E.g., Signature "ISB" represents three parameters, first a 16-bit integer (I), followed
 * by a zero-terminated string (S), followed by a byte (B). The TRS-80 needs to send the
 * parameters in that sequence. For a binary blob, the TRS-80 first needs to send a
 * 16-bit length field (in little endian encoding) followed by the binary blob of that length.
 *
 * Once the TRS-80 has sent all parameters, TRS-IO will call the appropriate method that
 * implements the behavior. Within the method, the parameters can be accessed via helper
 * functions:
 *   b(n): returns the n-th byte
 *   i(n): returns the n-th 16-bit integer
 *   l(n): returns the n-th 32-bit integer
 *   s(n): returns a pointer to the n-th string
 *   z(n): returns a uint_8* pointer to the n-th binary blob
 *   zl(n): returns the length of the n-th binary blob
 */
class ACMEModule : public virtual TrsIO {
public:
    ACMEModule() : TrsIO(ACME_MODULE_ID) {
        /*
         * Register four commands with different signatures
         */
        addCommand(cmd1, "II");
        addCommand(cmd2, "IBLBI");
        addCommand(cmd3, "BSB");
        addCommand(cmd4, "ZB");
    }

    static void cmd1() {
        // Signature: II
        cout << "cmd1(" << i(0) << ", " << i(1) << ")\n";
    }

    static void cmd2() {
        // Signature IBLBI
        cout << "cmd2(" << i(0) << ", " << (int) b(0) << ", " << l(0) << ", " << (int) b(1) << ", " << i(1) << ")\n";
    }

    static void cmd3() {
        // Signature BSB
        cout << "cmd3(" << (int) b(0) << ", " << s(0) << ", " << (int) b(1) << ")\n";
    }

    static void cmd4() {
        // Signature ZB
        uint8_t* p = z(0);
        cout << "cmd3(len=" << zl(0) << ", ";
        for (int i = 0; i < zl(0); i++) {
            cout << (int) *p++ << ", ";
        }
        cout << (int) b(0) << ")\n";
    }
};

static ACMEModule theACMEModule;
