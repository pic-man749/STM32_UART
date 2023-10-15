/*
 * uart.c
 *
 *  Created on: Jul 18, 2023
 *      Author: picman
 */
#include "uart.h"

static uint8_t uart_recv_dma[UART_BUFFER_SIZE];
static uint16_t uart_recv_dma_read_idx = 0;

void sputc(char c)
{
  HAL_UART_Transmit(p_huart, (uint8_t *)&c, 1, UART_TIMEOUT);
  return;
}

void sputs(char *str)
{
  int cnt = 0;
  for(cnt=0; cnt<UART_BUFFER_SIZE; cnt++){
    if(str[cnt]=='\0') break;
  }
  HAL_UART_Transmit(p_huart, (uint8_t *)str, cnt, UART_TIMEOUT);
  return;
}

void sputsln(char *str)
{
  sputs(str);
  HAL_UART_Transmit(p_huart, (uint8_t *)NEW_LINE, strlen(NEW_LINE), UART_TIMEOUT);
}

int sputsf(char *format_str, ...)
{
  char *tmp[UART_BUFFER_SIZE];

  va_list args;
  va_start( args, format_str );

  if(strlen(format_str) > UART_BUFFER_SIZE){
    va_end(args);
    return -1;
  }

  vsprintf(tmp, format_str, args);
  sputs(tmp);

  return 0;
}


char sgetc(void)
{
  char c;
  while(1){
    uint32_t write_idx = (UART_BUFFER_SIZE - p_huart->hdmarx->Instance->CNDTR);
    if(write_idx == uart_recv_dma_read_idx){
      continue;
    }
    c = uart_recv_dma[uart_recv_dma_read_idx++];
    if(uart_recv_dma_read_idx >= UART_BUFFER_SIZE){
      uart_recv_dma_read_idx = 0;
    }
    break;
  }
  return (char)c;
}

int sgets(char *buffer, int len)
{
  int idx = 0;

  while(1){
    char c = sgetc();
    if(c == RETURN_CHAR){
      buffer[idx++] = '\0';
      break;
    }else if(c == '\b'){
      buffer[--idx] = '\0';
    }else{
      buffer[idx++] = c;
    }

    if(idx >= UART_BUFFER_SIZE-1){
      buffer[UART_BUFFER_SIZE-1] = '\0';
      break;
    }
  }
  return idx;
}

void startRecv(){
  HAL_UART_Receive_DMA(p_huart, uart_recv_dma, UART_BUFFER_SIZE);
}
