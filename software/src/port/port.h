#ifndef PORT_H
#define PORT_H

#include "com_adapter.h"
#include "storage_adapter.h"

#include "bl_config.h"

/**
 ������������������������������
 ��                          ��
 ��        bootloader        ��
 ��                          ��
 ������������������������������
 ��         bl data          ��
 ������������������������������
 ��   exe area header(op)    ��
 ������������������������������
 ��                          ��
 ��                          ��
 ��        exe area          ��
 ��                          ��
 ��                          ��
 ������������������������������
 ��  backup area header(op)  ��
 ������������������������������
 ��                          ��
 ��                          ��
 ��       backup area        ��
 ��                          ��
 ��                          ��
 ������������������������������
 ��      customer data       ��
 ������������������������������
*/

#define ADDR_BL_AREA_FW                 (0x8000000)
#define ADDR_BL_AREA_FW_SIZE            (12*1024)
#define ADDR_BL_AREA_DATA               (ADDR_BL_AREA_FW + ADDR_BL_AREA_FW_SIZE)
#define ADDR_BL_AREA_DATA_SIZE          (2*1024)

#if FW_HEADER_ENABLE == 1
#define ADDR_EXE_AREA_HEADER            (ADDR_BL_AREA_DATA + ADDR_BL_AREA_DATA_SIZE)
#define ADDR_EXE_AREA_HEADER_SIZE       (2*1024)
#endif
#define ADDR_EXE_AREA_FW                (ADDR_BL_AREA_DATA + ADDR_BL_AREA_DATA_SIZE)
#define ADDR_EXE_AREA_FW_SIZE           (20*1024)

#if FW_HEADER_ENABLE == 1
#define ADDR_BACKUP_AREA_HEADER         (ADDR_EXE_AREA_FW + ADDR_EXE_AREA_FW_SIZE)
#define ADDR_BACKUP_AREA_HEADER_SIZE    (2*1024)
#endif
#define ADDR_BACKUP_AREA_FW             (ADDR_EXE_AREA_FW + ADDR_EXE_AREA_FW_SIZE)
#define ADDR_BACKUP_AREA_FW_SIZE        (20*1024)

extern com_adapter_str com_adapter;
extern storage_adapter_str storage_adapter;

#endif
