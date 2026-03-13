/*
 * CI2cDevice.cpp
 *
 *  Created on: March 3, 2026
 *      Author: mnl
 */

#include "CI2cDevice.h"

#define I2C_TIMEOUT 100U

CI2cDevice::CI2cDevice(I2C_HandleTypeDef *i2cController, uint8_t slaveAddress) :
		m_i2cController(i2cController), m_slaveAddress(0U) {
	if (slaveAddress <= 127) {
		m_slaveAddress = slaveAddress << 1; //8 Bits for address
	} else {
		m_slaveAddress = 0U;
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
	return HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &data, 1,
	I2C_TIMEOUT);
}

bool CI2cDevice::writeByte(uint8_t commandCode, uint8_t data) {
	uint8_t buffer[2] = { commandCode, data };
	return HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 2,
	I2C_TIMEOUT);
}

bool CI2cDevice::writeWord(uint8_t commandCode, uint16_t data) {
	uint8_t buffer[3];
	//method 1: bit schieben.
	buffer[0] = commandCode;
	buffer[1] = (uint8_t) (data & 0xFFU);
	buffer[2] = (uint8_t) ((data >> 8U) & 0xFFU);

	return HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 3U,
	I2C_TIMEOUT);
}

bool CI2cDevice::write32(uint8_t commandCode, uint32_t data) {
	uint8_t buffer[5];
	buffer[0] = commandCode;
	//method 2: memcpy();
	//void *	 memcpy (void *__restrict, const void *__restrict, size_t);
	memcpy(&buffer[1], &data, sizeof(data));

	return HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 5U,
	I2C_TIMEOUT);
}

bool CI2cDevice::write64(uint8_t commandCode, uint64_t data) {
	uint8_t buffer[9];
	buffer[0] = commandCode;
	memcpy(&buffer[1], &data, sizeof(data));

	return HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer, 9U,
	I2C_TIMEOUT);
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
	return HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, buffer,
			(uint16_t) (byteCount + 2U), I2C_TIMEOUT);
}

bool CI2cDevice::receiveByte(uint8_t &data) {

	return HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, &data, 1,
	I2C_TIMEOUT);
}

bool CI2cDevice::readByte(uint8_t commandCode, uint8_t &data) {

	if (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &commandCode,
			1, I2C_TIMEOUT) != HAL_OK) {
		return false;
	}
	return (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, &data, 1,
	I2C_TIMEOUT));
}

bool CI2cDevice::readWord(uint8_t commandCode, uint16_t &data) {

	if (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &commandCode,
			1U, I2C_TIMEOUT) != HAL_OK) {
		return false;
	}
	uint8_t buffer[2] = { 0U, 0U };
	if (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 2U,
	I2C_TIMEOUT) != HAL_OK) {
		return false;
	}
	data = (uint16_t) ((uint16_t) buffer[0] | (uint16_t) buffer[1] << 8U);

	return true;
}

//with repeated Start, HAL_I2C_Master_Sequential_Transmit/Receive_IT()
// or HAL_I2C_Master_Sequential_Transmit/Receive_DMA() or HAL_I2C_Mem_Read/Write() should be used
//but this is just small project then HAL_I2C_Master_Transmit + Receive would be applied

