/*
 * Author       : Kevin Black
 * Date written : 29 June 2007
 * Licensing    :
 *
 * This software is released under the MIT license.
 *
    Copyright (c) 2007 Kevin Black

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

 * Synopsis     :
 *
 * Defines the I2C Manager's published interface.
 *
 * The majority of the documentation occurs in i2c_manager.c.
 */
#ifndef __I2C_MANAGER_H__
#define __I2C_MANAGER_H__

/*******************************************************************************
 *
 * C++ linkage conventions
 *
 * Routines specified by this file need C linkage under C++
 */
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * Configuration options (to be #defined before #including this file!)
 *
 * I2CM_CONFIG_MAKESTATIC
 *         o If defined, then the interface functions will be declared static.
 */

/*******************************************************************************
 *
 * typedefs for data sizes
 *
 * Whilst an "int" can be used for a I2C slave address, the original I2C reference
 * design had a 7-bit address space, but new extended addressing modes may have
 * 10-bit addressing. In a microcontroller environment it makes sense to reduce
 * the size of certain data items. Therefore the size of arguments and items in
 * data structures of the I2C code is typdefed to "appropriate sizes". Using the
 * example given, a 7-bit address will fit in a signed byte if that is the extent
 * of the addressing that may be required on any given platform.
 *
 * NOTE: I have tried to use a nomenclature that might be common, such as int8_t
 *       for a signed 8-bit integer and uint8_t for an unsigned 8-bit integer,
 *       where such is required. This nomenclature may already be in use depending
 *       on compiler/target platform.
 *
 *       Additional type are declared, such as int_addr_r for integers describing an
 *       I2C device address specification and int_t for a general purpose integer
 *       that might be 8, 16 or 32 bits wide depending in platform.
 *
 *       If it really doesn't matter about the type of a variable or to avoid
 *       size problems that could conceivably be generated by using int_t, then
 *       standard type declarations will be used.
 *
 * This is all because I currently intend to eventually run some form of the code on
 * a microcontroller (for example the AVR), where space usage may prove to be important.
 */
typedef unsigned char byte_t;
#if defined(_MSC_VER) || defined(_WINDOWS) || defined (__unix) || defined(__linux) || defined(__linux__)
typedef int int_t;
typedef char int8_t;
typedef short int int16_t;
typedef long int32_t;
typedef __int64 int64_t;
typedef unsigned int uint_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;
typedef unsigned int bool_t;
typedef unsigned short int int_devaddr_t;
typedef unsigned long int_memaddr_t;
#endif

#ifdef HW_MCU_AVR
#error Untested
typedef char int_t;
typedef char int8_t;
typedef short int int16_t;
typedef long int32_t;
typedef long long int64_t;
typedef unsigned char uint_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;
typedef unsigned bool_t;
typedef unsigned char int_devaddr_t; /* NOTE: May not meet your needs */
typedef unsigned char int_memaddr_t; /* NOTE: May not meet your needs */
#endif

/*******************************************************************************
 *
 * TRUE and FALSE macros, assumption: if there is no TRUE macro, then there is no FALSE.
 */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*******************************************************************************
 *
 * Standardised I2C layer return codes i.e. values that will be returned by
 * the the I2C layer functions, and can be retrieved by the I2CM_GetLastError
 * function.
 *
 * NOTE: Additional codes may be implemented by an I2C layer implementation,
 *       these should start at 100, and prefferably be no greater than 127.
 *
 *       Rather than using enum, the values are #defined so that later versions
 *       of the software uses values that have the same meaning.
 *
 * WARNING: Codes may be restricted to the range -128 to 127 on some platforms.
 */
#define I2CM_SUCCESS                    0
#define I2CM_ERR_ALREADYINITIALISED     1 /* An attempt was made to initialisa an already initialised sub-system */
#define I2CM_ERR_NOTINITIALISED         2 /* An attempt was made to use a facility from an uninitialised sub-system */
#define I2CM_ERR_HARDWARE               3 /* I2C interface hardware error (e.g. not found) */
#define I2CM_ERR_SOFTWARE               4 /* A software problem (e.g. an internal error detected) */
#define I2CM_ERR_OUTOFMEMORY            5 /* What it says */
#define I2CM_ERR_BADARGS                6 /* Function was called with invalid or unusable arguments */
#define I2CM_ERR_NOTRANSACTION          7 /* No current transaction started */
#define I2CM_ERR_TRANSACTIONEXISTS      8 /* A transaction is already started */
#define I2CM_ERR_NOACK                  9 /* No acknowledgement received from I2C device */
#define I2CM_ERR_UNKNOWN               99 /* Un unknown error had occurred */

/*******************************************************************************
 *
 * Define the function interfaces
 */
/*
 * Prefered packing
 */
#pragma pack(push)
#pragma pack(1)

#ifdef I2CMSTATIC
#error "I2CMSTATIC should not be defined at this point..."
#endif
#ifdef I2CM_CONFIG_MAKESTATIC
#define I2CMSTATIC static
#else
#define I2CMSTATIC
#endif

typedef int (*I2CMPROC)();
typedef I2CMPROC (*I2CMPROCGETFNADDR) (int_t);
I2CMSTATIC I2CMPROC I2CM_GetProcAddress(int_t nfunc);

/*
 * Main layers interface
 */
#define I2CM_FID_INIT 1
I2CMSTATIC int_t I2CM_Init();
typedef int_t (*I2CM_INITFUNC) ();

#define I2CM_FID_GETLASTERROR 2
I2CMSTATIC int_t I2CM_GetLastError();
typedef int_t (*I2CM_GETLASTERRORFUNC) ();

#define I2CM_FID_GETLASTERRORTEXT 3
I2CMSTATIC const char *I2CM_GetLastErrorText();
typedef const char *(*I2CM_GETLASTERRORTEXTFUNC) ();

#define I2CM_FID_SETLASTERROR 4
I2CMSTATIC int_t I2CM_SetLastError(int_t nerror, const char *ptext);
typedef int_t (*I2CM_SETLASTERRORFUNC) (int_t nerror, const char *ptext);

I2CMSTATIC int_t I2CM_GetBusLastError(int_t nBus);
I2CMSTATIC const char *I2CM_GetBusLastErrorText(int_t nBus);
I2CMSTATIC int_t I2CM_SetBusLastError(int_t nBus, int_t nerror, const char *ptext);

#define I2CM_FID_ISINITIALISED 5
I2CMSTATIC int_t I2CM_IsInitialised();
typedef int_t (*I2CM_ISINITIALISEDFUNC) ();

#define I2CM_FID_SHUTDOWN 6
I2CMSTATIC int_t I2CM_Shutdown();
typedef int_t (*I2CM_SHUTDOWNFUNC) ();

#define I2CM_FID_GETBUSCOUNT 7
I2CMSTATIC int_t I2CM_GetBusCount();
typedef int_t (*I2CM_GETBUSCOUNTFUNC) ();

#define I2CM_FID_GETBUSHARDWARENAME 8
I2CMSTATIC int_t I2CM_GetBusHardwareName(int_t nBus, char *pbuffer, int_t n);
typedef int_t (*I2CM_GETBUSHARDWARENAMEFUNC) (int_t nBus, char *pbuffer, int_t n);

#define I2CM_FID_GETBUSNAME 9
I2CMSTATIC int_t I2CM_GetBusName(int_t nBus, char *pbuffer, int_t n);
typedef int_t (*I2CM_GETBUSNAMEFUNC) (int_t nBus, char *pbuffer, int_t n);

#define I2CM_FID_GETBUSID 10
I2CMSTATIC int_t I2CM_GetBusID(int_t nBus, unsigned long *pbusid);
typedef int_t (*I2CM_GETBUSIDFUNC) (int_t nBus, unsigned long *pbusid);

#define I2CM_FID_RESCANFORDEVICES 11
I2CMSTATIC int_t I2CM_RescanForDevices();
typedef int_t (*I2CM_RESCANFORDEVICESFUNC) ();

#define I2CM_FID_GETDEVICECOUNT 12
I2CMSTATIC int_t I2CM_GetDeviceCount();
typedef int_t (*I2CM_GETDEVICECOUNTFUNC) ();

#define I2CM_FID_GETBUSDEVICECOUNT 13
I2CMSTATIC int_t I2CM_GetBusDeviceCount(int_t nBus);
typedef int_t (*I2CM_GETBUSDEVICECOUNTFUNC) (int_t nBus);

#define I2CM_FID_GETDEVICEADDRESS 14
I2CMSTATIC int_t I2CM_GetDeviceAddress(int_t nBus, int_t nDevice);
typedef int_t (*I2CM_GETDEVICEADDRESSFUNC) (int_t nBus, int_t nDevice);

#define I2CM_FID_READ 15
I2CMSTATIC int_t I2CM_Read(int_t nBus, int_devaddr_t nSlaveAddress, int_t nMemAddressLen, int_memaddr_t nMemAddress, int_t nBytes, byte_t *pBuffer);
typedef int_t (*I2CM_READFUNC) (int_t nBus, int_devaddr_t nSlaveAddress, int_t nMemAddressLen, int_memaddr_t nMemAddress, int_t nBytes, BYTE *pBuffer);

#define I2CM_FID_WRITE 16
I2CMSTATIC int_t I2CM_Write(int_t nBus, int_devaddr_t nSlaveAddress, int_t nMemAddressLen, int_memaddr_t nMemAddress, int_t nBytes, const byte_t *pBuffer);
typedef int_t (*I2CM_WRITEFUNC) (int_t nBus, int_devaddr_t nSlaveAddress, int_t nMemAddressLen, int_memaddr_t nMemAddress, int_t nBytes, const byte_t *pBuffer);

#define I2CM_FID_STARTTRANSACTION 17
I2CMSTATIC int_t I2CM_StartTransaction(int_t nBus);
typedef int_t (*I2CM_STARTTRANSACTIONFUNC) (int_t nBus);

#define I2CM_FID_RESTARTTRANSACTION 18
I2CMSTATIC int_t I2CM_RestartTransaction(int_t nBus);
typedef int_t (*I2CM_RESTARTTRANSACTIONFUNC) (int_t nBus);

#define I2CM_FID_STOPTRANSACTION 19
I2CMSTATIC int_t I2CM_StopTransaction(int_t nBus);
typedef int_t (*I2CM_STOPTRANSACTIONFUNC) (int_t nBus);

#define I2CM_FID_GETBYTE 20
I2CMSTATIC int_t I2CM_GetByte(int_t nBus, byte_t *pnByte);
typedef int_t (*I2CM_GETBYTEFUNC) (int_t nBus, byte_t *pnByte);

#define I2CM_FID_PUTACK 21
I2CMSTATIC int_t I2CM_PutAck(int_t nBus, bool_t bAck);
typedef int_t (*I2CM_PUTACKFUNC) (int_t nBus, bool_t bAck);

#define I2CM_FID_PUTBYTE 22
I2CMSTATIC int_t I2CM_PutByte(int_t nBus, byte_t nByte);
typedef int_t (*I2CM_PUTBYTEFUNC) (int_t nBus, byte_t nByte);

#define I2CM_FID_GETACK 23
I2CMSTATIC int_t I2CM_GetAck(int_t nBus, bool_t *pbAck);
typedef int_t (*I2CM_GETACKFUNC) (int_t nBus, bool_t *pbAck);

/*
 * Global interface
 */
int I2CM_GetBytePutAck(int_t nBus, byte_t *pnByte, bool_t bAck);
int I2CM_PutByteGetAck(int_t nBus, byte_t nByte, bool_t *pbAck);

/*
 * Management interface
 */
int I2CManager_AddInterface(I2CMPROCGETFNADDR pgpaf); 
int I2CManager_ExternalAddInterfaces();

#pragma pack(pop) /* End of prefered packing */

#ifdef __cplusplus /* End of C++ linkage conventions */
}
#endif
#endif /* __I2C_MANAGER_H__ */
