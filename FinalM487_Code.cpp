/***********************************
Date:Mar.18th,2020
Author: Gabriel Hall
Decription: Starting case statements to write register values to AD9106 based on user input
Next Addition: Phase 
***********************************/

#include "mbed.h" 
#include "spidegproj.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#define NO_ACTION   0
#define SINE        1
#define BSIN        2
#define TRIGON      3
#define TRIGOFF     4

#define MAX_CHAN    4// Set as many as youd like

//****************M487 pinout declaration*****************************
Serial pc(USBTX, USBRX); // tx, rx
SPI dds(PA_8, PA_9, PA_10);//d11-mosi, d12-miso, d13-sclk
DigitalOut cs1(D12);//DDS1, can make an array of CS if multiple dds'
DigitalOut trigger_ad(D6);//Active LOW
DigitalOut led2(LED1);//Active LOW
DigitalOut led1(LED2);//Active LOW
DigitalIn  pb1(SW2);//Float HIGH

//************Function Declarations***************
void configSPImcu();
void writeSPI(int32_t data);
void configDDSRegisters();
void frequency_calc();
void dgain_calc();
void phase_calc();
void delay_calc();
void trigger_ON();
void trigger_OFF();
void sine_DDS();
void bsine_DDS();
void set_cycles();
void set_Bcycles();
void pattern_Period();
void fillStruct();//kossaian
void parString();//kossain

//************Global variables*************************************
char *chan[9];
int i=0;
int32_t wave_config[MAX_CHAN];//gh
int32_t pat_period[MAX_CHAN];//Use for burst pattern repeat
char str[128];
int ack = 1;

struct Chan{
    int Cnfg; //(noaction:0,sine:1, bsin:2, trigON:3, trigOFF:4)
    int Chan; //(0-3)
    float Ampl; //pu
    int Freq;// 
    float Phas;//Phase 
    float Dlay;//delay
    float BPer;//Burst period
    int Cycl;//Cycles pattern repeats max 256
    float PatP;//Whole pattern period max 5.8ish mS at fclk=180MHz
} string1;



int main() {
    
    configSPImcu();//Configure system
    configDDSRegisters();
      
    while(true){
        
        while(pc.readable()){ 
        led2 = 0;         
            //pc.printf("%s\n\r", str);
            scanf("%s", str);
            parString();
            
            str[0] = '\0';
            if(ack >=4){ ack = 0;}
            pc.printf("%i", ack);
            ack++;
        }
        led2 = 1;//Troubleshooting
        
        switch (string1.Cnfg) {// CONGIFURE CONFIG TO 0 TO SHOW MESSAGE HAS BEEN PROCESSED

            case SINE://1
                led1=0;
                sine_DDS();
                break;

            case BSIN://2
                bsine_DDS();
                break;

            case TRIGON://3
                trigger_ON();
                break;

            case TRIGOFF://4
                trigger_OFF();
                break;

            case NO_ACTION://0
                break;
        }//switch cnfg        
    }//while
}//main

/*******************************************************
Function: configSPImcu() 
Description: Configures SPI protocol used for the M487,
             sets chip select and trigger to high
             (Active lows)
Author: Gabriel Hall
********************************************************/
void configSPImcu(){
    
    int tx_length=32; 
    int mode=0;
    int freq=1000000; //1MHz
    
    cs1=1;
    trigger_ad=1;
    dds.format(tx_length, mode);   
    dds.frequency(freq);    
}

/*******************************************************
Function: writeSPI()
Description: write function being passed in generic data
             to be sent to ad9106
Author: Gabriel Hall
********************************************************/
void writeSPI(int32_t data){
    cs1=0;
    int32_t write = dds.write(data); 
    cs1=1;
    wait(0.00001);   
}

/*******************************************************
Function: configDDSRegisters()
Description: configures power, spi, calibraiton, and 
             pat_status for run ability from trigger
Author: Gabriel Hall
********************************************************/
void configDDSRegisters(){
    
    int32_t spi = (WRITE | SPICONFIG | 0x00002004);//Reset AD9106  
    dds.lock();
    writeSPI(spi);
    dds.unlock();
    spi = (WRITE | SPICONFIG | 0x00000000);
    int32_t power = (WRITE | POWERCONFIG | 0x00000E00);
    int32_t pat_status = (WRITE | PAT_STATUS | 0x00000001);
    int32_t calconfig = (WRITE | CALCONFIG | 0x00000002);
    dds.lock();
    writeSPI(spi);
    writeSPI(power);
    writeSPI(calconfig);
    writeSPI(pat_status);
    dds.unlock();
    led1 = 1;
}

