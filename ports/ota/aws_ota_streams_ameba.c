/*
Amazon FreeRTOS OTA PAL for Realtek Ameba V1.0.0
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* OTA PAL implementation for Realtek Ameba platform. */
#include "ota.h"
#include "ota_demo_config.h"
#include "ota_pal_streams.h"
#include "ota_interface_private.h"
#include "ota_config.h"
#include "iot_crypto.h"
#include "core_pkcs11.h"
#include "example_amazon_freertos.h"
#include "flash_api.h"
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
#include "ameba_ota.h"
#include "ota_api.h"
#elif defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBAZ2)
#include "platform_opts.h"
#include "osdep_service.h"
#include "device_lock.h"
#include "sys_api.h"
#endif
#include "platform_stdlib.h"

#define OTA_MEMDUMP 0
#define OTA_PRINT DiagPrintf
#ifndef BUF_SIZE
#define BUF_SIZE 2048
#endif

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
static uint32_t aws_ota_imgaddr = 0;
static uint32_t aws_ota_imgsz = 0;
static bool aws_ota_target_hdr_get = false;
static uint32_t ota_target_index = OTA_INDEX_2;
static uint32_t HdrIdx = 0;
static ota_hdr_manager_t aws_ota_target_hdr;
ota_manifest_t aws_manifest_new;
static bool aws_manifest_get = false;
#elif defined(CONFIG_AMEBAD)
static uint32_t aws_ota_imgaddr = 0;
static uint32_t aws_ota_imgsz = 0;
static bool aws_ota_target_hdr_get = false;
static uint32_t ota_target_index = OTA_INDEX_2;
static uint32_t HdrIdx = 0;
static update_ota_target_hdr aws_ota_target_hdr;
static uint8_t aws_ota_signature[8] = {0};
#define AWS_OTA_IMAGE_SIGNATURE_LEN 8
#define OTA1_FLASH_START_ADDRESS    LS_IMG2_OTA1_ADDR    //0x08006000
#define OTA2_FLASH_START_ADDRESS    LS_IMG2_OTA2_ADDR    //0x08106000
#elif defined(CONFIG_AMEBAZ2)
#define AWS_OTA_IMAGE_SIGNATURE_LEN 32
static flash_t flash_ota;
unsigned char sig_backup[AWS_OTA_IMAGE_SIGNATURE_LEN];
#endif

#define AWS_OTA_IMAGE_STATE_FLAG_IMG_NEW             0xffffffffU /* 11111111b A new image that hasn't yet been run. */
#define AWS_OTA_IMAGE_STATE_FLAG_PENDING_COMMIT      0xfffffffeU /* 11111110b Image is pending commit and is ready for self test. */
#define AWS_OTA_IMAGE_STATE_FLAG_IMG_VALID           0xfffffffcU /* 11111100b The image was accepted as valid by the self test code. */
#define AWS_OTA_IMAGE_STATE_FLAG_IMG_INVALID         0xfffffff8U /* 11111000b The image was NOT accepted by the self test code. */

typedef struct {
    int32_t lFileHandle;
} ameba_ota_context_t;

static ameba_ota_context_t ota_ctx;

#if OTA_MEMDUMP
void vMemDump(u32 addr, const u8 *start, u32 size, char * strHeader)
{
    int row, column, index, index2, max;
    u8 *buf, *line;

    if(!start ||(size==0))
            return;

    line = (u8*)start;

    /*
    16 bytes per line
    */
    if (strHeader)
       printf ("%s", strHeader);

    column = size % 16;
    row = (size / 16) + 1;
    for (index = 0; index < row; index++, line += 16)
    {
        buf = (u8*)line;

        max = (index == row - 1) ? column : 16;
        if ( max==0 ) break; /* If we need not dump this line, break it. */

        printf ("\n[%08x] ", addr + index*16 - (aws_ota_imgaddr - SPI_FLASH_BASE));

        //Hex
        for (index2 = 0; index2 < max; index2++)
        {
            if (index2 == 8)
            printf ("  ");
            printf ("%02x ", (u8) buf[index2]);
        }

        if (max != 16)
        {
            if (max < 8)
                printf ("  ");
            for (index2 = 16 - max; index2 > 0; index2--)
                printf ("   ");
        }

    }

    printf ("\n");
    return;
}
#endif

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
static int prvGet_ota_tartget_header(u8* buf, u32 len, ota_hdr_manager_t * pOtaTgtHdr, u8 target_idx)
{
    ota_sub_hdr_t * ImgHdr;
    ota_hdr_t * FileHdr;
    u8 * pTempAddr;
    u32 i = 0, j = 0;
    int index = -1;

    /*check if buf and len is valid or not*/
    if((len < (sizeof(ota_sub_hdr_t) + 8)) || (!buf)) {
        goto error;
    }

    FileHdr = (ota_hdr_t *)buf;
    ImgHdr = (ota_sub_hdr_t *)(buf + 8);
    pTempAddr = buf + 8;

    if(len < (FileHdr->HdrNum * ImgHdr->ImgHdrLen + 8)) {
        goto error;
    }

    /*get the target OTA header from the new firmware file header*/
    for(i = 0; i < FileHdr->HdrNum; i++) {
        index = -1;
        pTempAddr = buf + 8 + ImgHdr->ImgHdrLen * i;

        if(strncmp("OTA", (const char *)pTempAddr, 3) == 0)
            index = 0;
        else
            goto error;

        if(index >= 0) {
            _memcpy((u8*)(&pOtaTgtHdr->FileImgHdr[j]), pTempAddr, sizeof(ota_sub_hdr_t));
            j++;
        }
    }

    pOtaTgtHdr->ValidImgCnt = j;

    if(j == 0) {
        printf("\n\r[%s] no valid image\n", __FUNCTION__);
        goto error;
    }

    return 1;
error:
    return 0;
}
#elif defined(CONFIG_AMEBAZ2)
static void clean_upgrade_region()
{
    uint32_t curr_fw_idx = 0;

    curr_fw_idx = sys_update_ota_get_curr_fw_idx();
    printf("Current firmware index is fw%d. \r\n",curr_fw_idx);
    if(curr_fw_idx==1)
    {
        printf("Clean invalid firmware index is fw2. \r\n");
        cmd_ota_image(1);  //active : fw1(0) ; invalid : fw2(1)
    }
    else
    {
        printf("Clean invalid firmware index is fw1. \r\n");
        cmd_ota_image(0);  //active : fw2(1) ; invalid : fw1(0)
    }
}
#endif

