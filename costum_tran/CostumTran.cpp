/**
  ******************************************************************************
  * @file    Ymodem.cpp
  * @author  XinLi
  * @version v1.0
  * @date    21-January-2018
  * @brief   Ymodem protocol module source file.
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

/* Header includes -----------------------------------------------------------*/
#include "CostumTran.h"
#include <string.h>

/* Macro definitions ---------------------------------------------------------*/
/* Type definitions ----------------------------------------------------------*/
/* Variable declarations -----------------------------------------------------*/
/* Variable definitions ------------------------------------------------------*/
/* Function declarations -----------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

/**
  * @brief  Ymodem constructor.
  * @param  [in] timeDivide: The fractional factor of the time the ymodem is called.
  * @param  [in] timeMax:    The maximum time when calling the ymodem.
  * @param  [in] errorMax:   The maximum error count when calling the ymodem.
  * @note   The longest waiting time = call time / (@timeDivide + 1) * (@timeMax + 1).
  * @return None.
  */
CostumTran::CostumTran(uint32_t timeDivide, uint32_t timeMax, uint32_t errorMax)
{
  this->timeDivide = timeDivide;
  this->timeMax    = timeMax;
  this->errorMax   = errorMax;

  this->timeCount  = 0;
  this->errorCount = 0;
  this->dataCount  = 0;
    this->controlNumCount = 0;
    this->rxLength = 0;

  this->code       = CodeNone;
  this->stage      = StageNone;
//    this->mode     = RequestData;
    this->mode     = RequestAlarm;
}

/**
  * @brief  Set the fractional factor of the time the ymodem is called.
  * @param  [in] timeDivide: The fractional factor of the time the ymodem is called.
  * @return None.
  */
void CostumTran::setTimeDivide(uint32_t timeDivide)
{
  this->timeDivide = timeDivide;
}

/**
  * @brief  Get the fractional factor of the time the ymodem is called.
  * @param  None.
  * @return The fractional factor of the time the ymodem is called.
  */
uint32_t CostumTran::getTimeDivide()
{
  return timeDivide;
}

/**
  * @brief  Set the maximum time when calling the ymodem.
  * @param  [in] timeMax: The maximum time when calling the ymodem.
  * @return None.
  */
void CostumTran::setTimeMax(uint32_t timeMax)
{
  this->timeMax = timeMax;
}

/**
  * @brief  Get the maximum time when calling the ymodem.
  * @param  None.
  * @return The maximum time when calling the ymodem.
  */
uint32_t CostumTran::getTimeMax()
{
  return timeMax;
}

/**
  * @brief  Set the maximum error count when calling the ymodem.
  * @param  [in] errorMax: The maximum error count when calling the ymodem.
  * @return None.
  */
void CostumTran::setErrorMax(uint32_t errorMax)
{
  this->errorMax = errorMax;
}

/**
  * @brief  Get the maximum error count when calling the ymodem.
  * @param  None.
  * @return The maximum error count when calling the ymodem.
  */
uint32_t CostumTran::getErrorMax()
{
  return errorMax;
}

/**
  * @brief  Ymodem receive.
  * @param  None.
  * @return None.
  */
void CostumTran::receive()
{
  switch(stage)
  {
    case StageNone:
    {
      receiveStageNone();

      break;
    }

    case StageEstablishing:
    {
      receiveStageEstablishing();

      break;
    }

    case StageEstablished:
    {
      receiveStageEstablished();

      break;
    }

    case StageTransmitting:
    {
      receiveStageTransmitting();

      break;
    }

    case StageFinishing:
    {
      receiveStageFinishing();

      break;
    }

    default:
    {
      receiveStageFinished();
    }
  }
}

/**
  * @brief  Ymodem transmit.
  * @param  None.
  * @return None.
  */
void CostumTran::transmit()
{
  switch(stage)
  {
    case StageNone:
    {
      transmitStageNone();

      break;
    }

    case StageEstablishing:
    {
      transmitStageEstablishing();

      break;
    }

    case StageEstablished:
    {
      transmitStageEstablished();

      break;
    }

    case StageTransmitting:
    {
      transmitStageTransmitting();

      break;
    }

    case StageFinishing:
    {
      transmitStageFinishing();

      break;
    }

    default:
    {
      transmitStageFinished();
    }
  }
}

/**
  * @brief  Ymodem abort.
  * @param  None.
  * @return None.
  */
void CostumTran::abort()
{
  timeCount  = 0;
  errorCount = 0;
  dataCount  = 0;
  code       = CodeNone;
  stage      = StageNone;

  for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
  {
    txBuffer[txLength] = CodeCan;
  }

  write(txBuffer, txLength);
}

