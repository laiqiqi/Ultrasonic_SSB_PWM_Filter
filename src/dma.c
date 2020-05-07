#include "dma.h"
#include "main.h"

DMA_HandleTypeDef hdma_adc1 = {0};

void DMA2_Stream0_IRQHandler() {
	HAL_DMA_IRQHandler(&hdma_adc1);
}

void DMA_ADC1_init(ADC_HandleTypeDef* hadc1) {

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

   hdma_adc1.Instance = DMA2_Stream0;

   hdma_adc1.Init.Channel  = DMA_CHANNEL_0;
   hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
   hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
   hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
   hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
   hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
   hdma_adc1.Init.Mode = DMA_CIRCULAR;
   hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
   hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
   hdma_adc1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
   hdma_adc1.Init.MemBurst = DMA_MBURST_SINGLE;
   hdma_adc1.Init.PeriphBurst = DMA_PBURST_SINGLE;

   HAL_DMA_Init(&hdma_adc1);

   __HAL_LINKDMA(hadc1, DMA_Handle, hdma_adc1);

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

