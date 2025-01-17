/***********************************
Date:Mar.18th,2020
Author: Gabriel Hall and Alexander Kossaian
Decription: Starting case statements to write register values to AD9106 based on user input
Next Addition: Phase control
***********************************/

//********* Header files **************//
#include "mbed.h" 
#include "spidegproj.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

//********** Definitions **********//
#define NO_ACTION   0//Cmds for incoming string
#define SINE        1
#define BSIN        2
#define TRIGON      3
#define TRIGOFF     4

#define MAX_CHAN    4// Set as many as youd like

//****************M487 pinout/protocol declaration*****************************
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
char acknolegment[4];

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

/*******************************************************
Function: main() 
Description: Main houses a while loop to keep the MCU Running
             during the use of the AD9106. The configuration registers
             are before the while loop so theyre done once at the beginning
             of the program. Inside the while loop serial communication
             is achieved and the data recieved goes through a case statement to
             determine proper configuration of the AD9106 based on the command.
Author: Gabriel Hall and Alexander Kossaian
Function #: 01
********************************************************/
int main() {
    
    configSPImcu();//Configure SPI parameters
    configDDSRegisters();//Default register values for SPI protocol/Power
    
    pc.gets(acknolegment, 4);
    if (strcmp("ack", acknolegment)==0){
        pc.printf("ack1");
        }     
    
    while(true){
        
        while(pc.readable()){           
            scanf("%s", str);
            wait(0.5);
            pc.printf("%s\n", str);
            parString();     
            str[0] = '\0'; // "flushing" the string
        }
        //led2 = 1;     //Troubleshooting
        
        switch (string1.Cnfg) {// CONGIFURE CONFIG TO 0 TO SHOW MESSAGE HAS BEEN PROCESSED

            case SINE://1
                //led1=0; //Troubleshooting
                sine_DDS();
                string1.Cnfg = 0;
                break;

            case BSIN://2
                bsine_DDS();
                string1.Cnfg = 0;
                break;

            case TRIGON://3
                trigger_ON();
                string1.Cnfg = 0;
                break;

            case TRIGOFF://4
                trigger_OFF();
                string1.Cnfg = 0;
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
Function #: 02
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
Function #: 03
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
Function #: 04
********************************************************/
void configDDSRegisters(){
    
    int32_t spi = (WRITE | SPICONFIG | 0x00002004);//Reset AD9106  
    dds.lock();
    writeSPI(spi);
    dds.unlock();
    spi = (WRITE | SPICONFIG | 0x00000000);//normal spi configuration
    int32_t power = (WRITE | POWERCONFIG | 0x00000E00);//Reset values of 
    int32_t pat_status = (WRITE | PAT_STATUS | 0x00000001);
    int32_t calconfig = (WRITE | CALCONFIG | 0x00000002);
    dds.lock();
    writeSPI(spi);
    writeSPI(power);
    writeSPI(calconfig);
    writeSPI(pat_status);
    dds.unlock();
    //led1 = 1; //Troubleshooting
}

/*******************************************************
Function: fillStruct()
Description: fills struct string1 with data
Author: Alexander Kossaian 
Function #: 05
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
Function #: 06
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
Function #: 07
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
Function #: 08
********************************************************/
void dgain_calc(){
        
    float dgain = string1.Ampl*1024;//Normalizing gain from 0 to 1
    int32_t gain = (int32_t)dgain;// Proper Truncation
    gain = gain << 4;// Gain registers dont bits [3:0] therefore bit shift result left 4 bits
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
Description: Calculates phase shift for channels. 
             This is what phase shift should be
Author: Gabriel Hall
Function #: 09
********************************************************/
void phase_calc(){
    
    int32_t ph_w = 0;
    
    float phase = (string1.Phas/360);// Make value P.U.
    float ph = phase*4096;//Resolution may be 65535 based on AD answer as to why phase was not working
    ph_w = (int32_t)ph;//Truncate decimal
    ph_w = ph_w <<4;//Based on AD software output, might not need this shift

    int32_t phase_word = 0;
    
    switch (string1.Chan){
            
        case 0:
            phase_word = (WRITE | DDS1_PW | ph_w);
            break;
            
        case 1:
            phase_word = (WRITE | DDS2_PW | ph_w);
            break;
            
        case 2:
            phase_word = (WRITE | DDS3_PW | ph_w);
            break;
            
        case 3:
            phase_word = (WRITE | DDS4_PW | ph_w);
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
Function #: 10
********************************************************/ 
void delay_calc(){
    
    float delay = (string1.Dlay/string1.PatP)*65536; //Dlay and PatP in mSec, 2^16 resolution for delay registers
    int32_t del= (int32_t)delay; //Truncations 
    
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
Function #: 11
***************************************************************************************************************/
void trigger_ON(){
    
    int32_t wav2_1config = (WRITE | WAV2_1CONFIG | 0x00003232); //Configure wave to be sine/Burst sine
    int32_t wav4_3config = (WRITE | WAV4_3CONFIG | 0x00003232); //Configure wave to be sine/Burst sine
    int32_t ram_update = (WRITE | RAMUPDATE | 0x00000001);// Have to write 1 to Ramupdate to update shadow registers to active registers
    int32_t ram_update2 = (WRITE | RAMUPDATE | 0x00000000);//Reset ramupdate value
    
    int32_t pat43 = (pat_period[3] | pat_period[2]);// Update pattern period registers together 
    int32_t pat21 = (pat_period[1] | pat_period[0]);// Update pattern period registers together
    
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
    trigger_ad = 0;//Activate wave generation by setting Trigger to 0    
}

/*******************************************************
Function: trigger_OFF()
Description: turns AD9106 off
Author: Gabriel Hall
Function #: 12
********************************************************/
void trigger_OFF(){

    int32_t ram_update = (WRITE | RAMUPDATE | 0x00000000);//Force ramupdate to 0
    dds.lock();
    writeSPI(ram_update);
    dds.unlock();
    trigger_ad = 1;// Turn off AD9106         
}

/*******************************************************
Function: sine_wave_config()
Description: configures continuous sine waves and pattern
             period.
Author: Gabriel Hall
Function #: 13
********************************************************/
void sine_wave_config(){
    
    int32_t patperiod = (WRITE | PAT_PERIOD | 0x0000FFFF);//Setting maximum available pattern period
    
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
            int32_t dds_cyc2 = (WRITE | DDS_CYC2 | 0x00000000);//Repeat indefinetly
            dds.lock();
            writeSPI(patperiod);
            writeSPI(dds_cyc2);
            dds.unlock();
            break;}
            
        case 2:{
            wave_config[2] = 0x00000032; //Modulus for more AD9106's, also setting for continuous sine wave
            int32_t dds_cyc3 = (WRITE | DDS_CYC3 | 0x00000000);//Repeat indefinetly
            dds.lock();
            writeSPI(patperiod);
            writeSPI(dds_cyc3);
            dds.unlock();
            break;}
            
        case 3:{
            wave_config[3] = 0x00003200; //Modulus for more AD9106's, also setting for continuous sine wave
            int32_t dds_cyc4 = (WRITE | DDS_CYC4 | 0x00000000);//Repeat indefinetly
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
Function #: 14
********************************************************/ 
void set_cycles(){
    
    int32_t cycles = (int32_t)string1.Cycl; // Make sure proper truncation
    if(cycles > 255){    // Make sure cycles isnt over the max limit 
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
Function #: 15
********************************************************/ 
void set_Bcycles(){
    
    float cycles = string1.BPer*string1.Freq;//Same as multiplying by 1 over frequency period
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
Function #: 16
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
Function #: 17
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
Function #: 18
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

