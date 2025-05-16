/*******************************************************************************
Copyright 2025 Custom USB Gamepad Project

Mapping functionality for button-to-usage configuration
*******************************************************************************/

#ifndef _MAPPING_H
#define _MAPPING_H

#include <stdint.h>

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

/**
 * Copy mapping data from Feature Report buffer to the mapping table
 * @param featureReport The feature report buffer received from the host
 * @param length Length of the feature report data
 */
void Mapping_SetFromFeatureReport(uint8_t* featureReport, uint16_t length);

/**
 * Copy mapping data from the mapping table to the Feature Report buffer
 * @param featureReport The feature report buffer to be sent to the host
 */
void Mapping_GetAsFeatureReport(uint8_t* featureReport);

#endif /* _MAPPING_H */
