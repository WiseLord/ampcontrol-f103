#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_amp.h"

#include "hwlibs.h"

static PCD_HandleTypeDef hpcd_USB_FS;

void HAL_PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state);

void HAL_PCD_MspInit(PCD_HandleTypeDef *pcdHandle)
{
    if (pcdHandle->Instance == USB) {
        // Peripheral clock enable
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USB);

#ifdef STM32F3
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

        GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_14;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

        // Peripheral interrupt init
#ifdef STM32F1
        NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0);
        NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
#endif
#ifdef STM32F3
        NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 0);
        NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
#endif
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle)
{
    if (pcdHandle->Instance == USB) {
        // Peripheral clock disable
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USB);

        // Peripheral interrupt Deinit
#ifdef STM32F1
        NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
#endif
#ifdef STM32F3
        NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn);
#endif
    }
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SetupStage((USBD_HandleTypeDef *)hpcd->pData, (uint8_t *)hpcd->Setup);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataOutStage((USBD_HandleTypeDef *)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage((USBD_HandleTypeDef *)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SOF((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    // Set USB current speed
    switch (hpcd->Init.speed) {
    case PCD_SPEED_FULL:
        speed = USBD_SPEED_FULL;
        break;

    default:
        speed = USBD_SPEED_FULL;
        break;
    }
    USBD_LL_SetSpeed((USBD_HandleTypeDef *)hpcd->pData, speed);

    // Reset Device
    USBD_LL_Reset((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    // Inform USB library that core enters in suspend Mode
    USBD_LL_Suspend((USBD_HandleTypeDef *)hpcd->pData);
    // Enter in STOP mode

    if (hpcd->Init.low_power_enable) {
        // Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register
        SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Resume((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef *)hpcd->pData, epnum);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete((USBD_HandleTypeDef *)hpcd->pData, epnum);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevConnected((USBD_HandleTypeDef *)hpcd->pData);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevDisconnected((USBD_HandleTypeDef *)hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
  * @brief  Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    // Init USB Ip
    // Link the driver to the stack
    hpcd_USB_FS.pData = pdev;
    pdev->pData = &hpcd_USB_FS;

    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_FS.Init.lpm_enable = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;

    if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK) {
        // TODO: Handle error here
    }

    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x00, PCD_SNG_BUF, 0x18);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, 0x80, PCD_SNG_BUF, 0x58);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *)pdev->pData, AMP_EPIN_ADDR, PCD_SNG_BUF, 0x98);
    return USBD_OK;
}

/**
  * @brief  De-Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_DeInit(pdev->pData);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Starts the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Start(pdev->pData);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Stops the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Stop(pdev->pData);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Opens an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  ep_type: Endpoint type
  * @param  ep_mps: Endpoint max packet size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type,
                                  uint16_t ep_mps)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Closes an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *) pdev->pData;

    if ((ep_addr & 0x80) == 0x80) {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    } else {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  dev_addr: Device address
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf,
                                    uint16_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf,
                                          uint16_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

    switch (hal_status) {
    case HAL_OK :
        usb_status = USBD_OK;
        break;
    case HAL_ERROR :
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY :
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT :
        usb_status = USBD_FAIL;
        break;
    default :
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef *) pdev->pData, ep_addr);
}

/**
  * @brief  Delays routine for the USB device library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay)
{
    LL_mDelay(Delay);
}

/**
  * @brief  Static single allocation.
  * @param  size: Size of allocated memory
  * @retval None
  */
void *USBD_static_malloc(uint32_t size)
{
    static uint32_t mem[(sizeof(USBD_AMP_HandleTypeDef) / 4 + 1)]; /* On 32-bit boundary */
    return mem;
}

/**
  * @brief  Dummy memory free
  * @param  p: Pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p)
{

}

/**
  * @brief Software Device Connection
  * @param hpcd: PCD handle
  * @param state: Connection state (0: disconnected / 1: connected)
  * @retval None
  */
void HAL_PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state)
{
    if (state == 1) {
        // Configure Low connection state
    } else {
        // Configure High connection state
    }
}

#ifdef STM32F1
void USB_LP_CAN1_RX0_IRQHandler(void)
#endif
#ifdef STM32F3
void USB_LP_CAN_RX0_IRQHandler(void)
#endif
{
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
