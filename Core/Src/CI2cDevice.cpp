/*
 * CI2cDevice.cpp
 *
 *  Created on: March 3, 2026
 *      Author: mnl
 */

#include "CI2cDevice.h"

#define I2C_TIMEOUT 1000

CI2cDevice::CI2cDevice(I2C_HandleTypeDef *i2cController, uint8_t slaveAddress) {
	m_i2cController = i2cController;
	if (slaveAddress > 127) {
		m_slaveAddress = 127;
	} else {
		m_slaveAddress = slaveAddress;
	}
}


//	HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
//		uint16_t DevAddress,
//		uint8_t *pData,
//		uint16_t Size,
//		uint32_t Timeout)
bool CI2cDevice::quickCommand(bool readNotWrite) {
	HAL_StatusTypeDef state;

	//Send Address and R oder W bit
	if (!readNotWrite) {
		state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, NULL,
				0, I2C_TIMEOUT);
	} else {
		state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, NULL, 0,
		I2C_TIMEOUT);
	}
	return (state == HAL_OK);
}

bool CI2cDevice::sendByte(uint8_t data) {
	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &data, 1,
	I2C_TIMEOUT);

	return (state == HAL_OK);
}

bool CI2cDevice::writeByte(uint8_t commandCode, uint8_t data) {
	HAL_StatusTypeDef state;
	uint8_t buffer[2] = { commandCode, data };

	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 2,
	I2C_TIMEOUT);

	return (state == HAL_OK);
}

bool CI2cDevice::writeWord(uint8_t commandCode, uint16_t data) {
	HAL_StatusTypeDef state;
	uint8_t buffer[3];

	//method 1: bit schieben.
	buffer[0] = commandCode;
	buffer[1] = (uint8_t) (data & 0xFFU);
	buffer[2] = (uint8_t) ((data >> 8U) & 0xFFU);

	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 3U,
	I2C_TIMEOUT);
	return (state == HAL_OK);
}

bool CI2cDevice::write32(uint8_t commandCode, uint32_t data) {
	HAL_StatusTypeDef state;
	uint8_t buffer[5];
	buffer[0] = commandCode;
	//method 2: memcpy();
	//void *	 memcpy (void *__restrict, const void *__restrict, size_t);
	memcpy(&buffer[1], &data, sizeof(data));

	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 5U,
	I2C_TIMEOUT);
	return (state == HAL_OK);
}

bool CI2cDevice::write64(uint8_t commandCode, uint64_t data) {
	HAL_StatusTypeDef state;
	uint8_t buffer[9];
	buffer[0] = commandCode;
	memcpy(&buffer[1], &data, sizeof(data));

	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 9U,
	I2C_TIMEOUT);
	return (state == HAL_OK);
}

bool CI2cDevice::blockWrite(uint8_t commandCode, const uint8_t *data,
		uint8_t byteCount) {
	if (byteCount > 255 || byteCount == 0 || data == nullptr) {
		return false;
	}
	uint8_t sizeOfPakage = byteCount + 2;
	uint8_t buffer[sizeOfPakage];
	buffer[0] = commandCode;
	buffer[1] = byteCount;

	memcpy(&buffer[2], data, byteCount);

	HAL_StatusTypeDef state = HAL_I2C_Master_Transmit(m_i2cController,
			m_slaveAddress, buffer, (uint16_t) (byteCount + 2U), I2C_TIMEOUT);
	return (state == HAL_OK);
}

bool CI2cDevice::receiveByte(uint8_t &data) {

	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, &data, 1,
	I2C_TIMEOUT);
	return (state == HAL_OK);
}

bool CI2cDevice::readByte(uint8_t commandCode, uint8_t &data) {

	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress,
			&commandCode, 1, I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, &data, 1,
	I2C_TIMEOUT);

	return (state == HAL_OK);
}

bool CI2cDevice::readWord(uint8_t commandCode, uint16_t &data) {
	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress,
			&commandCode, 1, I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	uint8_t buffer[2];
	state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 1,
	I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	memcpy(&data, buffer, sizeof(buffer));

	return true;
}

bool CI2cDevice::read32(uint8_t commandCode, uint32_t &data) {
	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress,
			&commandCode, 1, I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	uint8_t buffer[4];
	state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 2,
	I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	memcpy(&data, buffer, sizeof(buffer));

	return true;
}

bool CI2cDevice::read64(uint8_t commandCode, uint64_t &data) {
	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress,
			&commandCode, 1, I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	uint8_t buffer[8];
	state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 4,
	I2C_TIMEOUT);
	if (state != HAL_OK) {
		return false;
	}
	memcpy(&data, buffer, sizeof(buffer));
	return true;
}
//with repeated Start, HAL_I2C_Master_Sequential_Transmit_IT() should be used
//but this is just small project then HAL_I2C_Master_Transmit + Receive would be applied

bool CI2cDevice::blockRead(uint8_t commandCode, uint8_t *data,
		uint8_t &byteCount) {
	HAL_StatusTypeDef state;
	state = HAL_I2C_Master_Transmit(
	        m_i2cController, m_slaveAddress,
	        &commandCode, 1U, I2C_TIMEOUT);
	    if (state != HAL_OK) {
	        return false;
	    }
	 uint8_t buffer[256];
	 state = HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress,
			 buffer,sizeof(buffer),I2C_TIMEOUT);
	 if (state != HAL_OK) {
	 	        return false;
	 	    }
	 byteCounr = buffer[0];
	 memcpy(data,&buffer[1],byteCount);
	 return true;
}

bool CI2cDevice::processCall(uint8_t commandCode, uint16_t sendData,
		uint16_t &receiveData) {
	HAL_StatusTypeDef state;

		//step1 : send commandCode + sendData
	    uint8_t txBuffer[3];
	    txBuffer[0] = commandCode;
	    memcpy(&txBuffer[1], &sendData, sizeof(sendData));

	    state = HAL_I2C_Master_Transmit(
	        m_i2cController, m_slaveAddress,
	        txBuffer, 3U, I2C_TIMEOUT);
	    if (state != HAL_OK) {
	        return false;
	    }
	    //receive 2 byte
	    uint8_t rxBuffer[2];
	    state = HAL_I2C_Master_Receive(
	        m_i2cController, m_slaveAddress,
	        rxBuffer, 2U, I2C_TIMEOUT);
	    if (state != HAL_OK) {
	        return false;
	    }

	    memcpy(&receiveData, rxBuffer, sizeof(rxBuffer));
	    return true;
}

bool CI2cDevice::blockWriteBlockReadProcessCall(uint8_t commandCode,
		const uint8_t *sendData, uint8_t sendCount, uint8_t *receiveData,
		uint8_t &receiveCount) {

	return 0;
}

bool CI2cDevice::hostNotify(uint8_t deviceAddress, uint8_t dataLow,
		uint8_t dataHigh) {

	return 0;
}

bool CI2cDevice::writeRegister(uint8_t regAddress, uint16_t value){

	return 0;
}


bool CI2cDevice::readRegister(uint8_t regAddress, uint16_t &value){

	return 0;
}