static void prvSysReset_ameba(u32 timeout_ms)
{
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    vTaskDelay(timeout_ms);
    sys_reset();
#elif defined(CONFIG_AMEBAD)
    WDG_InitTypeDef WDG_InitStruct;
    u32 CountProcess;
    u32 DivFacProcess;

#if defined(CONFIG_MBED_API_EN) && CONFIG_MBED_API_EN
    rtc_backup_timeinfo();
#endif

    WDG_Scalar(timeout_ms, &CountProcess, &DivFacProcess);
    WDG_InitStruct.CountProcess = CountProcess;
    WDG_InitStruct.DivFacProcess = DivFacProcess;
    WDG_Init(&WDG_InitStruct);

    WDG_Cmd(ENABLE);
#elif defined(CONFIG_AMEBAZ2)
    vTaskDelay(timeout_ms);
    ota_platform_reset();
#endif
}

OtaPalStatus_New_t prvPAL_Streams_Abort_ameba(AfrOtaJobDocumentFields_t *C)
{
    if (C != NULL && C->filepath != NULL) {
        LogInfo(("[%s] Abort OTA update", __FUNCTION__));
        C->filepath = NULL;
        ota_ctx.lFileHandle = 0x0;
    }
    return OtaPalSuccess_New;
}

bool prvPAL_Streams_CreateFileForRx_ameba(AfrOtaJobDocumentFields_t *C)
{
    OtaPalStatus_New_t mainErr = OtaPalSuccess_New;

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    int block_cnt = 0;
    int i=0;
    flash_t flash;

    uint32_t ImgId = OTA_IMGID_APP;

    /* determine the segment to store the OTA download in */
    if (ota_get_cur_index(ImgId) == OTA_INDEX_1)
    {
        ota_target_index = OTA_INDEX_2;
        flash_get_layout_info(IMG_APP_OTA2, &ota_ctx.lFileHandle, NULL);
    }
    else
    {
        ota_target_index = OTA_INDEX_1;
        flash_get_layout_info(IMG_APP_OTA1, &ota_ctx.lFileHandle, NULL);
    }
    ota_ctx.lFileHandle = ota_ctx.lFileHandle;

    //C->pFile = (uint8_t*)&ota_ctx;
    block_cnt = ((C->fileSize - 1) / (1024*64)) + 1;
    /* check the segment is valid and prepare the segment for write  */
    if ( ota_ctx.lFileHandle > SPI_FLASH_BASE )
    {
        OTA_PRINT("[OTA] valid ota addr (0x%x) \r\n", ota_ctx.lFileHandle);
        aws_ota_imgaddr = ota_ctx.lFileHandle;
        aws_ota_imgsz = 0;
        aws_ota_target_hdr_get = false;
        memset((void *)&aws_ota_target_hdr, 0, sizeof(ota_hdr_manager_t));
        memset((void *)&aws_manifest_new, 0, sizeof(ota_manifest_t));

        for( i = 0; i < block_cnt; i++)
        {
            OTA_PRINT("[OTA] Erase block @ 0x%x\n", ota_ctx.lFileHandle - SPI_FLASH_BASE + i * (64 * 1024));
            flash_erase_block(&flash, aws_ota_imgaddr - SPI_FLASH_BASE + i * (64 * 1024));
        }
    }
    else {
        OTA_PRINT("[OTA] invalid ota addr (%d) \r\n", ota_ctx.lFileHandle);
        ota_ctx.lFileHandle = (int32_t) NULL;      /* Nullify the file handle in all error cases. (fix: cast warning) */
    }
#elif defined(CONFIG_AMEBAD)
    int sector_cnt = 0;
    OTA_PRINT("\n\r[%s] OTA filesize: %d\n", __FUNCTION__, C->fileSize);

    /* determine the segment to store the OTA download in */
    if ( ota_get_cur_index() == OTA_INDEX_1 ) {
        ota_target_index = OTA_INDEX_2;
        ota_ctx.lFileHandle = OTA2_FLASH_START_ADDRESS;
        sector_cnt = ((C->fileSize - 1) / (1024 * 4)) + 1;
        OTA_PRINT("\n\r[%s] OTA2 address space will be upgraded\n", __FUNCTION__);
    } else {
        ota_target_index = OTA_INDEX_1;
        ota_ctx.lFileHandle = OTA1_FLASH_START_ADDRESS;
        sector_cnt = ((C->fileSize - 1) / (1024 * 4)) + 1;
        OTA_PRINT("\n\r[%s] OTA1 address space will be upgraded\n", __FUNCTION__);
    }
    /* check the segment is valid and prepare the segment for write  */
    if ( ota_ctx.lFileHandle > SPI_FLASH_BASE )
    {
        OTA_PRINT("[OTA] valid ota addr (0x%x) \r\n", ota_ctx.lFileHandle);
        aws_ota_imgaddr = ota_ctx.lFileHandle;
        aws_ota_imgsz = 0;
        aws_ota_target_hdr_get = false;
        memset((void *)&aws_ota_target_hdr, 0, sizeof(update_ota_target_hdr));
        memset((void *)aws_ota_signature, 0, sizeof(aws_ota_signature));

        for(int i = 0; i < sector_cnt; i++) {
            OTA_PRINT("[OTA] Erase sector_cnt @ 0x%x\n", ota_ctx.lFileHandle - SPI_FLASH_BASE + i * (1024*4));
            erase_ota_target_flash(aws_ota_imgaddr - SPI_FLASH_BASE + i * (1024*4), (1024*4));
        }
    }
    else {
        OTA_PRINT("[OTA] invalid ota addr (%d) \r\n", ota_ctx.lFileHandle);
        ota_ctx.lFileHandle = (int32_t) NULL;      /* Nullify the file handle in all error cases. (fix: cast warning) */
    }
#elif defined(CONFIG_AMEBAZ2)
    // OtaPalStatus_New_t mainErr = OtaPalSuccess_New;

    uint32_t curr_fw_idx = sys_update_ota_get_curr_fw_idx();

    LogInfo( ( "Current firmware index is %d", curr_fw_idx ) );

    C->filepath = (uint8_t*)&ota_ctx;

    ota_ctx.lFileHandle = sys_update_ota_prepare_addr() + SPI_FLASH_BASE;
    if(curr_fw_idx==1)
        LogInfo( ( "fw2 address 0x%x will be upgraded", ota_ctx.lFileHandle ) );
    else
        LogInfo( ( "fw1 address 0x%x will be upgraded", ota_ctx.lFileHandle ) );
#endif

    if( ota_ctx.lFileHandle <= SPI_FLASH_BASE )
    {
        mainErr = OtaPalRxFileCreateFailed_New;
    }

    return mainErr;
}

