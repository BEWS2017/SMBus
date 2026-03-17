/*
 * CINA226.h
 *
 *  Created on: Feb 12, 2026
 *      Author: Nam Ha
 */
#ifndef CINA226_H
#define CINA226_H

#include <string.h>
#include <stdint.h>
#include "CI2cDevice.h"

/**
 * Treiber für den INA226 Strom- und Leistungsmonitor.
 *
 * Register-Adressen (aus dem Datenblatt S.22, Tabelle 7-1):
 *   Configuration Register : 0x00
 *   Shunt Voltage Register  : 0x01
 *   Bus Voltage Register    : 0x02
 *   Power Register          : 0x03
 *   Current Register        : 0x04
 *   Calibration Register    : 0x05
 *   Manufacturer ID Register: 0xFE
 *
 * LSB-Werte (aus dem Datenblatt S.5, Tabelle 5.5):
 *   Shunt Voltage LSB : 2.5  µV
 *   Bus Voltage LSB   : 1.25 mV
 *   Power LSB         : 25 × Current_LSB
 */
class CINA226 : public CI2cDevice {
private:
    float m_currentLsb; // Einheit: A/Bit
    bool  m_failed;     // true, wenn eine Operation fehlgeschlagen ist

    /**
     * Schreibt den berechneten Kalibrierungswert in das
     * Calibration Register des INA226.
     * Formel (Datenblatt S.15, Gleichung 1):
     *   CAL = 0.00512 / (currentLsb × rShunt)
     *
     * "currentLsb" ist der gewünschte LSB-Wert in A/Bit.
     * "rShunt"     ist der Shunt-Widerstand in Ohm.
     */
    void calibrate(float currentLsb, float rShunt);

public:
    /**
     * Erzeugt ein neues CINA226-Objekt und konfiguriert den Baustein.
     *
     * "hi2c"          zeigt auf den konfigurierten I2C-Controller.
     * "deviceAddress" ist die 7-Bit Slave-Adresse des INA226.
     * "rShunt"        ist der verwendete Shunt-Widerstand in Ohm.
     * "maxCurrent"    ist der maximale erwartete Strom in Ampere.
     *
     * Im Konstruktor soll:
     * 1. Das Configuration Register mit dem Wert 0x4127
     *    (POR-Wert: Continuous, Shunt+Bus, AVG=1, tCT=1.1ms)
     *    beschrieben werden.
     * 2. Die Kalibrierung mit dem berechneten Current_LSB
     *    und dem Shunt-Widerstand durchgeführt werden.
     *    Current_LSB = maxCurrent / 32768
     */
    CINA226(I2C_HandleTypeDef* hi2c,
            uint8_t deviceAddress,
            float rShunt,
            float maxCurrent);

    /**
     * Gibt an, ob eine der vorhergehenden Operationen
     * fehlgeschlagen ist.
     * Setzt das Flag zurück.
     * @return true wenn ein Fehler aufgetreten ist, false sonst
     */
    bool hasFailed();

    /**
     * Liest die Shunt-Spannung und gibt sie in Volt zurück.
     * Hinweis: Der Wert im Register ist vorzeichenbehaftet
     * (Two's Complement). Verwenden Sie int16_t.
     * LSB = 2.5 µV = 0.0000025 V
     */
    float readShuntVoltage();

    /**
     * Liest die Bus-Spannung und gibt sie in Volt zurück.
     * LSB = 1.25 mV = 0.00125 V
     */
    float readBusVoltage();

    /**
     * Liest den Strom und gibt ihn in Ampere zurück.
     * Hinweis: Der Wert im Register ist vorzeichenbehaftet.
     * Strom [A] = Registerwert × m_currentLsb
     */
    float readCurrent();

    /**
     * Liest die Leistung und gibt sie in Watt zurück.
     * Power LSB = 25 × m_currentLsb
     */
    float readPower();

    /**
     * Prüft, ob der angeschlossene Baustein wirklich ein INA226
     * ist, indem das Manufacturer ID Register (Adresse 0xFE)
     * ausgelesen und mit dem erwarteten Wert 0x5449 verglichen wird.
     * Gibt true zurück, wenn die ID übereinstimmt, false sonst.
     */
    bool isConnected();
};

#endif
