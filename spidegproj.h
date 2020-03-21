//REGISTER ADDRESSES FOR AD9106
#ifndef DDS_REGISTERS
#define DDS_REGISTERS

#define WRITE           0x00000000//Write 0000000000000000
#define READ            0x80000000//read  1000000000000000

#define SPICONFIG       0x00000000//hex characters '00' added to the right for simplicity of bit wise anding
#define POWERCONFIG     0x00010000
#define CLOCKCONFIG     0x00020000
#define REFADJ          0x00030000
#define DAC4AGAIN       0x00040000
#define DAC3AGAIN       0x00050000
#define DAC2AGAIN       0x00060000
#define DAC1AGAIN       0x00070000
#define DACxRANGE       0x00080000
#define DAC4RSET        0x00090000
#define DAC3RSET        0x000A0000
#define DAC2RSET        0x000B0000
#define DAC1RSET        0x000C0000
#define CALCONFIG       0x000D0000
#define COMPOFFSET      0x000E0000
#define RAMUPDATE       0x001D0000
#define PAT_STATUS      0x001E0000
#define PAT_TYPE        0x001F0000
#define PATTERN_DLY     0x00200000
#define DAC4DOF         0x00220000
#define DAC3DOF         0x00230000
#define DAC2DOF         0x00240000
#define DAC1DOF         0x00250000
#define WAV4_3CONFIG    0x00260000
#define WAV2_1CONFIG    0x00270000
#define PAT_TIMEBASE    0x00280000
#define PAT_PERIOD      0x00290000
#define DAC4_3PATx      0x002A0000
#define DAC2_1PATx      0x002B0000
#define DOUT_START_DLY  0x002C0000
#define DOUT_CONFIG     0x002D0000
#define DAC4_CST        0x002E0000
#define DAC3_CST        0x002F0000
#define DAC2_CST        0x00300000
#define DAC1_CST        0x00310000
#define DAC4_DGAIN      0x00320000
#define DAC3_DGAIN      0x00330000
#define DAC2_DGAIN      0x00340000
#define DAC1_DGAIN      0x00350000
#define SAW4_3CONFIG    0x00360000
#define SAW2_1CONFIG    0x00370000
#define DDS_TW32        0x003E0000
#define DDS_TW1         0x003F0000
#define DDS4_PW         0x00400000
#define DDS3_PW         0x00410000
#define DDS2_PW         0x00420000
#define DDS1_PW         0x00430000
#define TRIG_TW_SEL     0x00440000
#define DDSx_CONFIG     0x00450000
#define TW_RAM_CONFIG   0x00470000
#define START_DLY4      0x00500000
#define START_ADDR4     0x00510000
#define STOP_ADDR4      0x00520000
#define DDS_CYC4        0x00530000
#define START_DLY3      0x00540000
#define START_ADDR3     0x00550000
#define STOP_ADDR3      0x00560000
#define DDS_CYC3        0x00570000
#define START_DLY2      0x00580000
#define START_ADDR2     0x00590000
#define STOP_ADDR2      0x005A0000
#define DDS_CYC2        0x005B0000
#define START_DLY1      0x005C0000
#define START_ADDR1     0x005D0000
#define STOP_ADDR1      0x005E0000
#define DDS_CYC1        0x005F0000
#define CFG_ERROR       0x00600000//Left two hex characters actual address
#define SRAM_DATA_START 0x60000000

#endif