#if defined(CONFIG_AMEBAD)
/**
  * @brief  save default mmu config
  * @retval none  
  */
static inline void mmu_save(u32 MMUIdx, u32 *vAddrSt, u32 *vAddrEnd, u32 *ctrl, u32 *offset)
{
    RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

    /* save 4 registers */
    *vAddrSt = RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_STRADDR;
    *vAddrEnd = RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_ENDADDR;
    *offset = RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_OFFSET;
    *ctrl =  RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_CTRL;
}

/**
  * @brief  restore default mmu config
  * @retval none  
  */
static inline void mmu_restore(u32 MMUIdx, u32 *vAddrSt, u32 *vAddrEnd, u32 *ctrl, u32 *offset)
{
    RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

    /* save 4 registers */
    RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_STRADDR = *vAddrSt;
    RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_ENDADDR = *vAddrEnd;
    RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_OFFSET = *offset;
    RSIP->FLASH_MMU[MMUIdx].MMU_ENTRYx_CTRL = *ctrl;
}

static bool rtl8721d_check_flash_ota_header_streams(void) {
    u32 AddrStart, Offset, IsMinus, PhyAddr;
    u32 CurrentImagePhysAddr, OtherOTAImagePhysAddr;
    u32 ota_buffer[16];                        /* contains the portion of the header. demonstrate reading the top 16 words */
    u32 mmuRecord[2][4] = {{0, }, };        /* to hold a temporary record of the MMU config to restore */
    u8 currentlyOnOTA2 = false;

    /* is OTF enabled? */
    u32 OTF_Enable = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_ENCRYPT_EN;

    /* check which OTA we are on and which we need to check */
    RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
    u32 CtrlTemp = RSIP->FLASH_MMU[0].MMU_ENTRYx_CTRL;

    /* read the MMU to obtain the image addresses */
    if (CtrlTemp & MMU_BIT_ENTRY_VALID) {
        AddrStart = RSIP->FLASH_MMU[0].MMU_ENTRYx_STRADDR;
        Offset = RSIP->FLASH_MMU[0].MMU_ENTRYx_OFFSET;
        IsMinus = CtrlTemp & MMU_BIT_ENTRY_OFFSET_MINUS;

        if(IsMinus)
            PhyAddr = AddrStart - Offset;
        else
            PhyAddr = AddrStart + Offset;

        if(PhyAddr == LS_IMG2_OTA1_ADDR){
            CurrentImagePhysAddr = LS_IMG2_OTA1_ADDR;
            OtherOTAImagePhysAddr = LS_IMG2_OTA2_ADDR;
            currentlyOnOTA2 = false;
        }else{
            CurrentImagePhysAddr = LS_IMG2_OTA2_ADDR;
            OtherOTAImagePhysAddr = LS_IMG2_OTA1_ADDR;
            currentlyOnOTA2 = true;
        }
    }

    printf("[OTA] RSIP Enabled: %s\n", OTF_Enable ? "true" : "false");
    printf("[OTA] Current on: %s, OTA on: %s\n", currentlyOnOTA2 ? "OTA2" : "OTA1", currentlyOnOTA2 ? "OTA1" : "OTA2");

    /* map the physical address to virtual memory so that the CPU is able to read it */
    mmu_save(0, &mmuRecord[0][0], &mmuRecord[0][1], &mmuRecord[0][2], &mmuRecord[0][3]);
    FLASH_Write_Lock();
    RSIP_MMU_Config(
        0,                                                      /* use MMU entry #0 */
        0x0C000000,                                             /* map this address as virtual memory */
        0x0C000000 + 4096 - 1,                                  /* map exactly 0x1000 (1 page=4KB)*/
        1,                                                      /* is negative offset (below) */
        0x0C000000 - (OtherOTAImagePhysAddr - SPI_FLASH_BASE)   /* the target physical address offset to map from */
    );
    memcpy(ota_buffer, (void *)0x0C000000, sizeof(ota_buffer)); /* regular memcpy can be used after mapping*/
    /* restore the MMU record after finish reading */
    mmu_restore(0, &mmuRecord[0][0], &mmuRecord[0][1], &mmuRecord[0][2], &mmuRecord[0][3]);
    FLASH_Write_Unlock();

    /* check the header */
    //vMemDump(OtherOTAImagePhysAddr, ota_buffer, sizeof(ota_buffer), "HEADER");

    return true;
}
#elif defined(CONFIG_AMEBAZ2)
#endif

/* Read the specified signer certificate from the filesystem into a local buffer. The
 * allocated memory becomes the property of the caller who is responsible for freeing it.
 */
uint8_t * prvPAL_Streams_ReadAndAssumeCertificate_ameba(const uint8_t * const pucCertName, int32_t * const lSignerCertSize)
{
    uint8_t*    pucCertData;
    uint32_t    ulCertSize;
    uint8_t     *pucSignerCert = NULL;

    extern BaseType_t PKCS11_PAL_GetObjectValue( const char * pcFileName,
                               uint8_t ** ppucData,
                               uint32_t * pulDataSize );

    if ( PKCS11_PAL_GetObjectValue( (const char *) pucCertName, &pucCertData, &ulCertSize ) != pdTRUE )
    {   /* Use the back up "codesign_keys.h" file if the signing credentials haven't been saved in the device. */
        pucCertData = (uint8_t*) otapalconfigCODE_SIGNING_CERTIFICATE;
        ulCertSize = sizeof( otapalconfigCODE_SIGNING_CERTIFICATE );
        LogInfo( ( "Assume Cert - No such file: %s. Using header file", (const char*)pucCertName ) );
    }
    else
    {
        LogInfo( ( "Assume Cert - file: %s OK", (const char*)pucCertName ) );
    }

    /* Allocate memory for the signer certificate plus a terminating zero so we can load it and return to the caller. */
    pucSignerCert = pvPortMalloc( ulCertSize +  1);
    if ( pucSignerCert != NULL )
    {
        memcpy( pucSignerCert, pucCertData, ulCertSize );
        /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
        pucSignerCert[ ulCertSize ] = '\0';
        *lSignerCertSize = ulCertSize + 1;
    }
    return pucSignerCert;
}

