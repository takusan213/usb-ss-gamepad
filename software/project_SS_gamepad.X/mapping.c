/*******************************************************************************
Copyright 2025 Custom USB Gamepad Project

Mapping functionality for button-to-usage configuration
*******************************************************************************/

// Flash read/write routines are declared in mapping.h
#include "mapping.h"
#include <xc.h>
#include <string.h>
// NVM ドライバを使う
#include "mcc_generated_files/nvm/nvm.h"
#include "demo_src/hid_rpt_map.h"

/* RAM working copy of the mapping data */
static struct {
    uint8_t ver;               // Version for compatibility checking
    uint8_t tbl[NUM_BUTTONS];  // Button-to-usage mapping table
    uint8_t crc;               // CRC8 checksum for data integrity
} map ;            

#define MAP_VER 0x01           // Current data structure version
#define HEF_ADDR 0x1F80        // High-Endurance Flash starting address (row0)

#define ROW_WORDS   32                  // 64B / 2B
static flash_data_t rowBuf[ROW_WORDS];  // uint16_t[32]

/**
 * Calculate CRC8 checksum (0x07 polynomial)
 * @param d Pointer to data
 * @param l Data length in bytes
 * @return CRC8 checksum
 */
static uint8_t crc8(const uint8_t *d, uint8_t l) {
    uint8_t c = 0;
    while (l--) {
        c ^= *d++;
        for (uint8_t i = 0; i < 8; i++) {
            c = (uint8_t)((c & 0x80) ? ((c << 1) ^ 0x07) : (c << 1));
        }
    }
    return c;
}

void map_to_rowbuf(void)
{
    /* 0x3FFF で初期化（未使用上位バイトは 0x3F） */
    for (uint8_t i = 0; i < ROW_WORDS; i++) rowBuf[i] = 0x3FFF;

    /* uint8_t map 構造体をuint16_t rowBufにコピー */
    for (uint8_t b = 0; b < sizeof(map); b++) {
        rowBuf[b] = 0x3F00 | ((uint8_t*) &map)[b]; 
    }
}

/**
 * Load mapping from High-Endurance Flash to RAM
 * If invalid data detected, initialize with default mapping
 */
void Mapping_Load(void) {
    // Read mapping data from flash to RAM via FLASH_Read
    for(uint8_t i = 0; i < sizeof(map); i ++){
        // Read 14-bit words from flash and convert to 8-bit
        flash_data_t data = FLASH_Read(HEF_ADDR + i);
        ((uint8_t*)&map)[i] = (uint8_t)(data & 0x00FF); // Use lower byte
    }
    
    // Validate data (version and CRC)
    if (map.ver != MAP_VER || map.crc != crc8((uint8_t*)&map, sizeof(map) - 1)) {
        // Invalid data, initialize with standardized default mapping
        // A, B, C buttons are 1,2,3
        map.tbl[0] = 1;  // A -> Button 1
        map.tbl[1] = 2;  // B -> Button 2
        map.tbl[2] = 3;  // C -> Button 3
        // X, Y, Z buttons are 4,5,6
        map.tbl[3] = 4;  // X -> Button 4
        map.tbl[4] = 5;  // Y -> Button 5
        map.tbl[5] = 6;  // Z -> Button 6
        // Shoulder buttons are 7,8
        map.tbl[6] = 7;  // L1 -> Button 7
        map.tbl[7] = 8;  // R1 -> Button 8
        // L2, R2, Left stick, Right stick, Home
        map.tbl[8] = 10;  // L2 -> Button 10
        map.tbl[9] = 11;  // R2 -> Button 11
        map.tbl[10] = 14; // Left stick -> Button 14
        map.tbl[11] = 13; // Right stick -> Button 13
        map.tbl[12] = 12; // Home -> Button 12
        // The last button could be spare/select
        map.tbl[13] = 9;  // Select/spare -> Button 9
    }
}

/**
 * Save mapping from RAM to High-Endurance Flash
 * @param tbl Pointer to button-to-usage mapping table
 */
void Mapping_Save(const uint8_t *tbl) {
    // Copy new mapping table to RAM structure
    memcpy(map.tbl, tbl, NUM_BUTTONS);
    
    // Update version and CRC
    map.ver = MAP_VER;
    map.crc = crc8((uint8_t*)&map, sizeof(map) - 1);
    
    // Convert map structure to row buffer for flash write
    map_to_rowbuf();

    // Save to flash via FLASH_RowWrite
    uint8_t gie = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    NVM_UnlockKeySet(UNLOCK_KEY);
    FLASH_PageErase(HEF_ADDR);  // Erase the page before writing
    while(NVM_IsBusy());  // Wait for erase to complete
    FLASH_RowWrite(HEF_ADDR, rowBuf);    // Write the row buffer to flash
    while(NVM_IsBusy());     
    NVM_UnlockKeyClear();
    INTCONbits.GIE = gie;
}

/**
 * Get the usage value for a physical button
 * @param physBtn Physical button index
 * @return Usage value
 */
uint8_t Mapping_GetUsage(uint8_t physBtn) {
    return map.tbl[physBtn];
}

/**
 * Copy mapping data from Feature Report buffer to the mapping table
 * @param featureReport The feature report buffer received from the host
 * @param length Length of the feature report data
 */
void Mapping_SetFromFeatureReport(uint8_t* featureReport, uint16_t length) {
    // Report ID may or may not be in the buffer depending on the implementation
    // For safety, assume data starts at index 0
    uint8_t dataOffset = 1;
    
    // // Check if the first byte is Report ID 1
    // if (length > 0 && featureReport[0] == 0x01) {
    //     // Skip Report ID byte
    //     dataOffset = 1;
    // }
    
    // Determine how many mapping bytes we can copy
    uint8_t maxBytes = (length - dataOffset > NUM_BUTTONS) ? NUM_BUTTONS : length - dataOffset;
    uint8_t newMapping[NUM_BUTTONS];
    
    // Copy data from feature report
    for (uint8_t i = 0; i < maxBytes; i++) {
        newMapping[i] = featureReport[i + dataOffset];
    }
    
    // For any remaining buttons, use existing mapping
    for (uint8_t i = maxBytes; i < NUM_BUTTONS; i++) {
        newMapping[i] = map.tbl[i];
    }
    
    // Save the new mapping to flash
    Mapping_Save(newMapping);

    Mapping_Load(); //for debugging, reload mapping after saving
}

/**
 * Copy mapping data from the mapping table to the Feature Report buffer
 * @param featureReport The feature report buffer to be sent to the host
 */
void Mapping_GetAsFeatureReport(uint8_t* featureReport) {
    // Set Report ID as first byte
    featureReport[0] = 0x00;  // Report ID 0

    // Copy mapping table to feature report (starting at index 1)
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        featureReport[i + 1] = map.tbl[i];
    }
    
    // Fill remaining bytes with zeros
    for (uint8_t i = NUM_BUTTONS + 1; i < HID_MAP_EP_BUF_SIZE; i++) {
        featureReport[i] = 0;
    }
}
