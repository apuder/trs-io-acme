//
// Created by Arno Puder on 2/5/2019.
//

#pragma once

#include "inttypes.h"

#define TRSIO_MAX_MODULES 5
#define TRSIO_MAX_RECEIVE_BUFFER 1024
#define TRSIO_MAX_PARAMETERS_PER_TYPE 5

typedef void (*proc_t)();

typedef struct {
    proc_t proc;
    const char* signature;
} command_t;

enum state_t {
    NEW_CMD,
    ACCEPT_FIXED_LENGTH_PARAM,
    ACCEPT_STRING_PARAM,
    ACCEPT_BLOCK_LEN_1,
    ACCEPT_BLOCK_LEN_2
};

class TrsIO {
private:
    static TrsIO* modules[];

    static uint8_t receiveBuffer[];
    static uint8_t* receivePtr;

    static TrsIO* currentModule;

    static uint8_t cmd;
    static state_t state;
    static uint16_t bytesToRead;

    static const char* signatureParams;

    static uint8_t* param_bytes[];
    static uint8_t* param_ints[];
    static uint8_t* param_longs[];
    static uint8_t* param_strs[];
    static uint8_t* param_blocks[];
    static uint16_t param_blocks_len[];


    static uint16_t paramCurrentByte;
    static uint16_t paramCurrentInt;
    static uint16_t paramCurrentLong;
    static uint16_t paramCurrentStr;
    static uint16_t paramCurrentBlock;

    static uint8_t* sendBuffer;
    static uint16_t sendBufferLen;

    uint16_t numCommands;
    command_t commands[10];

protected:
    void addCommand(proc_t proc, const char* signature) {
        commands[numCommands].proc = proc;
        commands[numCommands].signature = signature;
        numCommands++;
    }
public:

    explicit TrsIO(int id) {
        modules[id] = this;
        numCommands = 0;
    }

    static void initModules();

    inline static TrsIO* getModule(int id) {
        currentModule = modules[id];
        return currentModule;
    }

    virtual void init() {
        // Do nothing in base class
    }

    static void reset();
    static bool consumeNextByteFromZ80(uint8_t byte);

protected:
    void setSendBuffer(uint8_t* buffer, uint16_t bufferLen);

    inline static uint8_t b(uint8_t idx) {
        return *param_bytes[idx];
    }

    static uint16_t i(uint8_t idx) {
        return *((uint16_t *) param_ints[idx]);
    }

    static uint32_t l(uint8_t idx) {
        return *((uint32_t *) param_longs[idx]);
    }

    static const char* s(uint8_t idx) {
        return (const char*) param_strs[idx];
    }

    static uint8_t* z(uint8_t idx) {
        return param_blocks[idx];
    }

    static uint16_t zl(uint8_t idx) {
        return param_blocks_len[idx];
    }

public:
    void process();
    uint8_t* getSendBuffer();
    uint16_t getSendBufferLen();
    static uint8_t getNextByteFromSendBuffer();
};