static OtaPalStatus_New_t prvPAL_Streams_SignatureVerificationUpdate_ameba(AfrOtaJobDocumentFields_t *C, void * pvContext)
{
    (void) C; // unused

    OtaPalStatus_New_t mainErr = OtaPalSuccess_New;

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    u32 i;
    flash_t flash;
    u8 * pTempbuf;
    int rlen;
    u32 len = aws_ota_imgsz;
    u32 addr = ota_ctx.lFileHandle;//aws_ota_target_hdr.FileImgHdr[HdrIdx].FlashAddr;

    if(len <= 0) {
      return OtaPalSignatureCheckFailed_New;
    }

    pTempbuf = malloc(BUF_SIZE);
    if(!pTempbuf){
        mainErr = OtaPalSignatureCheckFailed_New;
        goto error;
    }

    /*handle manifest */
    memcpy(&aws_ota_target_hdr.Manifest[HdrIdx], &aws_manifest_new, sizeof(ota_manifest_t));
    CRYPTO_SignatureVerificationUpdate(pvContext, &aws_ota_target_hdr.Manifest[HdrIdx], sizeof(ota_manifest_t));

    printf("[%d]manifest\n",HdrIdx);
    for (int i = 0; i < sizeof(ota_manifest_t); i++) {
        printf("0x%x ",*((u8 *)&aws_ota_target_hdr.Manifest[HdrIdx] + i));
    }
    printf("\n");

    len = len - sizeof(ota_manifest_t);
    /* read flash data back to check signature of the image */
    for (i = 0; i < len; i += BUF_SIZE) {
        rlen = (len - i) > BUF_SIZE ? BUF_SIZE : (len - i);
        flash_stream_read(&flash, addr - SPI_FLASH_BASE + i + sizeof(ota_manifest_t), rlen, pTempbuf);
    #if OTA_MEMDUMP
        vMemDump(addr - SPI_FLASH_BASE + i + sizeof(ota_manifest_t), pTempbuf, rlen, "PAYLOAD1");
    #endif
        CRYPTO_SignatureVerificationUpdate(pvContext, pTempbuf, rlen);
    }

error:
    if(pTempbuf)
        free(pTempbuf);
#elif defined(CONFIG_AMEBAD)
    u32 i;
    flash_t flash;
    u8 * pTempbuf;
    int rlen;
    u32 len = aws_ota_imgsz;
    u32 addr = aws_ota_target_hdr.FileImgHdr[HdrIdx].FlashAddr;

    if( len <= 0 ) {
        mainErr = OtaPalSignatureCheckFailed_New;
        return mainErr;
    }

    pTempbuf = ota_update_malloc(BUF_SIZE);
    if( pTempbuf == NULL ) {
        mainErr = OtaPalSignatureCheckFailed_New;
        goto error;
    }

    /*add image signature(81958711)*/
    CRYPTO_SignatureVerificationUpdate(pvContext, aws_ota_signature, AWS_OTA_IMAGE_SIGNATURE_LEN);

    len = len-8;
    /* read flash data back to check signature of the image */
    for( i = 0; i < len; i += BUF_SIZE ) {
        rlen = (len - i) > BUF_SIZE ? BUF_SIZE : (len - i);
        //flash_stream_read(&flash, addr - SPI_FLASH_BASE + i + AWS_OTA_IMAGE_SIGNATURE_LEN, rlen, pTempbuf);
        ota_readstream_user(addr - SPI_FLASH_BASE + i + AWS_OTA_IMAGE_SIGNATURE_LEN, rlen, pTempbuf); // use USER mode to read back raw from Flash, skip SPIC
        Cache_Flush();
        CRYPTO_SignatureVerificationUpdate(pvContext, pTempbuf, rlen);
    }

error:
    if( pTempbuf != NULL ) {
        ota_update_free(pTempbuf);
    }
#elif defined(CONFIG_AMEBAZ2)
    int firmware_len = (C->fileSize % OTA_FILE_BLOCK_SIZE) == 4 ? (C->fileSize - 4) :
                        C->fileSize;  // if fw length % 4096 = 4, it may include 4 bytes checksum at the end of fw
    int chklen = firmware_len; // skip 4byte ota length

    uint8_t *pTempbuf = pvPortMalloc(OTA_FILE_BLOCK_SIZE);

    uint32_t addr = ota_ctx.lFileHandle - SPI_FLASH_BASE;

    uint32_t cur_block = 0;
    while (chklen > 0) {
        int rdlen = chklen > OTA_FILE_BLOCK_SIZE ? OTA_FILE_BLOCK_SIZE : chklen;
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_stream_read(&flash_ota, addr + cur_block * OTA_FILE_BLOCK_SIZE, rdlen, pTempbuf);
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
        if (chklen == firmware_len) {   // for first block
            /* update sigature from backup buffer */
            CRYPTO_SignatureVerificationUpdate(pvContext, (uint8_t *)sig_backup, AWS_OTA_IMAGE_SIGNATURE_LEN);
            /* update content */
            CRYPTO_SignatureVerificationUpdate(pvContext, (uint8_t *)pTempbuf + AWS_OTA_IMAGE_SIGNATURE_LEN, rdlen - AWS_OTA_IMAGE_SIGNATURE_LEN);
        } else {
            /* update content */
            CRYPTO_SignatureVerificationUpdate(pvContext, (uint8_t *)pTempbuf, rdlen);
        }
        chklen -= rdlen;
        cur_block++;
    }

exit:
    if(pTempbuf)
        update_free(pTempbuf);
#endif

    return mainErr;
}