/**
  * @brief  Receives a packet of data.
  * @param  None.
  * @return Packet type.
  */
CostumTran::Code CostumTran::receivePacket()
{
  if(code == CodeNone)
  {
    if(read(&(rxBuffer[rxLength]), 1) > 0)
    {
      if(rxBuffer[rxLength] == CodeExclamationMark)
      {
          rxLength += 1;
        return CodeExclamationMark;

      }
      else
      {
        return CodeNone;
      }
    }
    else
    {
      return CodeNone;
    }
  }
  else
  {
    if(code == CodeSoh)
    {
        if(RequestData == getMode())
        {
            uint32_t len = read(&(rxBuffer[rxLength]), DATA_PACKAGE_LEN  - rxLength);

            if(len < DATA_PACKAGE_LEN - rxLength)
            {
              rxLength += len;

              return CodeNone;
            }
            else
            {

              code = CodeNone;

              return CodeSoh;
            }
        }
        else if(RequestAlarm == getMode())
        {
            uint32_t len = read(&(rxBuffer[rxLength]), ALARM_PACKAGE_LEN  - rxLength);

            if(len < ALARM_PACKAGE_LEN - rxLength)
            {
              rxLength += len;

              return CodeNone;
            }
            else
            {

              code = CodeNone;

              return CodeSoh;
            }
        }
        else
        {

          code = CodeNone;

          return CodeSoh;
        }
    }
    else if(code == CodeStx)
    {
      uint32_t len = read(&(rxBuffer[rxLength]), YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD - rxLength);

      if(len < (YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD - rxLength))
      {
        rxLength += len;

        return CodeNone;
      }
      else
      {
        code = CodeNone;

        return CodeStx;
      }
    }
    else
    {
      code = CodeNone;

      return CodeNone;
    }
  }
}

/**
  * @brief  Receive none stage.
  * @param  None.
  * @return None.
  */
void CostumTran::receiveStageNone()
{
  timeCount   = 0;
  errorCount  = 0;
  dataCount   = 0;
  code        = CodeNone;
  stage       = StageEstablishing;
  memset(txBuffer, getControlCode(), 3);
  txLength    = 3;
  write(txBuffer, txLength);
}

/**
  * @brief  Receive establishing stage.
  * @param  None.
  * @return None.
  */
