#include "mbed.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
Serial pc(USBTX, USBRX); // tx, rx

char *chan[7];
int i=0;
int fillStruct();

struct Chan{
    int Cnfg; //(sine:0, bsin:1, trigON:2, trigOFF:3)
    int Chan; //(0-3)
    float Ampl; //pu
    int Freq;
    int Phas;
    float Dlay;
    float Ofst;
} string1;

int main()
{
    char str[128];
    pc.printf("%s\n\r", str);
    pc.gets(str, 55);
    
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
    return 0;
}

int fillStruct(){
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
    return(0);
     
}