/*******************************************************
Function: fillStruct()
Description: fills struct string1 with data
Author: Alexander Kossaian 
********************************************************/
void fillStruct(){
    int t = 0;

string1.Chan = atoi(chan[0]);
        //pc.printf("Channel: %i\n", string1.Chan);
    
while(t < 9){
        
    char *var;
    char *val = chan[t];
        
    var = strtok_r(val, ":", &val);
    
        if(strcmp("Cnfg", var)==0){
            string1.Cnfg = atoi(val);
            //pc.printf("Config has: %i in it\n", string1.Cnfg);
            }
        if(strcmp("Ampl", var)==0){
            string1.Ampl = atof(val);
            //pc.printf("Amplitude has: %f in it\n", string1.Ampl);
            }
        if(strcmp("Freq", var)==0){
            string1.Freq = atoi(val);
            //pc.printf("Freq has: %i in it\n", string1.Freq);
            }
        if(strcmp("Phas", var)==0){
            string1.Phas = atof(val);
            //pc.printf("Phase has: %i in it\n", string1.Phas);
            }
        if(strcmp("Dlay", var)==0){
            string1.Dlay = atof(val);
            //pc.printf("Delay has: %f in it\n", string1.Dlay);
            }
        if(strcmp("BPer", var)==0){
            string1.BPer = atof(val);
            //pc.printf("Offset has: %f in it\n", string1.BPer);
            }
        if(strcmp("PatP", var)==0){
            string1.PatP = atof(val);
            //printf("Patern period has: %f in it\n", string1.PatP);
            }
        if(strcmp("Cycl", var)==0){
            string1.Cycl = atoi(val);
            //printf("Number of cycles has: %i in it\n", string1.Cycl);
            }
        t++;
    }
}
/*******************************************************
Function: parString()
Description: Parses incoming string
Author: Alexander Kossaian 
********************************************************/   
void parString(){
    char *token = strtok(str, ":");
    
    while (token != NULL)
    {
        token = strtok(NULL, ",");
        chan[i] = token;
        i++;
        if(i==9){i=0;break;}
    }
    fillStruct();
}

/*******************************************************
Function: frequency_calc()
Description: calculates tuning words needed for wanted frequency
Author: Gabriel Hall
********************************************************/
void frequency_calc(){

    float fref = 180000000; // max fout 5 MHz
    float exponential = 16777216;//2^24
    //printf("Fref: %X , %f \n", fref, fref);
    
    float freq= (float)string1.Freq;
    
    //printf("Freq: %X , %f \n", Freq, Freq);
    
    float tuning_word = (freq/fref)*exponential;
    
    //printf("Freq: %X , %f \n", tuning_word, tuning_word);
    
    int32_t tw = (int32_t)tuning_word;
    
    //printf("tw: %X , %d \n", tw, tw);
    
    int32_t msb = (tw & 0x00FFFF00)>>8;//capture bits [23:8] then shift them eight to the right
    int32_t lsb = (tw & 0x000000FF);
    
    int32_t msb_tw = (WRITE | DDS_TW32 | msb);
    int32_t lsb_tw = (WRITE | DDS_TW1 | lsb);
    //printf("msb: %X / lsb: %X", msb_tw, lsb_tw);
    
    dds.lock();
    writeSPI(msb_tw);
    writeSPI(lsb_tw);
    dds.unlock();     
}

/*******************************************************
Function: dgain_calc()
Description: calculates digital gain register (0 -> 1 PU)
Author: Gabriel Hall
********************************************************/
void dgain_calc(){
        
    float dgain = string1.Ampl*1024;//Normalizing gain from 0 to 1
    int32_t gain = (int32_t)dgain;
    gain = gain << 4;
    int32_t gain_word = 0; 
    switch (string1.Chan){
    
        case 0:
            gain_word = (WRITE | DAC1_DGAIN | gain);
            break;
        case 1:
            gain_word = (WRITE | DAC2_DGAIN | gain);
            break;
        case 2:
            gain_word = (WRITE | DAC3_DGAIN | gain);
            break;
        case 3:
            gain_word = (WRITE | DAC4_DGAIN | gain);
            break;
        } 
    dds.lock();
    writeSPI(gain_word);
    dds.unlock();  
}