void CostumTran::receiveStageEstablishing()
{
  switch(receivePacket())
  {
    case CodeExclamationMark:
    {
      controlNumCount++;

      if(controlNumCount >= COSTUM_CODE_CONTROL_NUMBER)
      {
          timeCount   = 0;
          errorCount  = 0;
          dataCount   = 0;
          controlNumCount = 0;
          code        = CodeSoh;
          stage       = StageTransmitting;
          break;
        }
    }
    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

//        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
//        {
//          txBuffer[txLength] = CodeCan;
//        }

//        write(txBuffer, txLength);
        callback(StatusTimeout, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
//        txBuffer[0] = CodeC;
//        txLength    = 1;
//        write(txBuffer, txLength);
        for(txLength = 0; txLength < COSTUM_CODE_CONTROL_NUMBER; txLength++)
        {
          txBuffer[txLength] = getControlCode();
        }

        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Receive established stage.
  * @param  None.
  * @return None.
  */
void CostumTran::receiveStageEstablished()
{
    timeCount   = 0;
    errorCount  = 0;
    dataCount   = 0;
    controlNumCount = 0;
    code        = CodeSoh;
    stage       = StageTransmitting;
}

/**
  * @brief  Receive transmitting stage.
  * @param  None.
  * @return None.
  */
void CostumTran::receiveStageTransmitting()
{
  switch(receivePacket())
  {
      case CodeSoh:
      {
        if(RequestData == this->mode)
        {
            _recv temp;
            memcpy(temp.data, rxBuffer, DATA_PACKAGE_LEN);
            if((temp._instance.tail[0] == CodeQuestionMark) && (temp._instance.tail[1] == CodeQuestionMark) && (temp._instance.tail[2] == CodeQuestionMark))
            {

                qDebug() << "got one package.";
                char tmp[10];
                memset(tmp, 0x00, 10);
                memcpy(tmp, temp._instance.number, sizeof(temp._instance.number));
                qDebug() << tmp;
                qDebug() << QString(tmp).toInt();
                timeCount   = 0;
                errorCount  = 0;
                dataCount   = 0;
                rxLength    = 0;
                code        = CodeNone;
                stage       = StageEstablishing;
                break;
            }
            else
            {
                qDebug() << "packet tail check error.";
                errorCount++;
                if(errorCount > errorMax)
                {
                    timeCount  = 0;
                    errorCount = 0;
                    dataCount  = 0;
                    rxLength   = 0;
                    code       = CodeNone;
                    stage      = StageNone;
                }
                else
                {
                    timeCount   = 0;
                    dataCount   = 0;
                    rxLength    = 0;
                    code        = CodeNone;
                    stage       = StageEstablishing;
                }

                break;
            }
        }
        else if(RequestAlarm == this->mode)
        {
            if(rxBuffer[ALARM_PACKAGE_LEN - 1] == CodeQuestionMark && rxBuffer[ALARM_PACKAGE_LEN - 2] == CodeQuestionMark && rxBuffer[ALARM_PACKAGE_LEN - 3] == CodeQuestionMark)
            {
                qDebug() << "got one alarm pack.";
                timeCount   = 0;
                errorCount  = 0;
                dataCount   = 0;
                rxLength    = 0;
                code        = CodeNone;
                stage       = StageEstablishing;
                break;
            }
            else
            {
                qDebug() << "alarm pack tail check failed.";
                errorCount++;
                if(errorCount > errorMax)
                {
                    timeCount  = 0;
                    errorCount = 0;
                    dataCount  = 0;
                    rxLength   = 0;
                    code       = CodeNone;
                    stage      = StageNone;
                }
                else
                {
                    timeCount   = 0;
                    dataCount   = 0;
                    rxLength    = 0;
                    code        = CodeNone;
                    stage       = StageEstablishing;
                }

                break;
            }
        }
      }
    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        rxLength   = 0;
        code       = CodeNone;
        stage      = StageNone;

//        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
//        {
//          txBuffer[txLength] = CodeCan;
//        }

//        write(txBuffer, txLength);
        callback(StatusTimeout, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
//        txBuffer[0] = CodeNak;
//        txLength    = 1;
//        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Receive finishing stage.
  * @param  None.
  * @return None.
  */
void CostumTran::receiveStageFinishing()
{
  switch(receivePacket())
  {
    case CodeEot:
    {
      timeCount   = 0;
      errorCount  = 0;
      dataCount   = 0;
      code        = CodeNone;
      stage       = StageFinished;
      txBuffer[0] = CodeAck;
      txBuffer[1] = CodeC;
      txLength    = 2;
      write(txBuffer, txLength);

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
        txBuffer[0] = CodeNak;
        txLength    = 1;
        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Receive finished stage.
  * @param  None.
  * @return None.
  */
void CostumTran::receiveStageFinished()
{
    qDebug() << "#####################start to read data";
  switch(receivePacket())
  {
    case CodeSoh:
    {
      uint16_t crc = ((uint16_t)(rxBuffer[YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD - 2]) << 8) |
                     ((uint16_t)(rxBuffer[YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD - 1]) << 0);

      if((rxBuffer[1] == 0x00) && (rxBuffer[2] == 0xFF) &&
         (crc == crc16(&(rxBuffer[YMODEM_PACKET_HEADER]), YMODEM_PACKET_SIZE)))
      {
        timeCount   = 0;
        errorCount  = 0;
        dataCount   = 0;
        code        = CodeNone;
        stage       = StageNone;
        txBuffer[0] = CodeAck;
        txLength    = 1;
        write(txBuffer, txLength);
        callback(StatusFinish, NULL, NULL);
      }
      else
      {
        errorCount++;

        if(errorCount > errorMax)
        {
          timeCount  = 0;
          errorCount = 0;
          dataCount  = 0;
          code       = CodeNone;
          stage      = StageNone;

          for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
          {
            txBuffer[txLength] = CodeCan;
          }

          write(txBuffer, txLength);
          callback(StatusError, NULL, NULL);
        }
        else
        {
          txBuffer[0] = CodeNak;
          txLength    = 1;
          write(txBuffer, txLength);
        }
      }

      break;
    }

    case CodeEot:
    {
      errorCount++;

      if(errorCount > errorMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else
      {
        txBuffer[0] = CodeAck;
        txBuffer[1] = CodeC;
        txLength    = 2;
        write(txBuffer, txLength);
      }

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
        txBuffer[0] = CodeNak;
        txLength    = 1;
        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Transmit none stage.
  * @param  None.
  * @return None.
  */
void CostumTran::transmitStageNone()
{
  timeCount   = 0;
  errorCount  = 0;
  dataCount   = 0;
  code        = CodeNone;
  stage       = StageEstablishing;
}

/**
  * @brief  Transmit establishing stage.
  * @param  None.
  * @return None.
  */
void CostumTran::transmitStageEstablishing()
{
  switch(receivePacket())
  {
    case CodeC:
    {
      memset(&(txBuffer[YMODEM_PACKET_HEADER]), 0x00, YMODEM_PACKET_SIZE);

      if(callback(StatusEstablish, &(txBuffer[YMODEM_PACKET_HEADER]), &(txLength)) == CodeAck)
      {
        uint16_t crc = crc16(&(txBuffer[YMODEM_PACKET_HEADER]), txLength);

        timeCount                                       = 0;
        errorCount                                      = 0;
        dataCount                                       = 0;
        code                                            = CodeNone;
        stage                                           = StageEstablished;
        txBuffer[0]                                     = CodeSoh;
        txBuffer[1]                                     = 0x00;
        txBuffer[2]                                     = 0xFF;
        txBuffer[txLength + YMODEM_PACKET_OVERHEAD - 2] = (uint8_t)(crc >> 8);
        txBuffer[txLength + YMODEM_PACKET_OVERHEAD - 1] = (uint8_t)(crc >> 0);
        txLength                                        = txLength + YMODEM_PACKET_OVERHEAD;
        write(txBuffer, txLength);
      }
      else
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
      }

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusTimeout, NULL, NULL);
      }
    }
  }
}

/**
  * @brief  Transmit established stage.
  * @param  None.
  * @return None.
  */
void CostumTran::transmitStageEstablished()
{
  switch(receivePacket())
  {
    case CodeNak:
    {
      errorCount++;

      if(errorCount > errorMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else
      {
        write(txBuffer, txLength);
      }

      break;
    }

    case CodeC:
    {
      errorCount++;

      if(errorCount > errorMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = dataCount;
        code       = CodeNone;
        stage      = (Stage)(stage + dataCount);
        write(txBuffer, txLength);
      }

      break;
    }

    case CodeAck:
    {
      memset(&(txBuffer[YMODEM_PACKET_HEADER]), 0x00, YMODEM_PACKET_1K_SIZE);

      switch(callback(StatusTransmit, &(txBuffer[YMODEM_PACKET_HEADER]), &(txLength)))
      {
        case CodeAck:
        {
          uint16_t crc = crc16(&(txBuffer[YMODEM_PACKET_HEADER]), txLength);

          timeCount                                       = 0;
          errorCount                                      = 0;
          dataCount                                       = 1;
          code                                            = CodeNone;
          stage                                           = StageEstablished;
          txBuffer[0]                                     = txLength > YMODEM_PACKET_SIZE ? CodeStx : CodeSoh;
          txBuffer[1]                                     = 0x01;
          txBuffer[2]                                     = 0xFE;
          txBuffer[txLength + YMODEM_PACKET_OVERHEAD - 2] = (uint8_t)(crc >> 8);
          txBuffer[txLength + YMODEM_PACKET_OVERHEAD - 1] = (uint8_t)(crc >> 0);
          txLength                                        = txLength + YMODEM_PACKET_OVERHEAD;

          break;
        }

        case CodeEot:
        {
          timeCount   = 0;
          errorCount  = 0;
          dataCount   = 2;
          code        = CodeNone;
          stage       = StageEstablished;
          txBuffer[0] = CodeEot;
          txLength    = 1;
          write(txBuffer, txLength);

          break;
        }

        default:
        {
          timeCount  = 0;
          errorCount = 0;
          dataCount  = 0;
          code       = CodeNone;
          stage      = StageNone;

          for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
          {
            txBuffer[txLength] = CodeCan;
          }

          write(txBuffer, txLength);
        }
      }

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Transmit transmitting stage.
  * @param  None.
  * @return None.
  */
void CostumTran::transmitStageTransmitting()
{
  switch(receivePacket())
  {
    case CodeNak:
    {
      errorCount++;

      if(errorCount > errorMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else
      {
        write(txBuffer, txLength);
      }

      break;
    }

    case CodeAck:
    {
      memset(&(txBuffer[YMODEM_PACKET_HEADER]), 0x00, YMODEM_PACKET_1K_SIZE);

      switch(callback(StatusTransmit, &(txBuffer[YMODEM_PACKET_HEADER]), &(txLength)))
      {
        case CodeAck:
        {
          uint16_t crc = crc16(&(txBuffer[YMODEM_PACKET_HEADER]), txLength);

          timeCount                                       = 0;
          errorCount                                      = 0;
          dataCount                                       = dataCount + 1;
          code                                            = CodeNone;
          stage                                           = StageTransmitting;
          txBuffer[0]                                     = txLength > YMODEM_PACKET_SIZE ? CodeStx : CodeSoh;
          txBuffer[1]                                     = dataCount;
          txBuffer[2]                                     = 0xFF - dataCount;
          txBuffer[txLength + YMODEM_PACKET_OVERHEAD - 2] = (uint8_t)(crc >> 8);
          txBuffer[txLength + YMODEM_PACKET_OVERHEAD - 1] = (uint8_t)(crc >> 0);
          txLength                                        = txLength + YMODEM_PACKET_OVERHEAD;
          write(txBuffer, txLength);

          break;
        }

        case CodeEot:
        {
          timeCount   = 0;
          errorCount  = 0;
          dataCount   = 0;
          code        = CodeNone;
          stage       = StageFinishing;
          txBuffer[0] = CodeEot;
          txLength    = 1;
          write(txBuffer, txLength);

          break;
        }

        default:
        {
          timeCount  = 0;
          errorCount = 0;
          dataCount  = 0;
          code       = CodeNone;
          stage      = StageNone;

          for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
          {
            txBuffer[txLength] = CodeCan;
          }

          write(txBuffer, txLength);
        }
      }

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Transmit finishing stage.
  * @param  None.
  * @return None.
  */
void CostumTran::transmitStageFinishing()
{
  switch(receivePacket())
  {
    case CodeNak:
    {
      timeCount   = 0;
      errorCount  = 0;
      dataCount   = 0;
      code        = CodeNone;
      stage       = StageFinishing;
      txBuffer[0] = CodeEot;
      txLength    = 1;
      write(txBuffer, txLength);

      break;
    }

    case CodeC:
    {
      memset(&(txBuffer[YMODEM_PACKET_HEADER]), 0x00, YMODEM_PACKET_SIZE);
      uint16_t crc = crc16(&(txBuffer[YMODEM_PACKET_HEADER]), YMODEM_PACKET_SIZE);

      timeCount                                                 = 0;
      errorCount                                                = 0;
      dataCount                                                 = 0;
      code                                                      = CodeNone;
      stage                                                     = StageFinished;
      txBuffer[0]                                               = CodeSoh;
      txBuffer[1]                                               = 0x00;
      txBuffer[2]                                               = 0xFF;
      txBuffer[YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD - 2] = (uint8_t)(crc >> 8);
      txBuffer[YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD - 1] = (uint8_t)(crc >> 0);
      txLength                                                  = YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD;
      write(txBuffer, txLength);

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Transmit finished stage.
  * @param  None.
  * @return None.
  */
void CostumTran::transmitStageFinished()
{
  switch(receivePacket())
  {
    case CodeC:
    case CodeNak:
    {
      errorCount++;

      if(errorCount > errorMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else
      {
        write(txBuffer, txLength);
      }

      break;
    }

    case CodeAck:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusFinish, NULL, NULL);

      break;
    }

    case CodeA1:
    case CodeA2:
    case CodeCan:
    {
      timeCount  = 0;
      errorCount = 0;
      dataCount  = 0;
      code       = CodeNone;
      stage      = StageNone;
      callback(StatusAbort, NULL, NULL);

      break;
    }

    default:
    {
      timeCount++;

      if((timeCount / (timeDivide + 1)) > timeMax)
      {
        timeCount  = 0;
        errorCount = 0;
        dataCount  = 0;
        code       = CodeNone;
        stage      = StageNone;

        for(txLength = 0; txLength < YMODEM_CODE_CAN_NUMBER; txLength++)
        {
          txBuffer[txLength] = CodeCan;
        }

        write(txBuffer, txLength);
        callback(StatusError, NULL, NULL);
      }
      else if((timeCount % (timeDivide + 1)) == 0)
      {
        write(txBuffer, txLength);
      }
    }
  }
}

/**
  * @brief  Calculate CRC16 checksum.
  * @param  [in] buff: The data to be calculated.
  * @param  [in] len:  The length of the data to be calculated.
  * @return Calculated CRC16 checksum.
  */
uint16_t CostumTran::crc16(uint8_t *buff, uint32_t len)
{
  uint16_t crc = 0;

  while(len--)
  {
    crc ^= (uint16_t)(*(buff++)) << 8;

    for(int i = 0; i < 8; i++)
    {
      if(crc & 0x8000)
      {
        crc = (crc << 1) ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    }
  }

  return crc;
}

CostumTran::Code CostumTran::getControlCode() const
{
    if(this->mode == RequestData)
    {
        return CodeExclamationMark;
    }
    else if(RequestAlarm == this->mode)
    {
        return CodeQuestionMark;
    }
    else
    {
        return CodeNone;
    }
}

CostumTran::Mode CostumTran::getMode() const
{
    return mode;
}

void CostumTran::setMode(const Mode &value)
{
    mode = value;
}
