
#include "trsio.h"

using namespace std;


TrsIO* TrsIO::modules[TRSIO_MAX_MODULES] = {};

uint8_t* TrsIO::sendBuffer = nullptr;

uint16_t TrsIO::sendBufferLen = 0;

uint8_t TrsIO::receiveBuffer[TRSIO_MAX_RECEIVE_BUFFER];
uint8_t* TrsIO::receivePtr;

TrsIO* TrsIO::currentModule;

uint8_t TrsIO::cmd;
state_t TrsIO::state;
uint16_t TrsIO::bytesToRead;

const char* TrsIO::signatureParams;

uint8_t* TrsIO::param_bytes[TRSIO_MAX_PARAMETERS_PER_TYPE];
uint8_t* TrsIO::param_ints[TRSIO_MAX_PARAMETERS_PER_TYPE];
uint8_t* TrsIO::param_longs[TRSIO_MAX_PARAMETERS_PER_TYPE];
uint8_t* TrsIO::param_strs[TRSIO_MAX_PARAMETERS_PER_TYPE];
uint8_t* TrsIO::param_blocks[TRSIO_MAX_PARAMETERS_PER_TYPE];
uint16_t TrsIO::param_blocks_len[TRSIO_MAX_PARAMETERS_PER_TYPE];


uint16_t TrsIO::paramCurrentByte;
uint16_t TrsIO::paramCurrentInt;
uint16_t TrsIO::paramCurrentLong;
uint16_t TrsIO::paramCurrentStr;
uint16_t TrsIO::paramCurrentBlock;

void TrsIO::initModules() {
    for (auto &module : modules) {
        TrsIO* mod = module;
        if (mod != nullptr) {
            module->init();
        }
    }
    reset();
}

void TrsIO::reset() {
    state = NEW_CMD;
    paramCurrentByte = 0;
    paramCurrentInt = 0;
    paramCurrentLong = 0;
    paramCurrentStr = 0;
    paramCurrentBlock = 0;
    receivePtr = receiveBuffer;
}

bool TrsIO::consumeNextByteFromZ80(uint8_t byte) {
    static uint8_t b;

    switch (state) {
        case NEW_CMD:
            cmd = byte;
            signatureParams = currentModule->commands[cmd].signature;
            break;
        case ACCEPT_FIXED_LENGTH_PARAM:
            *receivePtr++ = byte;
            if (--bytesToRead == 0) {
                break;
            }
            return true;
        case ACCEPT_STRING_PARAM:
            *receivePtr++ = byte;
            if (byte == 0) {
                break;
            }
            return true;
        case ACCEPT_BLOCK_LEN_1:
            b = byte;
            state = ACCEPT_BLOCK_LEN_2;
            return true;
        case ACCEPT_BLOCK_LEN_2:
            bytesToRead = b | (byte << 8);
            param_blocks_len[paramCurrentBlock] = bytesToRead;
            param_blocks[paramCurrentBlock++] = receivePtr;
            state = ACCEPT_FIXED_LENGTH_PARAM;
            return true;
    }

    if (*signatureParams == '\0') {
        return false;
    }

    switch (*signatureParams++) {
        case 'B':
            param_bytes[paramCurrentByte++] = receivePtr;
            bytesToRead = 1;
            state = ACCEPT_FIXED_LENGTH_PARAM;
            break;
        case 'I':
            param_ints[paramCurrentInt++] = receivePtr;
            bytesToRead = 2;
            state = ACCEPT_FIXED_LENGTH_PARAM;
            break;
        case 'L':
            param_longs[paramCurrentLong++] = receivePtr;
            bytesToRead = 4;
            state = ACCEPT_FIXED_LENGTH_PARAM;
            break;
        case 'S':
            param_strs[paramCurrentStr++] = receivePtr;
            state = ACCEPT_STRING_PARAM;
            break;
        case 'Z':
            state = ACCEPT_BLOCK_LEN_1;
            break;
        default:
            //TODO bad signature string
            ;
    }
    return true;
}

void TrsIO::process() {
    commands[cmd].proc();
    reset();
}

void TrsIO::setSendBuffer(uint8_t* buffer, uint16_t bufferLen) {
    sendBuffer = buffer;
    sendBufferLen = bufferLen;
}

uint8_t* TrsIO::getSendBuffer() {
    return sendBuffer;
}

uint16_t TrsIO::getSendBufferLen() {
    return sendBufferLen;
}

uint8_t TrsIO::getNextByteFromSendBuffer() {
    if (sendBufferLen == 0) {
        return 0xff;
    }
    sendBufferLen--;
    return *sendBuffer++;
}