OtaPalStatus_New_t prvPAL_Streams_SetPlatformImageState_ameba (OtaImageState_New_t eState);
OtaPalStatus_New_t prvPAL_Streams_CheckFileSignature_ameba(AfrOtaJobDocumentFields_t * const C)
{
    OtaPalStatus_New_t mainErr = OtaPalSuccess_New;

    int32_t lSignerCertSize;
    void *pvSigVerifyContext;
    uint8_t *pucSignerCert = NULL;

#if (defined(CONFIG_AMEBAZ2))
    extern void *calloc_freertos(size_t nelements, size_t elementSize);
    mbedtls_platform_set_calloc_free(calloc_freertos, vPortFree);
#endif

    /* Verify an ECDSA-SHA256 signature. */
    if (CRYPTO_SignatureVerificationStart(&pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256) == pdFALSE) {
        mainErr = OtaPalSignatureCheckFailed_New;
        goto exit;
    }

    LogInfo(("[%s] Started %s signature verification, file: %s", __FUNCTION__, OTA_JsonFileSignatureKey, (const char *)C->certfile));
    if ((pucSignerCert = prvPAL_Streams_ReadAndAssumeCertificate_ameba((const uint8_t *const)C->certfile, &lSignerCertSize)) == NULL) {
        mainErr = OtaPalBadSignerCert_New;
        goto exit;
    }


    if (prvPAL_Streams_SignatureVerificationUpdate_ameba(C, pvSigVerifyContext) != OtaPalSuccess_New) {
        mainErr = OtaPalSignatureCheckFailed_New;
        goto exit;
    }

    if (CRYPTO_SignatureVerificationFinal(pvSigVerifyContext, (char *)pucSignerCert, lSignerCertSize, C->signature, C->signatureLen) == pdFALSE) {
        mainErr = OtaPalSignatureCheckFailed_New;
        prvPAL_Streams_SetPlatformImageState_ameba(OtaImageStateRejected_New);
        goto exit;
    }

#if defined(CONFIG_AMEBAD)
    rtl8721d_check_flash_ota_header_streams();
#endif
exit:
    /* Free the signer certificate that we now own after prvPAL_ReadAndAssumeCertificate(). */
    if (pucSignerCert != NULL) {
        vPortFree(pucSignerCert);
    }
    return mainErr;
}

/* Close the specified file. This will also authenticate the file if it is marked as secure. */
OtaPalStatus_New_t prvPAL_Streams_CloseFile_ameba(AfrOtaJobDocumentFields_t *C)
{
    OtaPalStatus_New_t mainErr = OtaPalSuccess_New;

    LogInfo(("[OTA] Authenticating and closing file.\r\n"));

    if (C == NULL) {
        mainErr = OtaPalNullFileContext_New;
        goto exit;
    }

    if (C->signature != NULL) {
#if OTA_MEMDUMP
        vMemDump(C->signature, C->signatureLen, "Signature");
#endif
        /* TODO: Verify the file signature, close the file and return the signature verification result. */
        mainErr = prvPAL_Streams_CheckFileSignature_ameba(C);

    } else {
        mainErr = OtaPalSignatureCheckFailed_New;
    }

    if (mainErr == OtaPalSuccess_New) {
        LogInfo(("[%s] %s signature verification passed.", __FUNCTION__, OTA_SIG_KEY_STR));
    } else {
        LogError(("[%s] Failed to pass %s signature verification: %d.", __FUNCTION__, OTA_SIG_KEY_STR, mainErr));

        /* If we fail to verify the file signature that means the image is not valid. We need to set the image state to aborted. */
        prvPAL_Streams_SetPlatformImageState_ameba(OtaImageStateAborted_New);
    }

exit:
    return mainErr;
}

void hexdump(unsigned char *a, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++) {        
        if(i % 16 == 0 && i > 0) OTA_PRINT("\n");
        OTA_PRINT("%02X ", a[i]);
    }
    OTA_PRINT("\n");
}

