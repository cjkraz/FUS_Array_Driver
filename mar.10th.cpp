/***********************************
Date:Mar.2nd,2020
Author: Gabriel Hall
Decription: Starting case statements to write register values to AD9106 based on user input
Next Addition: Feedback to python
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

//M487 pinout declaration
Serial pc(USBTX, USBRX); // tx, rx
SPI dds(PA_8, PA_9, PA_10);//d11-mosi, d12-miso, d13-sclk
DigitalOut cs1(D12);//DDS1
DigitalOut trigger(D6);//Active LOW
DigitalOut led2(LED1);//Active LOW
DigitalIn  pb1(SW2);//Float HIGH

//Function Declarations
void configSPImcu();
void writeSPI(int32_t data);
void configDDSRegisters();
void fillStruct();
void data_transfer();
void frequency_calc();
void dgain_calc();
void phase_calc();
void delay_calc();
void trigger_ON();
void trigger_OFF();
void sine_DDS();

//Global variables
char *chan[7];
int i=0;

struct Chan{
    int Cnfg; //(noaction:0,sine:1, bsin:2, trigON:3, trigOFF:4)
    int Chan; //(0-3)
    float Ampl; //pu
    int Freq;// 
    int Phas;//
    float Dlay;
    float Ofst;
} string1;

int main() {
    
    configSPImcu();
    configDDSRegisters();
    
    while(true){
        
        data_transfer();
            
        switch (string1.Cnfg) {// CONGIFURE CONFIG TO 0 TO SHOW MESSAGE HAS BEEN PROCESSED
                
            case SINE://1
                sine_DDS();
                break;
                
            case BSIN://2
                // Configure later
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
        string1.Cnfg = 0;        
    }//while
}//main

void configSPImcu(){
    
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

void configDDSRegisters(){
    
    int32_t spi = (WRITE | SPICONFIG | 0x00000002);//Reset AD9106  
    dds.lock();
    writeSPI(spi);
    dds.unlock();
    spi = (WRITE | SPICONFIG | 0x00000000);
    int32_t power = (WRITE | POWERCONFIG | 0x00000E00);
    int32_t pat_status = (WRITE | PAT_STATUS | 0x00000001);
    int32_t calconfig = (WRITE | CALCONFIG | 0x00000002);
    /*      int32_t dac4rset = (WRITE | DAC4RSET | 0x00001F00);////// Might not be needed
            int32_t dac3rset = (WRITE | DAC3RSET | 0x00001F00);///////
            int32_t dac2rset = (WRITE | DAC2RSET | 0x00001F00);////////
            int32_t dac1rset = (WRITE | DAC1RSET | 0x00001F00);*/////////
    
    /*      int32_t calconfig = (WRITE | CALCONFIG | 0x00000002);//Not enabled [3]->Enable
            int32_t wav4_3config = (WRITE | WAV4_3CONFIG | 0x00000000);//////
            int32_t wav2_1config = (WRITE | WAV2_1CONFIG | 0x00003232);////USED sine_DDS()
            int32_t patperiod = (WRITE | PAT_PERIOD | 0x0000FFFF);//////USED sine_DDS()
            int32_t dac4_3pat = (WRITE | DAC4_3PATx | 0x00000000);////////
            int32_t dac2_1pat = (WRITE | DAC2_1PATx | 0x00001010);/////////
            int32_t dac1_gain = (WRITE | DAC1_DGAIN | 0x00002000);////////
            int32_t saw4_3config = (WRITE | SAW4_3CONFIG | 0x00000000);//////
            int32_t saw2_1config = (WRITE | SAW2_1CONFIG | 0x00000200);
            int32_t dds_tw32 = (WRITE | DDS_TW32 | 0x00000024);/////////
            int32_t dds_tw1 = (WRITE | DDS_TW1 | 0x00006900);/////////
            int32_t dds_cyc4 = (WRITE | DDS_CYC4 | 0x00000000);////////
            int32_t dds_cyc3 = (WRITE | DDS_CYC3 | 0x00000000);///////
            int32_t dds_cyc2 = (WRITE | DDS_CYC2 | 0x00000000);///////
            int32_t start_dly1 = (WRITE | START_DLY1 | 0x00000FA0);
            int32_t dds_cyc1 = (WRITE | DDS_CYC1 | 0x00000100);*/
    
    dds.lock();
    writeSPI(spi);
    writeSPI(power);
    writeSPI(calconfig);
    writeSPI(pat_status);
    dds.unlock();
}

