/**
 * $Id: $
 *
 * @brief Red Pitaya Scpi server SPI SCPI commands interface
 *
 * @Author Red Pitaya
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */


#ifndef RPHW_SPI_H_
#define RPHW_SPI_H_

#include "scpi/types.h"

scpi_result_t RP_SPI_Init(scpi_t * context);
scpi_result_t RP_SPI_InitDev(scpi_t *context);
scpi_result_t RP_SPI_Release(scpi_t * context);
scpi_result_t RP_SPI_SetDefault(scpi_t * context);
scpi_result_t RP_SPI_SetSettings(scpi_t * context);
scpi_result_t RP_SPI_GetSettings(scpi_t * context);

scpi_result_t RP_SPI_CreateMessage(scpi_t * context);
scpi_result_t RP_SPI_DestroyMessage(scpi_t * context);
scpi_result_t RP_SPI_GetMessageLen(scpi_t * context);
scpi_result_t RP_SPI_GetRXBuffer(scpi_t * context);
scpi_result_t RP_SPI_GetTXBuffer(scpi_t * context);
scpi_result_t RP_SPI_GetCSChangeState(scpi_t * context);


scpi_result_t RP_SPI_SetTX(scpi_t * context);
scpi_result_t RP_SPI_SetTXRX(scpi_t * context);
scpi_result_t RP_SPI_SetRX(scpi_t * context);
scpi_result_t RP_SPI_SetTXCS(scpi_t * context);
scpi_result_t RP_SPI_SetTXRXCS(scpi_t * context);
scpi_result_t RP_SPI_SetRXCS(scpi_t * context);

scpi_result_t RP_SPI_SetMode(scpi_t * context);
scpi_result_t RP_SPI_GetMode(scpi_t * context);

scpi_result_t RP_SPI_SetSpeed(scpi_t * context);
scpi_result_t RP_SPI_GetSpeed(scpi_t * context);

scpi_result_t RP_SPI_SetWord(scpi_t * context);
scpi_result_t RP_SPI_GetWord(scpi_t * context);

scpi_result_t RP_SPI_Pass(scpi_t * context);

#endif /* RPHW_UART_H_ */