/*******************************************************
Function: phase_calc()
Description: Calculates phase shift for channels
Author: Gabriel Hall
********************************************************/
void phase_calc(){
    
    int32_t ph_w = 0;
    float max = 1/(string1.Freq);
    float cyc = 180000000*max;//number of cycles of clock inside freq
    if(string1.Phas > 359){
        string1.Phas = 0;
        }
    float factor = cyc/360;    
    float phase = string1.Phas*factor; 
    ph_w = (int32_t)phase;
       
    int32_t phase_word = 0;
    
    switch (string1.Chan){
    
        case 0:
            phase_word = (WRITE | START_DLY1 | ph_w);//Switch to DDSN_PW addresses if you get phase working
            break;
        case 1:
            phase_word = (WRITE | START_DLY2 | ph_w);
            break;
        case 2:
            phase_word = (WRITE | START_DLY3 | ph_w);
            break;
        case 3:
            phase_word = (WRITE | START_DLY4 | ph_w);
            break;
        } 
    dds.lock();
    writeSPI(phase_word);
    dds.unlock();  
 }

/*******************************************************
Function: delay_calc()
Description: calculates delay for each channel
Author: Gabriel Hall
********************************************************/ 
void delay_calc(){
    
    float delay = (string1.Dlay/string1.PatP)*65536;
    int32_t del= (int32_t)delay;
    int32_t delay_word = 0;
    switch (string1.Chan){
    
        case 0:
            delay_word = (WRITE | START_DLY1 | del);
            break;
        case 1:
            delay_word = (WRITE | START_DLY2 | del);
            break;
        case 2:
            delay_word = (WRITE | START_DLY3 | del);
            break;
        case 3:
            delay_word = (WRITE | START_DLY4 | del);
            break;
        }    
    dds.lock();
    writeSPI(delay_word);
    dds.unlock();
 }

/***************************************************************************************************************
Function: trigger_ON()
Description: configures overall wave configuration register
             and updates shadow registers in AD9106 to active
             registers. Drives trigger low after to activate
             AD9106
Author: Gabriel Hall
***************************************************************************************************************/
void trigger_ON(){
    
    int32_t wav2_1config = (WRITE | WAV2_1CONFIG | 0x00003232); 
    int32_t wav4_3config = (WRITE | WAV4_3CONFIG | 0x00003232); 
    int32_t ram_update = (WRITE | RAMUPDATE | 0x00000001);
    int32_t ram_update2 = (WRITE | RAMUPDATE | 0x00000000);
    
    int32_t pat43 = (pat_period[3] | pat_period[2]);
    int32_t pat21 = (pat_period[1] | pat_period[0]);
    
    int32_t dac43pat = (WRITE | DAC4_3PATx | pat43);
    int32_t dac21pat = (WRITE | DAC2_1PATx | pat21);
    
    
    dds.lock();
    writeSPI(wav2_1config);
    writeSPI(wav4_3config);
    writeSPI(dac43pat);
    writeSPI(dac21pat);
    writeSPI(ram_update);
    wait(0.00001);
    writeSPI(ram_update2);
    dds.unlock();
    trigger_ad = 0;    
}

/*******************************************************
Function: trigger_OFF()
Description: turns AD9106 off
Author: Gabriel Hall
********************************************************/
void trigger_OFF(){

    int32_t ram_update = (WRITE | RAMUPDATE | 0x00000000);
    dds.lock();
    writeSPI(ram_update);
    dds.unlock();
    trigger_ad = 1;        
}