int32_t prvPAL_Streams_WriteBlock_ameba(AfrOtaJobDocumentFields_t *C, uint32_t ulOffset, uint8_t* pData, uint32_t ulBlockSize)
{
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    (void) C; // unused

    uint32_t address = ota_ctx.lFileHandle - SPI_FLASH_BASE;
    uint32_t WriteLen, offset;
    uint32_t version=0, major=0, minor=0, build=0;
    flash_t flash;
    static uint32_t img_sign = 0;

    if (aws_ota_target_hdr_get != true)
    {
        u32 RevHdrLen;

        /* first block is downloaded, check if this is a valid OTA image by reading the header */
        if(ulOffset == 0)
        {
            img_sign = 0;
            memset((void *)&aws_ota_target_hdr, 0, sizeof(ota_hdr_manager_t));
            memcpy((u8*)(&aws_ota_target_hdr.FileHdr), pData, sizeof(aws_ota_target_hdr.FileHdr));
            if(aws_ota_target_hdr.FileHdr.HdrNum > 2 || aws_ota_target_hdr.FileHdr.HdrNum <= 0)
            {
                OTA_PRINT("INVALID IMAGE BLOCK 0\r\n");
                return -1;
            }

            memcpy((u8*)(&aws_ota_target_hdr.FileImgHdr[HdrIdx]), pData+sizeof(aws_ota_target_hdr.FileHdr), 8);
            RevHdrLen = (aws_ota_target_hdr.FileHdr.HdrNum * aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgHdrLen) + sizeof(aws_ota_target_hdr.FileHdr);
            if (!prvGet_ota_tartget_header(pData, RevHdrLen, &aws_ota_target_hdr, ota_target_index))
            {
                OTA_PRINT("Get OTA header failed\n");
                return -1;
            }

            hexdump(&aws_ota_target_hdr, sizeof(ota_hdr_manager_t));

            // check version from header
            version = aws_ota_target_hdr.FileHdr.FwVer;
            major = version / 1000000;
            minor = (version - (major*1000000)) / 1000;
            build = (version - (major*1000000) - (minor * 1000))/1;
            if( aws_ota_target_hdr.FileHdr.FwVer <= (APP_VERSION_MAJOR*1000000 + APP_VERSION_MINOR * 1000 + APP_VERSION_BUILD)) {
                OTA_PRINT("\nOTA failed!!!\n");
                OTA_PRINT("New Firmware version(%d,%d,%d) must greater than current firmware version(%d,%d,%d)\n\n",major,minor,build,APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_BUILD);
                return -1;
            } else {
                OTA_PRINT("New Firmware version (%d,%d,%d), current firmware version(%d,%d,%d)\n",major,minor,build,APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_BUILD);
            }

            aws_ota_target_hdr_get = true;
        }
        else
        {
            aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen = ota_ctx.lFileHandle;
            aws_ota_target_hdr.FileHdr.HdrNum = 0x1;
            aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset = 0x20;
        }
    }

    LogInfo(("[%s] C->fileSize %d, iOffset: 0x%x: iBlockSize: 0x%x", __FUNCTION__, C->fileSize, ulOffset, ulBlockSize));

    /* check if already downloaded beyond the size, drop additional downloaded data */
    if(aws_ota_imgsz >= aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen){
        OTA_PRINT("[OTA] image download is already done, dropped, aws_ota_imgsz=0x%X, ImgLen=0x%X\n",aws_ota_imgsz,aws_ota_target_hdr.FileImgHdr[aws_ota_target_hdr.FileHdr.HdrNum].ImgLen);
        return ulBlockSize;
    }

    // handle first block, do not write ota header to flash
    if(ulOffset <= aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset) {
        uint32_t byte_to_write = (ulOffset + ulBlockSize) - aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset;

        pData += (ulBlockSize - byte_to_write);

        if(OTA_FILE_BLOCK_SIZE >= 0x1000 && ulOffset == 0)
        {
             OTA_PRINT("[OTA] manifest data arrived \n");
             //Save manifest
             memcpy(&aws_manifest_new, pData, sizeof(ota_manifest_t));
             //Erase manifest for protect shutdown while ota downloading
             memset(pData, 0xff, sizeof(ota_manifest_t));

             printf("[%d]manifest\n",HdrIdx);
             for (int i = 0; i < sizeof(ota_manifest_t); i++) {
                 printf("0x%x ",*((u8 *)&aws_manifest_new + i));
             }
             printf("\n");
        }

        OTA_PRINT("[OTA] FIRST Write %d bytes @ 0x%x\n", byte_to_write, address);
        if(flash_stream_write(&flash, address, byte_to_write, pData) < 0){
            OTA_PRINT("[%s] Write sector failed\n", __FUNCTION__);
            return -1;
        }
#if OTA_MEMDUMP
        vMemDump(address, pData, byte_to_write, "PAYLOAD1");
#endif
        aws_ota_imgsz += byte_to_write;
        return ulBlockSize;
    }

    WriteLen = ulBlockSize;
    offset = ulOffset - aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset;

    /* bounds check for last block */
    if ((offset + ulBlockSize) >= aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen) {

        if(offset > aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen)
            return ulBlockSize;
        WriteLen -= (offset + ulBlockSize - aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen);
        OTA_PRINT("[OTA] LAST image data arrived %d\n", WriteLen);
    }

    LogInfo( ("[OTA] Write %d bytes @ 0x%x \n", WriteLen, address + offset) );
    if(flash_stream_write(&flash, address + offset, WriteLen, pData) < 0){
        LogInfo( ("[%s] Write sector failed\n", __FUNCTION__) );
        return -1;
    }
#if OTA_MEMDUMP
    vMemDump(address+offset, pData, ulBlockSize, "PAYLOAD2");
#endif
    aws_ota_imgsz += WriteLen;
#elif defined(CONFIG_AMEBAD)
    (void) C; // unused

    uint32_t address = ota_ctx.lFileHandle - SPI_FLASH_BASE;
    uint32_t WriteLen, offset;
    uint32_t version=0, major=0, minor=0, build=0;

    if (aws_ota_target_hdr_get != true)
    {
        u32 RevHdrLen;

        /* first block is downloaded, check if this is a valid OTA image by reading the header */
        if(ulOffset == 0)
        {
            memset((void *)&aws_ota_target_hdr, 0, sizeof(update_ota_target_hdr));
            memset((void *)aws_ota_signature, 0, sizeof(aws_ota_signature));
            memcpy((u8*)(&aws_ota_target_hdr.FileHdr), pData, sizeof(aws_ota_target_hdr.FileHdr));
            if(aws_ota_target_hdr.FileHdr.HdrNum > 2 || aws_ota_target_hdr.FileHdr.HdrNum <= 0)
            {
                OTA_PRINT("INVALID IMAGE BLOCK 0\r\n");
                return -1;
            }

            memcpy((u8*)(&aws_ota_target_hdr.FileImgHdr[HdrIdx]), pData+sizeof(aws_ota_target_hdr.FileHdr), AWS_OTA_IMAGE_SIGNATURE_LEN);
            RevHdrLen = (aws_ota_target_hdr.FileHdr.HdrNum * aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgHdrLen) + sizeof(aws_ota_target_hdr.FileHdr);
            if ( !get_ota_tartget_header(pData, RevHdrLen, &aws_ota_target_hdr, ota_target_index) )
            {
                OTA_PRINT("Get OTA header failed\n");
                return -1;
            }

            hexdump(&aws_ota_target_hdr, sizeof(update_ota_target_hdr));

            // check version from header
            version = aws_ota_target_hdr.FileHdr.FwVer;
            major = version / 1000000;
            minor = (version - (major*1000000)) / 1000;
            build = (version - (major*1000000) - (minor * 1000))/1;
            if( aws_ota_target_hdr.FileHdr.FwVer <= (APP_VERSION_MAJOR*1000000 + APP_VERSION_MINOR * 1000 + APP_VERSION_BUILD)) {
                OTA_PRINT("\nOTA failed!!!\n");
                OTA_PRINT("New Firmware version(%d,%d,%d) must greater than current firmware version(%d,%d,%d)\n\n",major,minor,build,APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_BUILD);
                return -1;
            } else {
                OTA_PRINT("New Firmware version (%d,%d,%d), current firmware version(%d,%d,%d)\n",major,minor,build,APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_BUILD);
            }

            aws_ota_target_hdr_get = true;
        }
        else
        {
            aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen = ota_ctx.lFileHandle;
            aws_ota_target_hdr.FileHdr.HdrNum = 0x1;
            aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset = 0x20;
        }
    }

    LogInfo(("[%s] C->fileSize %d, iOffset: 0x%x: iBlockSize: 0x%x", __FUNCTION__, C->fileSize, ulOffset, ulBlockSize));

    /* check if already downloaded beyond the size, drop additional downloaded data */
    if(aws_ota_imgsz >= aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen){
        OTA_PRINT("[OTA] image download is already done, dropped, aws_ota_imgsz=0x%X, ImgLen=0x%X\n",aws_ota_imgsz,aws_ota_target_hdr.FileImgHdr[aws_ota_target_hdr.FileHdr.HdrNum].ImgLen);
        return ulBlockSize;
    }

    // handle first block, do not write ota header to flash
    if(ulOffset <= aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset) {
        uint32_t byte_to_write = (ulOffset + ulBlockSize) - aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset;

        pData += (ulBlockSize - byte_to_write);

        if( ulOffset == 0 ) {
            memcpy(aws_ota_target_hdr.Sign[HdrIdx],pData,sizeof(aws_ota_signature));
            memcpy(aws_ota_signature, pData, sizeof(aws_ota_signature));
            memset(pData, 0xff, sizeof(aws_ota_signature)); 
        }

        OTA_PRINT("[OTA] FIRST Write %d bytes @ 0x%x\n", byte_to_write, address);
        if( ota_writestream_user(address, byte_to_write, pData) < 0 ) {
            OTA_PRINT("[%s] Write sector failed\n", __FUNCTION__);
            return -1;
        }
#if OTA_MEMDUMP
        vMemDump(address, pData, byte_to_write, "PAYLOAD1");
#endif
        aws_ota_imgsz += byte_to_write;
        return ulBlockSize;
    }

    WriteLen = ulBlockSize;
    offset = ulOffset - aws_ota_target_hdr.FileImgHdr[HdrIdx].Offset;

    /* bounds check for last block */
    if ((offset + ulBlockSize) >= aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen) {

        if(offset > aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen)
            return ulBlockSize;
        WriteLen -= (offset + ulBlockSize - aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen);
        OTA_PRINT("[OTA] LAST image data arrived %d\n", WriteLen);
    }

    LogInfo( ("[OTA] Write %d bytes @ 0x%x \n", WriteLen, address + offset) );
    if( ota_writestream_user(address + offset, WriteLen, pData) < 0 ) {
        LogInfo( ("[%s] Write sector failed\n", __FUNCTION__) );
        return -1;
    }
#if OTA_MEMDUMP
    vMemDump(address+offset, pData, ulBlockSize, "PAYLOAD2");
#endif
    aws_ota_imgsz += WriteLen;
#elif defined(CONFIG_AMEBAZ2)
    static uint32_t buf_size = OTA_FILE_BLOCK_SIZE;
    static uint32_t ota_len = 0;
    static uint32_t total_blocks = 0;
    static uint32_t cur_block = 0;
    static uint32_t idx = 0;
    static uint32_t read_bytes = 0;
    static uint8_t *buf = NULL;
    static uint32_t ota_address = 0;
    static uint8_t first_time = 0;

    if (C == NULL) {
        return -1;
    }

    LogInfo(("[%s] C->fileSize %d, iOffset: 0x%x: iBlockSize: 0x%x", __FUNCTION__, C->fileSize, ulOffset, ulBlockSize));

    ota_address = ota_ctx.lFileHandle - SPI_FLASH_BASE;
    buf_size = OTA_FILE_BLOCK_SIZE;
    ota_len = C->fileSize;
    total_blocks = (ota_len + buf_size - 1) / buf_size;
    idx = ulOffset;
    read_bytes = ulBlockSize;
    cur_block = idx / buf_size;
    buf = pData;

    LogInfo(("[%s] ota_len:%d, cur_block:%d", __FUNCTION__, ota_len, cur_block));
#if OTA_MEMDUMP
    vMemDump(pData, ulBlockSize, "PAYLOAD0");
#endif

    if(first_time==0) {
        LogInfo(("[%s] update_ota_erase_upg_region", __FUNCTION__, idx, ota_len));
        update_ota_erase_upg_region(C->fileSize, 0, sys_update_ota_prepare_addr());
        first_time = 1;
    }

    if (idx >= ota_len) {
        LogInfo(("[%s] image download is already done, dropped, idx=0x%X, ota_len=0x%X", __FUNCTION__, idx, ota_len));
        goto exit;
    }

    // for first block
    if (0 == cur_block) {
        LogInfo(("[%s] FIRST image data arrived %d, back up the first 32-bytes fw signature", __FUNCTION__, read_bytes));

        memcpy(sig_backup, pData, AWS_OTA_IMAGE_SIGNATURE_LEN);
        memset(buf, 0xFF, AWS_OTA_IMAGE_SIGNATURE_LEN); // not flash write 8-bytes fw label
        LogInfo(("[%s] sig_backup get", __FUNCTION__));
    }

    // check final block
    if (cur_block == (total_blocks - 1)) {
        LogInfo(("[%s] LAST image data arrived", __FUNCTION__));
    }

    /* write block by flash api */
    device_mutex_lock(RT_DEV_LOCK_FLASH);
    LogInfo(("[OTA] Write %d bytes @ 0x%x (0x%x)\n", read_bytes, ota_address + ulOffset, (ota_address + ulOffset)));
    if(flash_stream_write(&flash_ota, ota_address + ulOffset, read_bytes, buf) < 0){
        LogInfo(("[%s] Write sector failed\n", __FUNCTION__));
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
        return -1;
    }
    device_mutex_unlock(RT_DEV_LOCK_FLASH);

exit:

    LogInfo(("[%s] Write bytes: read_bytes %d, ulBlockSize %d", __FUNCTION__, read_bytes, ulBlockSize));
#endif

    return ulBlockSize;
}