void data_transfer()
{
    char str[128];
    pc.printf("%s\n\r", str);
    scanf("%s", str);// had to add ; thanks cossain
    
    char *token = strtok(str, ":");
    
    while (token != NULL)
        {
        //printf("Before: %s\n", token);
        token = strtok(NULL, ",");
        chan[i] = token;
        //printf("Chan[%i]: %s\n", i, chan[i]);
        i++;
        if(i==7){break;}
        }
    fillStruct();
    //return 0;
}

void fillStruct(){
int t = 0;
string1.Chan = atoi(chan[0]);
    
while(t < 7){
        
    char *var;
    char *val = chan[t];
        
    var = strtok_r(val, ":", &val);
        //printf("Var: %s\n", var);
        //printf("Val: %s\n", val);
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
            string1.Phas = atoi(val);
            //pc.printf("Phase has: %i in it\n", string1.Phas);
        }
        if(strcmp("Dlay", var)==0){
            string1.Dlay = atof(val);
            //pc.printf("Delay has: %f in it\n", string1.Dlay);
        }
        if(strcmp("Ofst", var)==0){
            string1.Ofst = atof(val);
            //pc.printf("Offset has: %f in it\n", string1.Ofst);
        }
        t++;
    }
    //return(0);
     
}

void frequency_calc(){

    int32_t fref = 180000000; // max fout 5 MHz
    
    int32_t tuning_word = (string1.Freq/fref)<<24;
    int32_t msb = (tuning_word & 0x00FFFF00)>>8;//capture bits [23:8] then shift them eight to the right
    int32_t lsb = (tuning_word & 0x000000FF);
    
    int32_t msb_tw = (WRITE | DDS_TW32 | msb);
    int32_t lsb_tw = (WRITE | DDS_TW1 | lsb);
    
    dds.lock();
    writeSPI(msb_tw);
    writeSPI(lsb_tw);
    dds.unlock();     
}

void dgain_calc(){
        
    float dgain = string1.Ampl*1024;//Normalizing gain from 0 to 1
    int32_t gain = dgain;
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

void phase_calc(){
    
    int32_t phase = (string1.Phas/360)<<16;
    int32_t phase_word = 0;
    switch (string1.Chan){
    
        case 0:
            phase_word = (WRITE | DDS1_PW | phase);
            break;
        case 1:
            phase_word = (WRITE | DDS2_PW | phase);
            break;
        case 2:
            phase_word = (WRITE | DDS3_PW | phase);
            break;
        case 3:
            phase_word = (WRITE | DDS4_PW | phase);
            break;
        }    
    dds.lock();
    writeSPI(phase_word);
    dds.unlock();
 }
 
void delay_calc(){
    
    int32_t delay = string1.Dlay*65536;
    int32_t delay_word = 0;
    switch (string1.Chan){
    
        case 0:
            delay_word = (WRITE | START_DLY1 | delay);
            break;
        case 1:
            delay_word = (WRITE | START_DLY2 | delay);
            break;
        case 2:
            delay_word = (WRITE | START_DLY3 | delay);
            break;
        case 3:
            delay_word = (WRITE | START_DLY4 | delay);
            break;
        }    
    dds.lock();
    writeSPI(delay_word);
    dds.unlock();
 }

void trigger_ON(){

    int32_t ram_update = (WRITE | RAMUPDATE | 0x00000001);
    dds.lock();
    writeSPI(ram_update);
    dds.unlock();
    trigger = 0;        
}

void trigger_OFF(){

    int32_t ram_update = (WRITE | RAMUPDATE | 0x00000000);
    dds.lock();
    writeSPI(ram_update);
    dds.unlock();
    trigger = 1;        
}

void sine_DDS(){

    int32_t patperiod = (WRITE | PAT_PERIOD | 0x0000FFFF);  
    int32_t wav2_1config = (WRITE | WAV2_1CONFIG | 0x00003232); 
    frequency_calc(); 
    phase_calc();
    dgain_calc();
    //delay_calc();
    dds.lock();
    writeSPI(patperiod);
    writeSPI(wav2_1config);
    dds.unlock();
     
}

