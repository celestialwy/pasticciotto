#ifndef VMAS_H
#define VMAS_H

#include <stdint.h>
#include "debug.h"

#define DEFAULT_STACKSIZE 0x100
#define DEFAULT_CODESIZE 0x300
#define DEFAULT_DATASIZE 0x100

class VMAddrSpace {
private:
    uint32_t stacksize, codesize, datasize;

public:
    VMAddrSpace();

    VMAddrSpace(uint32_t ss, uint32_t cs, uint32_t ds);

    ~VMAddrSpace();

    uint8_t *stack, *code, *data;

    bool allocate(void);

    bool insStack(uint8_t *buf, uint32_t size);

    bool insCode(uint8_t *buf, uint32_t size);

    bool insData(uint8_t *buf, uint32_t size);

    template <typename src_t, typename dst_t>
    bool getArgs(uint32_t idx, src_t * src, dst_t * dst) {
        // both regs
        if (sizeof(*src) == sizeof(*dst)) {
            if (idx + 1 >= codesize) {
                DBG_ERROR(("Argument out of code segment bounds.\n"));
                return false;
            }
            *dst = code[idx + 1] >> 4;
            *src = code[idx + 1] & 0b00001111;
        }
        else {
            /*
             * OP DST SRC
             * DST = IP + 1
             * SRC = IP + 1 + SIZE(DST)
             */
            if (idx + sizeof(*dst) >= codesize) {
                DBG_ERROR(("Argument out of code segment bounds.\n"));
                return false;
            }

            *dst = *((dst_t *)&code[idx + 1]);
            *src = *((src_t *)&code[idx + 1 + sizeof(*dst)]);
        }
        return true;
    }

    template <typename T>
    bool getArgs(uint32_t ip, T * arg) {
        if (ip + 1 >= codesize) {
            DBG_ERROR(("Argument out of code segment bounds.\n"));
            return false;
        }
        *arg = *((T *)&code[ip + 1]);
        return true;
    }
};

#endif