OtaPalStatus_New_t prvPAL_Streams_ActivateNewImage_ameba(void)
{
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    flash_t flash;
    OTA_PRINT("[OTA] [%s] Download new firmware %d bytes completed @ 0x%x\n", __FUNCTION__, aws_ota_imgsz, aws_ota_imgaddr);
    OTA_PRINT("[OTA] FirmwareSize = %d, OtaTargetHdr.FileImgHdr.ImgLen = %d\n", aws_ota_imgsz, aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen);

    /*------------- verify checksum and update signature-----------------*/
    if(ota_storage_verify_checksum(&aws_ota_target_hdr, ota_target_index, 0/*header index*/) == OTA_OK ){
        if(ota_storage_update_manifest(&aws_ota_target_hdr, ota_target_index, 0/*header index*/) != OTA_OK ) {
            OTA_PRINT("[OTA] [%s], change signature failed\r\n", __FUNCTION__);
            return OtaPalActivateFailed_New;
        } else {
            flash_erase_sector(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET);
            flash_write_word(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, AWS_OTA_IMAGE_STATE_FLAG_PENDING_COMMIT);
            OTA_PRINT("[OTA] [%s] Update OTA success!\r\n", __FUNCTION__);
        }
    }else{
        /*if checksum error, clear the signature zone which has been written in flash in case of boot from the wrong firmware*/
        flash_erase_sector(&flash, aws_ota_imgaddr - SPI_FLASH_BASE);
        OTA_PRINT("[OTA] [%s] The checksum is wrong!\n\r", __FUNCTION__);
        return OtaPalActivateFailed_New;
    }
#elif defined(CONFIG_AMEBAD)
    flash_t flash;
    OTA_PRINT("[OTA] [%s] Download new firmware %d bytes completed @ 0x%x\n", __FUNCTION__, aws_ota_imgsz, aws_ota_imgaddr);
    OTA_PRINT("[OTA] FirmwareSize = %d, OtaTargetHdr.FileImgHdr.ImgLen = %d\n", aws_ota_imgsz, aws_ota_target_hdr.FileImgHdr[HdrIdx].ImgLen);

    /*------------- verify checksum and update signature-----------------*/
    if( verify_ota_checksum(&aws_ota_target_hdr) ) {
        if( change_ota_signature(&aws_ota_target_hdr, ota_target_index) != 1 ) {
            OTA_PRINT("[OTA] [%s], change signature failed\r\n", __FUNCTION__);
            return OtaPalActivateFailed_New;
        } else {
            device_mutex_lock(RT_DEV_LOCK_FLASH);
            flash_erase_sector(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET);
            flash_write_word(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, AWS_OTA_IMAGE_STATE_FLAG_PENDING_COMMIT);
            device_mutex_unlock(RT_DEV_LOCK_FLASH);
            OTA_PRINT("[OTA] [%s] Update OTA success!\r\n", __FUNCTION__);
        }
    }else{
        /*if checksum error, clear the signature zone which has been written in flash in case of boot from the wrong firmware*/
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_erase_sector(&flash, aws_ota_imgaddr - SPI_FLASH_BASE);
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
        OTA_PRINT("[OTA] [%s] The checksum is wrong!\n\r", __FUNCTION__);
        return OtaPalActivateFailed_New;
    }
#elif defined(CONFIG_AMEBAZ2)
    int ret = -1;
    uint32_t NewFWAddr = 0;

    NewFWAddr = sys_update_ota_prepare_addr();
    if(NewFWAddr == -1){
        return OtaPalActivateFailed_New;
    }

    ret = update_ota_signature(sig_backup, NewFWAddr);
    if(ret == -1){
        LogInfo(("[%s] Update signature fail\r\n", __FUNCTION__));
        return OtaPalActivateFailed_New;
    }
    else{
        prvPAL_Streams_SetPlatformImageState_ameba(OtaImageStateTesting_New);
        LogInfo(("[OTA] [%s] Update OTA success!\r\n", __FUNCTION__));
    }
#endif
    return OtaPalSuccess_New;
}

