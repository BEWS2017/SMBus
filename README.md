This project implements all 16 SMBus protocols in C++ on STM32 (HAL),
including Quick Command, Send/Receive Byte, Write/Read Byte, Word, 32, 64,
Block Write/Read, Process Call, and Host Notify.

The implementation follows a layered architecture:
- write() and read() encapsulate the HAL I2C calls directly
- All SMBus protocol methods build on top of these two base functions
