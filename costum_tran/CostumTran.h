/**
  ******************************************************************************
  * @file    Ymodem.h
  * @author  XinLi
  * @version v1.0
  * @date    21-January-2018
  * @brief   Header file for Ymodem.cpp module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>Copyright &copy; 2018 XinLi</center></h2>
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

#ifndef __COSTUMTRAN_H
#define __COSTUMTRAN_H

/* Header includes -----------------------------------------------------------*/
#include <stdint.h>
#include <QDebug>

/* Macro definitions ---------------------------------------------------------*/
#define YMODEM_PACKET_HEADER    (3)
#define YMODEM_PACKET_TRAILER   (2)
#define YMODEM_PACKET_OVERHEAD  (YMODEM_PACKET_HEADER + YMODEM_PACKET_TRAILER)
#define YMODEM_PACKET_SIZE      (128)
#define YMODEM_PACKET_1K_SIZE   (1024)

#define YMODEM_CODE_CAN_NUMBER  (5)

#define COSTUM_CODE_CONTROL_NUMBER (3)
#define DATA_PACKAGE_LEN        (270)
#define ALARM_PACKAGE_LEN       (88)

/* Type definitions ----------------------------------------------------------*/
class CostumTran
{
public:
    // 3+4+6+19+10+4+5+4+9+4+12+3+7+4+7+4+9+4+7+4+(4+4)*9+(5+4)*7
    typedef struct _v_t
    {
        char reserve[4];
        char volt[4];
    } _v;

    typedef struct _v2_t
    {
        char reserve[5];
        char volt[4];
    }_v2;

    typedef union _recv_data_t
    {
        char data[DATA_PACKAGE_LEN];
        struct
        {
            char head[3];
            char reserve1[3];
            char number[4];
            char reserve2[6];
            char time[19];
            char reserve3[10];
            char packvolt[4];
            char reserve4[5];
            char soc[4];
            char reserve5[9];
            char packcur[4];
            char reserve6[12];
            char packstatus[3];
            char reserve7[7];
            char temp1[4];
            char reserve8[7];
            char temp2[4];
            char reserve9[9];
            char pcbtemp[4];
            char reserve10[7];
            char stemp[4];
            _v v[9];
            _v2 v2[7];
            char tail[3];
        }_instance;
    }_recv;

  enum Code
  {
        CodeNone = 0x00,
        CodeSoh  = 0x01,
        CodeStx  = 0x02,
        CodeEot  = 0x04,
        CodeAck  = 0x06,
        CodeNak  = 0x15,
        CodeCan  = 0x18,
        CodeC    = 0x43,
        CodeA1   = 0x41,
        CodeA2   = 0x61,
        CodeExclamationMark = 0x21,
        CodeQuestionMark = 0x3F
  };

  enum Stage
  {
    StageNone,
    StageEstablishing,
    StageEstablished,
    StageTransmitting,
    StageFinishing,
    StageFinished
  };

  enum Status
  {
    StatusEstablish,
    StatusTransmit,
    StatusFinish,
    StatusAbort,
    StatusTimeout,
    StatusError
  };

  enum Mode
  {
      RequestData,
      RequestAlarm
  };

  CostumTran(uint32_t timeDivide = 499, uint32_t timeMax = 5, uint32_t errorMax = 999);

  void setTimeDivide(uint32_t timeDivide);
  uint32_t getTimeDivide();

  void setTimeMax(uint32_t timeMax);
  uint32_t getTimeMax();

  void setErrorMax(uint32_t errorMax);
  uint32_t getErrorMax();

  void receive();
  void transmit();
  void abort();

  Code getControlCode() const;
  Mode getMode() const;
  void setMode(const Mode &value);

private:
  Code receivePacket();

  void receiveStageNone();
  void receiveStageEstablishing();
  void receiveStageEstablished();
  void receiveStageTransmitting();
  void receiveStageFinishing();
  void receiveStageFinished();

  void transmitStageNone();
  void transmitStageEstablishing();
  void transmitStageEstablished();
  void transmitStageTransmitting();
  void transmitStageFinishing();
  void transmitStageFinished();

  uint16_t crc16(uint8_t *buff, uint32_t len);

  virtual Code callback(Status status, uint8_t *buff, uint32_t *len) = 0;

  virtual uint32_t read(uint8_t *buff, uint32_t len)  = 0;
  virtual uint32_t write(uint8_t *buff, uint32_t len) = 0;

  uint32_t timeDivide;
  uint32_t timeMax;
  uint32_t errorMax;

  uint32_t timeCount;
  uint32_t errorCount;
  uint8_t  dataCount;
  uint8_t controlNumCount;

  Code  code;
  Stage stage;
  Mode mode;

  uint8_t  rxBuffer[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD];
  uint8_t  txBuffer[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD];
  uint32_t rxLength;
  uint32_t txLength;
};

/* Variable declarations -----------------------------------------------------*/
/* Variable definitions ------------------------------------------------------*/
/* Function declarations -----------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

#endif /* __YMODEM_H */