bool CI2cDevice::read32(uint8_t commandCode, uint32_t &data) {

	if (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &commandCode,
			1U, I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	uint8_t buffer[4] = { 0U };
	if (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 4U,
	I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	const uint32_t b0 = (uint32_t) buffer[0];
	const uint32_t b1 = (uint32_t) buffer[1];
	const uint32_t b2 = (uint32_t) buffer[2];
	const uint32_t b3 = (uint32_t) buffer[3];

	data = (uint32_t) (b0 | (b1 << 8U) | (b2 << 16U) | (b3 << 24U));

	return true;
}

bool CI2cDevice::read64(uint8_t commandCode, uint64_t &data) {

	if (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &commandCode,
			1U, I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	uint8_t buffer[8] = { 0U };
	if (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 8U,
	I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	// Cast tat ca sang uint64_t truoc khi shift
	const uint64_t b0 = (uint64_t) buffer[0];
	const uint64_t b1 = (uint64_t) buffer[1];
	const uint64_t b2 = (uint64_t) buffer[2];
	const uint64_t b3 = (uint64_t) buffer[3];
	const uint64_t b4 = (uint64_t) buffer[4];
	const uint64_t b5 = (uint64_t) buffer[5];
	const uint64_t b6 = (uint64_t) buffer[6];
	const uint64_t b7 = (uint64_t) buffer[7];

	data = (uint64_t) (b0 | (b1 << 8U) | (b2 << 16U) | (b3 << 24U) | (b4 << 32U)
			| (b5 << 40U) | (b6 << 48U) | (b7 << 56U));

	//			other method:
	//			for (uint8_t i = 0U; i < 8U; i++) {
	//				data |= ((uint64_t) buffer[i] << (i * 8U));
	//			}

	return true;
}

//HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
//HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
bool CI2cDevice::blockRead(uint8_t commandCode, uint8_t *data,
		uint8_t &byteCount) {
	uint8_t buffer[256] = { 0U };
	if (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, &commandCode,
			1U, I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	if (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, buffer, 256U,
	I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	byteCount = buffer[0U];
	if (byteCount == 0U) {
		return false;
	}

	for (uint8_t i = 0U; i < byteCount; i++) {
		data[i] = buffer[i + 1U];
	}
	return 0;
}

bool CI2cDevice::processCall(uint8_t commandCode, uint16_t sendData,
		uint16_t &receiveData) {

	uint8_t txBuffer[3] = { 0U };
	txBuffer[0] = commandCode;
	txBuffer[1] = (uint8_t) sendData & 0xFF;
	txBuffer[2] = (uint8_t) (sendData >> 8U);

	if (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, txBuffer, 3U,
	I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	uint8_t rxBuffer[2] = { 0U };
	if (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, rxBuffer, 2U,
	I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	receiveData = (uint16_t) (((uint16_t) (rxBuffer[0]))
			| (((uint16_t) rxBuffer[1]) << 8U));

	return true;
}

bool CI2cDevice::blockWriteBlockReadProcessCall(uint8_t commandCode,
		const uint8_t *sendData, uint8_t sendCount, uint8_t *receiveData,
		uint8_t &receiveCount) {

	return 0;
}

bool CI2cDevice::hostNotify(uint8_t deviceAddress, uint8_t dataLow,
		uint8_t dataHigh) {
	const uint8_t HOST_ADDRESS = 0x08U; // SMBus Host Address (fixed)
	uint8_t txBuffer[3] = { 0U };
	txBuffer[0] = deviceAddress;
	txBuffer[1] = dataLow;
	txBuffer[2] = dataHigh;
	if (HAL_I2C_Master_Transmit(m_i2cController, HOST_ADDRESS, txBuffer, 3U,
			I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	return true;
}

//These two methods could be applied in above methods
bool CI2cDevice::write(uint8_t *data, uint16_t length) {
	if (data == nullptr) {
		return false;
	}
	return (HAL_I2C_Master_Transmit(m_i2cController, m_slaveAddress, data,
			length, I2C_TIMEOUT) == HAL_OK);
}

bool CI2cDevice::read(uint8_t *data, uint16_t length) {
	if (data == nullptr) {
		return false;
	}
	return (HAL_I2C_Master_Receive(m_i2cController, m_slaveAddress, data,
			length, I2C_TIMEOUT) == HAL_OK);
}

bool CI2cDevice::writeRegister(uint8_t regAddress, uint16_t value) {
	uint8_t buffer[3] = {0U};
	buffer[0] = regAddress;
	buffer[1] = (uint8_t)(value >> 8U);
	buffer[2] = (uint8_t)(value & 0xFF);
	return write(buffer,3U);
}

bool CI2cDevice::readRegister(uint8_t regAddress, uint16_t &value) {

	//first, send regAddress through write()
	if(!write(&regAddress,1U)){
		return false;
	}

	//then, read 2 byte through read()
	uint8_t rxBuffer[2] = {0U};
	if(!read(rxBuffer,2U)){
		return false;
	}
	//finally, big-endian
	value = (uint16_t)(((uint16_t)rxBuffer[1]) << 8U | ((uint16_t)rxBuffer[0])); //Big Endian: Big first then small

	return true;
}
