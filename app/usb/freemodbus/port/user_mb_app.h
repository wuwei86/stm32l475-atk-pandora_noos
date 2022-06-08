#ifndef    USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START                    0
#define S_DISCRETE_INPUT_NDISCRETES               8
#define S_COIL_START                              0
#define S_COIL_NCOILS                             24
#define S_REG_INPUT_START                         0
#define S_REG_INPUT_NREGS                         8
#define S_REG_HOLDING_START                       0
#define S_REG_HOLDING_NREGS                       8
/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     RT_S_HD_RESERVE
/* salve mode: input register's all address */
#define          S_IN_RESERVE                     RT_S_IN_RESERVE
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     RT_S_CO_RESERVE
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     RT_S_DI_RESERVE

/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START                    0
#define M_DISCRETE_INPUT_NDISCRETES               8
#define M_COIL_START                              0
#define M_COIL_NCOILS                             8
#define M_REG_INPUT_START                         0
#define M_REG_INPUT_NREGS                         8
#define M_REG_HOLDING_START                       0
#define M_REG_HOLDING_NREGS                       8
/* master mode: holding register's all address */
#define          M_HD_RESERVE                     RT_M_HD_RESERVE
/* master mode: input register's all address */
#define          M_IN_RESERVE                     RT_M_IN_RESERVE
/* master mode: coil's all address */
#define          M_CO_RESERVE                     RT_M_CO_RESERVE
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     RT_M_DI_RESERVE



extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES];
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS];
extern USHORT   usSRegInBuf[S_REG_INPUT_NREGS];
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS];

#endif
