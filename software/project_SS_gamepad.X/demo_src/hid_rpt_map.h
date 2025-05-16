#ifndef HID_RPT_MAP_H
#define HID_RPT_MAP_H

#define HID_MAP_RPT_SIZE 64

const struct{uint8_t report[HID_MAP_RPT_SIZE];}hid_map_rpt={{ 
  0x06,0x00,0xFF,            // Usage Page (Vendor)
  0x09,0x01,                 // Usage 1
  0xA1,0x01,                 // Collection (Appl)
  0x85,0x01,                 //   Report ID 1
  0x75,0x08,0x95,0x40,       //   64 bytes
  0xB1,0x03,                 //   Feature (Data,Var,Abs)
  0xC0                       // End
}};

#endif // HID_RPT_MAP_H
