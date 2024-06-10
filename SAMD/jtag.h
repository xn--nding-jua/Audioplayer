/*
Source: Florian Zachs, https://github.com/HerrNamenlos123/JTAG_Interface

MIT License

Copyright (c) 2020 Florian Zachs as HerrNamenlos123

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __JTAG_H__
#define __JTAG_H__

#include "Arduino.h"

/* States of JTAG State Machine */
#define JS_RESET         0
#define JS_RUNIDLE       1
#define JS_SELECT_IR     2
#define JS_CAPTURE_IR    3
#define JS_SHIFT_IR      4
#define JS_EXIT1_IR      5
#define JS_PAUSE_IR      6
#define JS_EXIT2_IR      7
#define JS_UPDATE_IR     8
#define JS_SELECT_DR     9
#define JS_CAPTURE_DR    10
#define JS_SHIFT_DR      11
#define JS_EXIT1_DR      12
#define JS_PAUSE_DR      13
#define JS_EXIT2_DR      14
#define JS_UPDATE_DR     15
#define JS_UNDEFINE      16

#define JSM_RESET_COUNT  5

#define JTAG_VENDOR_ID   0x6E
#define JTAG_ID_VJTAG    0x84

/* JTAG Instructions */
#define  JI_EXTEST				0x000
#define  JI_PROGRAM				0x002
#define  JI_STARTUP				0x003
#define  JI_CHECK_STATUS		0x004
#define  JI_SAMPLE				0x005
#define  JI_IDCODE				0x006
#define  JI_USERCODE			0x007
#define  JI_BYPASS				0x3FF
#define  JI_PULSE_NCONFIG		0x001
#define  JI_CONFIG_IO			0x00D
#define  JI_HIGHZ				0x00B
#define  JI_CLAMP				0x00A
#define  JI_ACTIVE_DISENGAGE	0x2D0
#define  JI_FACTORY				0x281
#define  JI_USER0_VDR			0x00C
#define  JI_USER1_VIR			0x00E

#define JBC_WRITE               0
#define JBC_READ                1

#define MAX_JTAG_INIT_CLOCK 3192
#define CDF_IDCODE_LEN 32

#define IDCODE 0x20F20DD
#define JSEQ_MAX 360
#define JSEQ_CONF_DONE 224
#define INST_LEN 10
#define INIT_COUNT 200

#if 1

#define TDI 12
#define TDO 15
#define TCK 13
#define TMS 14

#else

#define TDI 26
#define TDO 29
#define TCK 27
#define TMS 28

#endif

#ifdef __cplusplus
extern "C" {
#endif
int jtagInit(void);
int jtagReload(void);
int jtagWriteBuffer(unsigned int address, const uint8_t* data, size_t len);
int jtagReadBuffer(unsigned int address, uint8_t* data, size_t len);
void jtagDeinit(void);
int mbPinSet(void);
int mbCmdSend(uint32_t* data, int len);
int mbEveSend(uint32_t* data, int len);
int mbWrite(uint32_t address, void* data, int len);
int mbRead(uint32_t address, void* data, int len);
#ifdef __cplusplus
}
#endif

#endif
