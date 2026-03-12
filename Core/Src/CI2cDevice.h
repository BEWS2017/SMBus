#ifndef SRC_CI2CDEVICE_H_
#define SRC_CI2CDEVICE_H_
#include "stm32f4xx_hal.h"
#include <cstdint>
#include <ctype.h>
#include <cstring>
#include "i2c.h"

/**
 * Modelliert ein I2C/SMBus-Device (einen Slave) das an einen I2C-Controller
 * angeschlossen ist. Unterstuetzt alle standardisierten SMBus-Protokolle
 * gemaess SMBus Specification 3.x.
 */
class CI2cDevice {
protected:
	I2C_HandleTypeDef *m_i2cController;
	uint8_t m_slaveAddress;

public:
	/**
	 * Initialisiert ein neues Objekt mit den uebergebenen Werten.
	 *
	 * @param i2cController Verweis auf den Controller, an den das
	 * Device angeschlossen ist
	 * @param slaveAddress Adresse des Slave (zulaessiger Wert im Bereich
	 * 0 bis 127)
	 */
	CI2cDevice(I2C_HandleTypeDef *i2cController, uint8_t slaveAddress);

	// =========================================================================
	// SMBus Write-Protokolle
	// =========================================================================

	/**
	 * Quick Command: Sendet lediglich die Slave-Adresse mit dem R/W-Bit
	 * (0 = Write, 1 = Read), ohne weitere Daten. Wird oft zur Geraetepruefung
	 * oder zum Ausloesen einfacher Aktionen verwendet.
	 *
	 * @param readNotWrite true fuer Read-Bit, false fuer Write-Bit
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool quickCommand(bool readNotWrite);

	/**
	 * Send Byte: Sendet ein einzelnes Datenbyte ohne Command-Code
	 * an den Slave.
	 *
	 * @param data das zu sendende Byte
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool sendByte(uint8_t data);

	/**
	 * Write Byte: Fuehrt ein Kommando mit einem Byte als Argument aus.
	 * Typischerweise kann commandCode als die Adresse eines Registers
	 * interpretiert werden, in das der angegebene Wert geschrieben wird.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Zielregister
	 * @param data das zu schreibende Byte
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool writeByte(uint8_t commandCode, uint8_t data);

	/**
	 * Write Word: Schreibt ein 16-Bit-Wort (2 Byte, Little-Endian) in das
	 * durch commandCode adressierte Register des Slave.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Zielregister
	 * @param data das zu schreibende 16-Bit-Wort
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool writeWord(uint8_t commandCode, uint16_t data);

	/**
	 * Write 32: Schreibt einen 32-Bit-Wert (4 Byte, Little-Endian) in das
	 * durch commandCode adressierte Register des Slave.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Zielregister
	 * @param data der zu schreibende 32-Bit-Wert
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool write32(uint8_t commandCode, uint32_t data);

	/**
	 * Write 64: Schreibt einen 64-Bit-Wert (8 Byte, Little-Endian) in das
	 * durch commandCode adressierte Register des Slave.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Zielregister
	 * @param data der zu schreibende 64-Bit-Wert
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool write64(uint8_t commandCode, uint64_t data);

	/**
	 * Block Write: Schreibt einen Datenblock von bis zu 255 Byte in das
	 * durch commandCode adressierte Register des Slave. Das erste
	 * uebertragene Byte ist die Anzahl der folgenden Datenbytes (byteCount).
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Zielregister
	 * @param data Zeiger auf den zu schreibenden Datenblock
	 * @param byteCount Anzahl der zu schreibenden Bytes (zulaessig: 1-255)
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool blockWrite(uint8_t commandCode, const uint8_t *data,
			uint8_t byteCount);

	// =========================================================================
	// SMBus Read-Protokolle
	// =========================================================================

	/**
	 * Receive Byte: Empfaengt ein einzelnes Datenbyte ohne vorherigen
	 * Command-Code vom Slave (kein Repeated Start).
	 *
	 * @param data enthaelt nach erfolgreichem Aufruf das empfangene Byte
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool receiveByte(uint8_t &data);

	/**
	 * Read Byte: Sendet einen Command-Code und liest anschliessend ein Byte
	 * als Rueckgabewert vom Slave (mit Repeated Start). Typischerweise kann
	 * commandCode als Registeradresse interpretiert werden.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Quellregister
	 * @param data enthaelt nach erfolgreichem Aufruf das empfangene Byte
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool readByte(uint8_t commandCode, uint8_t &data);

	/**
	 * Read Word: Sendet einen Command-Code und liest anschliessend ein
	 * 16-Bit-Wort (2 Byte, Little-Endian) vom Slave (mit Repeated Start).
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Quellregister
	 * @param data enthaelt nach erfolgreichem Aufruf das empfangene 16-Bit-Wort
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool readWord(uint8_t commandCode, uint16_t &data);

	/**
	 * Read 32: Sendet einen Command-Code und liest anschliessend einen
	 * 32-Bit-Wert (4 Byte, Little-Endian) vom Slave (mit Repeated Start).
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Quellregister
	 * @param data enthaelt nach erfolgreichem Aufruf den empfangenen 32-Bit-Wert
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool read32(uint8_t commandCode, uint32_t &data);

	/**
	 * Read 64: Sendet einen Command-Code und liest anschliessend einen
	 * 64-Bit-Wert (8 Byte, Little-Endian) vom Slave (mit Repeated Start).
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Quellregister
	 * @param data enthaelt nach erfolgreichem Aufruf den empfangenen 64-Bit-Wert
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool read64(uint8_t commandCode, uint64_t &data);

	/**
	 * Block Read: Sendet einen Command-Code und liest anschliessend einen
	 * Datenblock vom Slave (mit Repeated Start). Das erste empfangene Byte
	 * gibt die Anzahl der folgenden Datenbytes an und wird in byteCount
	 * gespeichert.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion bzw.
	 * das Quellregister
	 * @param data Zeiger auf den Puffer, in den der empfangene Block
	 * geschrieben wird (muss mindestens 255 Byte gross sein)
	 * @param byteCount enthaelt nach erfolgreichem Aufruf die Anzahl
	 * der empfangenen Datenbytes
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool blockRead(uint8_t commandCode, uint8_t *data, uint8_t &byteCount);

	// =========================================================================
	// SMBus Kombinierte Protokolle
	// =========================================================================

	/**
	 * Process Call: Sendet einen Command-Code zusammen mit einem 16-Bit-Wort
	 * und empfaengt als Antwort ein 16-Bit-Wort vom Slave (kombinierter
	 * Write/Read mit Repeated Start). Dient typischerweise zur Berechnung
	 * eines Ergebnisses auf dem Slave.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion
	 * @param sendData das zu sendende 16-Bit-Wort als Eingabewert
	 * @param receiveData enthaelt nach erfolgreichem Aufruf das empfangene
	 * 16-Bit-Ergebnis
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool processCall(uint8_t commandCode, uint16_t sendData,
			uint16_t &receiveData);

	/**
	 * Block Write - Block Read Process Call: Sendet einen Datenblock und
	 * empfaengt als Antwort einen Datenblock vom Slave (kombinierter
	 * Block-Write/Block-Read mit Repeated Start). Das erste gesendete Byte
	 * ist die Anzahl der folgenden Sendedaten; das erste empfangene Byte
	 * ist die Anzahl der folgenden Empfangsdaten.
	 *
	 * @param commandCode identifiziert die auszufuehrende Aktion
	 * @param sendData Zeiger auf den zu sendenden Datenblock
	 * @param sendCount Anzahl der zu sendenden Bytes (zulaessig: 1-255)
	 * @param receiveData Zeiger auf den Puffer fuer die empfangenen Daten
	 * (muss mindestens 255 Byte gross sein)
	 * @param receiveCount enthaelt nach erfolgreichem Aufruf die Anzahl
	 * der empfangenen Datenbytes
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool blockWriteBlockReadProcessCall(uint8_t commandCode,
			const uint8_t *sendData, uint8_t sendCount, uint8_t *receiveData,
			uint8_t &receiveCount);

	// =========================================================================
	// SMBus Host-Protokoll
	// =========================================================================

	/**
	 * Host Notify: Wird vom Slave initiiert, um dem Host eine Benachrichtigung
	 * zu senden (Slave agiert temporaer als Master). Uebertraegt die eigene
	 * Geraeteadresse sowie zwei Datenbytes an die reservierte Host-Adresse
	 * (0x08).
	 * Hinweis: Diese Methode simuliert das Senden einer Host-Notify-Nachricht
	 * und ist primaer fuer Test- und Diagnosezwecke vorgesehen.
	 *
	 * @param deviceAddress Adresse des benachrichtigenden Slave-Devices
	 * @param dataLow niederwertiges Datenbyte der Benachrichtigung
	 * @param dataHigh hoeherwertiges Datenbyte der Benachrichtigung
	 * @return true, wenn die Aktion erfolgreich war
	 */
	bool hostNotify(uint8_t deviceAddress, uint8_t dataLow, uint8_t dataHigh);

	/**
	 * Schreibt in das Register mit der Adresse "regAddress"
	 * einen 16-Bit Wert (Big Endian).
	 * Gibt true zurück, wenn die Übertragung erfolgreich war,
	 * false sonst.
	 */
	bool writeRegister(uint8_t regAddress, uint16_t value);

	/**
	 * Liest aus dem Register mit der Adresse "regAddress"
	 * einen 16-Bit Wert (Big Endian).
	 * Schreibt den gelesenen Wert in "value".
	 * Gibt true zurück, wenn die Übertragung erfolgreich war,
	 * false sonst.
	 */
	bool readRegister(uint8_t regAddress, uint16_t &value);
};

#endif /* SRC_CI2CDEVICE_H_ */