/*******************************************************
Function: sine_wave_config()
Description: configures continuous sine waves and pattern
             period.
Author: Gabriel Hall
********************************************************/
void sine_wave_config(){
    
    int32_t patperiod = (WRITE | PAT_PERIOD | 0x0000FFFF);  
    
    switch(string1.Chan){
        case 0:{
            wave_config[0] = 0x00000032; //Modulus for more AD9106's, also setting for continuous sine wave
            int32_t dds_cyc1 = (WRITE | DDS_CYC1 | 0x00000000);//Repeat indefinetly
            dds.lock();
            writeSPI(patperiod);
            writeSPI(dds_cyc1);
            dds.unlock();
            break;}
            
        case 1:{
            wave_config[1] = 0x00003200; //Modulus for more AD9106's, also setting for continuous sine wave
            int32_t dds_cyc2 = (WRITE | DDS_CYC2 | 0x00000000);
            dds.lock();
            writeSPI(patperiod);
            writeSPI(dds_cyc2);
            dds.unlock();
            break;}
            
        case 2:{
            wave_config[2] = 0x00000032; //Modulus for more AD9106's, also setting for continuous sine wave
            int32_t dds_cyc3 = (WRITE | DDS_CYC3 | 0x00000000);
            dds.lock();
            writeSPI(patperiod);
            writeSPI(dds_cyc3);
            dds.unlock();
            break;}
            
        case 3:{
            wave_config[3] = 0x00003200; //Modulus for more AD9106's, also setting for continuous sine wave
            int32_t dds_cyc4 = (WRITE | DDS_CYC4 | 0x00000000);
            dds.lock();
            writeSPI(patperiod);
            writeSPI(dds_cyc4);
            dds.unlock();
            break;}
            
    }//switch
    
}

/*******************************************************
Function: set_cycles()
Description: Sets cycles for pattern repeat for each channel. Info
            sent in trigger_ON() function
Author: Gabriel Hall
********************************************************/ 
void set_cycles(){
    
    int32_t cycles = (int32_t)string1.Cycl; 
    if(cycles > 255){
        cycles = 255;
        }
    switch (string1.Chan){
    
        case 0:
            pat_period[0]= cycles;
            break;
        case 1:
            pat_period[1]= cycles<<2;
            break;
        case 2:
            pat_period[2]= cycles;
            break;
        case 3:
            pat_period[3]= cycles<<2;
            break;
        }    
}

/*******************************************************
Function: set_Bcycles()
Description: Sets cycles in sinBurst for each channel burst period
Author: Gabriel Hall
********************************************************/ 
void set_Bcycles(){
    
    float cycles = string1.BPer*string1.Freq;
    int cyc = (int32_t)cycles;
    int32_t cycle = 0;
    switch (string1.Chan){
    
        case 0:
            cycle = (WRITE | DDS_CYC1 | cyc);
            dds.lock();
            writeSPI(cycle);
            dds.unlock();
            break;
        case 1:
            cycle = (WRITE | DDS_CYC2 | cyc);
            dds.lock();
            writeSPI(cycle);
            dds.unlock();
            break;
        case 2:
            cycle = (WRITE | DDS_CYC3 | cyc);
            dds.lock();
            writeSPI(cycle);
            dds.unlock();
            break;
        case 3:
            cycle = (WRITE | DDS_CYC4 | cyc);
            dds.lock();
            writeSPI(cycle);
            dds.unlock();
            break;
        }    
}
 
 /*******************************************************
Function: pattern_Period()
Description: Sets cycles in sinBurst for each channel
Author: Gabriel Hall
********************************************************/
 void pattern_Period(){

    float pat = string1.PatP/(5.8253) * 65535;//Normalized
    int32_t pa = (int32_t)pat;//Convert to int32 type for transmission truncating the decimal point
    int32_t pattern = (WRITE | PAT_PERIOD | pa);
    int32_t timebase = 0x000000F0;//Max time base for pattern period
    int32_t time = (WRITE | PAT_TIMEBASE | timebase);
    
    dds.lock();
    writeSPI(pattern);
    writeSPI(time);
    dds.unlock();       
}

/*******************************************************
Function: sine_DDS()
Description: holds functions for sine wave configuring
Author: Gabriel Hall
********************************************************/
void sine_DDS(){

    sine_wave_config();
    frequency_calc(); 
    delay_calc();
    //phase_calc();
    dgain_calc();
}

/*******************************************************
Function: bsine_DDS()
Description: holds functions for burst sine wave configuring
Author: Gabriel Hall
********************************************************/
void bsine_DDS(){

   frequency_calc();
   dgain_calc();
   delay_calc();
   //phase_calc();
   set_cycles();
   set_Bcycles();
   pattern_Period(); 
}

