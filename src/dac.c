#include "dac.h"

DAC_HandleTypeDef hdac;

void DAC_init(void) {
  DAC_ChannelConfTypeDef sConfig = {0};

  hdac.Instance = DAC;

  if (HAL_DAC_Init(&hdac) != HAL_OK)
    Error_Handler();

  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_00) != HAL_OK)
    Error_Handler();
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_90) != HAL_OK)
    Error_Handler();

  DAC_set_channel_value(DAC_CHANNEL_00, 0);
  DAC_set_channel_value(DAC_CHANNEL_90, 0);
}

void DAC_start(){
    HAL_DAC_Start(&hdac, DAC_CHANNEL_00);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_90);
}

void DAC_set_channel_value(uint32_t channel, uint32_t value) {
    HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, value);
}
