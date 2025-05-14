/*******************************************************************************
Copyright 2025 Custom USB Gamepad Project

Mapping functionality for button-to-usage configuration
*******************************************************************************/

#ifndef _MAPPING_H
#define _MAPPING_H

#include <stdint.h>

// Prototypes for flash read/write routines
void flash_read_block16(uint16_t *dest, uint16_t address, uint16_t count);
void flash_write_block16(uint16_t address, uint16_t *data);

#define NUM_BUTTONS 14

/**
 * Load mapping from High-Endurance Flash to RAM
 */
void Mapping_Load(void);

/**
 * Save mapping from RAM to High-Endurance Flash
 * @param tbl Pointer to button-to-usage mapping table (at least NUM_BUTTONS bytes)
 */
void Mapping_Save(const uint8_t *tbl);

/**
 * Get the usage value for a physical button
 * @param physBtn Physical button index (0-13)
 * @return Usage value (1-14)
 */
uint8_t Mapping_GetUsage(uint8_t physBtn);

#endif /* _MAPPING_H */
