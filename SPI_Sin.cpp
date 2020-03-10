/***********************************
Date:Feb.1st,2020
Author: Gabriel Hall
Decription: Main Configures and writes to SPI3 of M487. 
Next Addition: Register Organization
***********************************/

#include "mbed.h" 
#include "spidegproj.h"

//Global variables
SPI dds(PA_8, PA_9, PA_10);//d11-mosi, d12-miso, d13-sclk
DigitalOut cs1(D12);//DDS1
DigitalOut trigger(D6);//Active LOW
DigitalOut led2(LED1);//Active LOW
DigitalIn  pb1(SW2);//Float HIGH

//Function Declarations
void configSPI();
void writeSPI(int32_t data);

/*struct settings{ //Future Coding
    
}

struct dds{
    
    
{*/

int main() {
    
    configSPI();
    while(true){
        if(pb1 == 0){
            led2=0;
            int32_t spi = (WRITE | SPICONFIG | 0x00000000);///////////
            int32_t power = (WRITE | POWERCONFIG | 0x00000E00);///////////
            int32_t dac4rset = (WRITE | DAC4RSET | 0x00001F00);////////
            int32_t dac3rset = (WRITE | DAC3RSET | 0x00001F00);////////
            int32_t dac2rset = (WRITE | DAC2RSET | 0x00001F00);/////////
            int32_t dac1rset = (WRITE | DAC1RSET | 0x00001F00);///////////
            int32_t calconfig = (WRITE | CALCONFIG | 0x00000002);
            int32_t pat_status = (WRITE | PAT_STATUS | 0x00000001);
            int32_t wav4_3config = (WRITE | WAV4_3CONFIG | 0x00000000);
            int32_t wav2_1config = (WRITE | WAV2_1CONFIG | 0x00003232);
            int32_t patperiod = (WRITE | PAT_PERIOD | 0x0000FFFF);
            int32_t dac4_3pat = (WRITE | DAC4_3PATx | 0x00000000);
            int32_t dac2_1pat = (WRITE | DAC2_1PATx | 0x00000101);// Changed from 1010
            int32_t dac1_gain = (WRITE | DAC1_DGAIN | 0x00004000);//changed from 2000
            int32_t saw4_3config = (WRITE | SAW4_3CONFIG | 0x00000000);
            int32_t saw2_1config = (WRITE | SAW2_1CONFIG | 0x00000200);
            int32_t dds_tw32 = (WRITE | DDS_TW32 | 0x00000024);
            int32_t dds_tw1 = (WRITE | DDS_TW1 | 0x00006900);
            int32_t dds_cyc4 = (WRITE | DDS_CYC4 | 0x00000000);
            int32_t dds_cyc3 = (WRITE | DDS_CYC3 | 0x00000000);
            int32_t dds_cyc2 = (WRITE | DDS_CYC2 | 0x00000000);
            int32_t start_dly1 = (WRITE | START_DLY1 | 0x00000FA0);
            int32_t dds_cyc1 = (WRITE | DDS_CYC1 | 0x00000100);
            int32_t ram_update = (WRITE | RAMUPDATE | 0x00000001);
            
            dds.lock();
            writeSPI(power);
            writeSPI(dac4rset);
            writeSPI(dac3rset);
            writeSPI(dac2rset);
            writeSPI(dac1rset);
            writeSPI(calconfig);
            writeSPI(pat_status);
            writeSPI(wav4_3config);
            writeSPI(wav2_1config);
            writeSPI(patperiod);
            writeSPI(dac4_3pat);
            writeSPI(dac2_1pat);
            writeSPI(dac1_gain);
            writeSPI(saw4_3config);
            writeSPI(saw2_1config);
            writeSPI(dds_tw32);
            writeSPI(dds_tw1);
            writeSPI(dds_cyc4);
            writeSPI(dds_cyc3);
            writeSPI(dds_cyc2);
            writeSPI(start_dly1);
            writeSPI(dds_cyc1);
            writeSPI(ram_update);
            /*cs1=0;
            int32_t write = dds.write(spi); 
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(power);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac4rset);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac3rset);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac2rset);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac1rset);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(calconfig);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(wav4_3config);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(wav2_1config);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(patperiod);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac4_3pat);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac2_1pat);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dac1_gain);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(saw4_3config);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(saw2_1config);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dds_tw32);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dds_tw1);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dds_cyc4);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dds_cyc3);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dds_cyc2);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(start_dly1);
            cs1=1;
            wait(0.00001);
            cs1=0;
            write = dds.write(dds_cyc1);
            cs1=1;*/
            dds.unlock();
            trigger=0;
            led2=1;
        }
    }
}

void configSPI(){
    
    int tx_length=32; 
    int mode=0;
    int freq=1000000; //1MHz
    
    cs1=1;
    trigger=1;
    dds.format(tx_length, mode);   
    dds.frequency(freq);   
    led2 = 1; 
}

void writeSPI(int32_t data){
    cs1=0;
    int32_t write = dds.write(data); 
    cs1=1;
    wait(0.00001);   
}