OtaPalStatus_New_t prvPAL_Streams_ResetDevice_ameba ( void )
{
    prvSysReset_ameba(10);
    return OtaPalSuccess_New;
}

OtaPalStatus_New_t prvPAL_Streams_SetPlatformImageState_ameba (OtaImageState_New_t eState)
{
    OtaPalStatus_New_t mainErr = OtaPalSuccess_New;
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    flash_t flash;

    if ((eState != OtaImageStateUnknown_New) && (eState <= OtaLastImageState_New)) {
        /* write state to file */
        flash_erase_sector(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET);
        flash_write_word(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, eState);
#elif defined(CONFIG_AMEBAD)
    flash_t flash;

    if ((eState != OtaImageStateUnknown_New) && (eState <= OtaLastImageState_New)) {
        /* write state to file */
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_erase_sector(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET);
        flash_write_word(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, eState);
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
#elif defined(CONFIG_AMEBAZ2)
    if ((eState != OtaImageStateUnknown_New) && (eState <= OtaLastImageState_New)) {
        /* write state to file */
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_erase_sector(&flash_ota, AWS_OTA_IMAGE_STATE_FLASH_OFFSET);
        flash_write_word(&flash_ota, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, eState);
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
#endif
    } else { /* Image state invalid. */
        LogError(("[%s] Invalid image state provided.", __FUNCTION__));
        mainErr = OtaPalBadImageState_New;
    }

    return mainErr;
}

OtaPalImageState_New_t prvPAL_Streams_GetPlatformImageState_ameba( void )
{
    OtaPalImageState_New_t eImageState = OtaPalImageStateUnknown_New;
    uint32_t eSavedAgentState  =  OtaImageStateUnknown_New;
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBASMART)
    flash_t flash;

    flash_read_word(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, &eSavedAgentState );
#elif defined(CONFIG_AMEBAD)
    flash_t flash;

    device_mutex_lock(RT_DEV_LOCK_FLASH);
    flash_read_word(&flash, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, &eSavedAgentState );
    device_mutex_unlock(RT_DEV_LOCK_FLASH);
#elif defined(CONFIG_AMEBAZ2)
    device_mutex_lock(RT_DEV_LOCK_FLASH);
    flash_read_word(&flash_ota, AWS_OTA_IMAGE_STATE_FLASH_OFFSET, &eSavedAgentState );
    device_mutex_unlock(RT_DEV_LOCK_FLASH);
#endif

    switch ( eSavedAgentState  )
    {
        case OtaImageStateTesting_New:
            /* Pending Commit means we're in the Self Test phase. */
            eImageState = OtaPalImageStatePendingCommit_New;
            break;
        case OtaImageStateAccepted_New:
            eImageState = OtaPalImageStateValid_New;
            break;
        case OtaImageStateRejected_New:
        case OtaImageStateAborted_New:
        default:
            eImageState = OtaPalImageStateInvalid_New;
            break;
    }
    LogInfo( ( "Image current state (0x%02x).", eImageState ) );

    return eImageState;
}
