/*
 * CINA226.cpp
 *
 *  Created on: Feb 12, 2026
 *      Author: Nam Ha
 */

#include "CINA226.h"

// Page 21 and 22 : Register maps
#define INA226_SHUNT_VOLT_REG	(0x01U)
#define INA226_CONFIG_REG	(0x00U)
#define INA226_BUS_VOLT_REG	(0x02U)
#define INA226_POW_REG	(0x03U)
#define INA226_CURR_REG	(0x04U)
#define INA226_CALI_REG	(0x05U)
#define INA226_M_ID_REG	(0xFEU)

CINA226::CINA226(I2C_HandleTypeDef* hi2c,
		uint8_t deviceAddress,
		float rShunt,
		float maxCurrent) :
		CI2cDevice(hi2c, deviceAddress){
	//if(rShunt <= 0.0f || maxCurrent <= 0.0f){
	// error warning
	// return;
	//}
	m_failed = false;
	m_currentLsb = maxCurrent / 32768.0f;

	if(!writeRegister(INA226_CONFIG_REG,0x4127)){
		m_failed = true;
	}
	calibrate(maxCurrent/32768.f,rShunt);
}

// attention: tolerant factor
// if CAL = 512.78 then only 512 would be saved into register
// and 0.78 would be lost
// to avoid this problem: use round up or just add 0.5f.
void CINA226::calibrate(float currentLsb, float rShunt){

	//same here if currentLsb and rShunt = 0 then error warning
	uint16_t CAL = static_cast<uint16_t>((0.00512f / (currentLsb * rShunt)) + 0.5f);
	if(!writeRegister(INA226_CALI_REG,CAL)){
		m_failed = true;
	}
}

//set fail-flag back
//return true when m_failed = true and vice versa
bool CINA226::hasFailed(){
	bool tmp = m_failed;
	m_failed = false;
	return tmp;
}

bool CINA226::isConnected(){
	uint16_t value = 0U;
	const uint16_t iD = 0x5449U;
	if(readRegister(INA226_M_ID_REG,value)){
		if(value == iD){
			return true;
		}
		m_failed = true; //if value != iD
	}else{
		m_failed = true;
	}
	return false;
}

float CINA226::readBusVoltage(){
	uint16_t busVoltage = 0U;
	float result = 0.0f;
	const float busVoltFactor = 0.00125f;
	if(readRegister(INA226_BUS_VOLT_REG,busVoltage)){
		result = static_cast<float>(busVoltage) * busVoltFactor;
	}else{
		m_failed = true;
	}
	return result;
}

float CINA226::readShuntVoltage(){
	int16_t signedVolt = 0;
	uint16_t rawVolt = 0U;
	float result = 0.0f;
	const float shuntVoltFactor = 0.0000025f;
	if(readRegister(INA226_SHUNT_VOLT_REG,rawVolt)){
		memcpy(&signedVolt,&rawVolt,sizeof(rawVolt));
		result = static_cast<float>(signedVolt) * shuntVoltFactor;
	}else{
		m_failed = true;
	}

	return result;
}

float CINA226::readCurrent(){
	uint16_t unsignedValue = 0U; //unsigned value from register
	int16_t signedValue = 0; //signed value;
	float result = 0.0f;
	if(readRegister(INA226_CURR_REG,unsignedValue)){
		memcpy(&signedValue,&unsignedValue,sizeof(unsignedValue)); //copy from unsignedValue to signedValue. Looking for another way that not using memcpy
		//another way:
		// Reinterpret two's complement bit pattern as signed.
		// static_cast<int16_t> is implementation-defined for values > INT16_MAX,
		// but all ARM embedded compilers (GCC, ARMCC, IAR) guarantee correct
		// two's complement conversion. Verified: sizeof(int16_t) == sizeof(uint16_t).
		//signedValue = static_cast<int16_t>(unsignedValue);

		result = static_cast<float>(signedValue) * m_currentLsb;
	}else{
		m_failed = true;
	}
	return result;
}

float CINA226::readPower(){
	uint16_t value = 0U; //value from register
	float result = 0.0f;
	float const powerFactor = 25.0f;
	if(readRegister(INA226_POW_REG,value)){
		//Page 24: Power LSB is programmed equal to 25 * current LSB
		result = static_cast<float>(value) * (powerFactor * m_currentLsb);
	}else{
		m_failed = true;
	}
	return result;
}
