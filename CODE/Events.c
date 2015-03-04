/** ###################################################################
**     Filename  : Events.C
**     Project   : Pedal
**     Processor : MCF51AC256ACLK
**     Beantype  : Events
**     Version   : Driver 01.02
**     Compiler  : CodeWarrior ColdFireV1 C Compiler
**     Date/Time : 4/7/2010, 20:29
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         No public methods
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2008
**     UNIS, spol. s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"


#include "Tab.h"

word Medidas[3];
word Samples[15000];
word Count = 0;
byte T_Slice = 0;
word D_Time = 0;
word Rv_Time = 0;
byte Select = 0;
word D_Sample = 0;
word Rv_Sample = 0;

byte Port_Sim1a = 0;
byte Port_Sim1b = 0;
byte Port_Sim2a = 0;
byte Port_Sim2b = 0;

/*
** ===================================================================
**     Event       :  AD1_OnEnd (module Events)
**
**     From bean   :  AD1 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AD1_OnEnd(void)
{
  /* Write your code here ... */   
  AD1_GetValue(Medidas);  /* Guarda valor dos AD´s */
  
  Select = Bits2_GetVal();
   
  switch (Select){
    /**********************************/
    /* 10 - 250 ms (380 - 9300 Steps) */
    case 0:
      if(!T_Slice){        
        if (Count < 14999){  /* Verifica se estourou o contador */
          Samples[Count] = Medidas[0];
          Count++;    /* Incrementa contador */
        }else{
          Samples[14999] = Medidas[0];
          Count = 0;
          D_Time = (word)(Medidas[1] >> 2);  /* Guarda Valor do Off-Set */ 
        } 
      
        if (Count >= Tab_K2[D_Time]){
          D_Sample = Samples[Count - Tab_K2[D_Time]];
          asm{
            NOP;
          }
        }else{
          D_Sample = Samples[(word)(15000 - (Tab_K2[D_Time] - Count))];
        }
        
        Byte1_PutVal((byte)(D_Sample));
        Bits1_PutVal((byte)(D_Sample >> 8));           
  
       // Port_Sim1a = (byte)(D_Sample);
       // Port_Sim1b = (byte)(D_Sample >> 8);
      }
      
      if(T_Slice){
        T_Slice = 0;
      }else{
        T_Slice = 1;
      }
    /* ----------- OK!!!!!!! ---------------- */     
    break;

    /*********************************/
    /* 150 ~ 800 ms (2800 a 14999 Steps) */
    case 1:
      /* Primeiro intervalo */
      if (!T_Slice){
        if (Count < 14999){  /* Verifica se estourou o contador */
          Samples[Count] = Medidas[0];
          Count++;    /* Incrementa contador */
        }else{
          Samples[14999] = Medidas[0];
          Count = 0;
          D_Time = (word)(Medidas[1] >> 2);  /* Guarda Valor do Off-Set */ 
        } 
             
        if (Count >= Tab_K3[D_Time]){
          D_Sample = Samples[Count - Tab_K3[D_Time]]; 
          asm{
            NOP;
          }
        }else{
          D_Sample = Samples[(word)(15000 - (Tab_K3[D_Time] - Count))];
        }    
      
        Byte1_PutVal((byte)(D_Sample));
        Bits1_PutVal((byte)(D_Sample >> 8));     
      }
    
      if (T_Slice == 0x03){
        T_Slice = 0;
      }else{
        T_Slice++;
      }
        
      /* --------------- OK!!!!! -------------------- */
      break;
    
    /*********************************/
    case 2:
    /* 1ms a 14ms - 72 a 1023 */
        if (Count < 14999){  /* Verifica se estourou o contador */
          Samples[Count] = Medidas[0];
          Count++;    /* Incrementa contador */
        }else{
          Samples[14999] = Medidas[0];
          Count = 0;
          D_Time = (word)(Medidas[1] >> 2);  /* Guarda Valor do Off-Set */ 
        }   
        
        if (Count >= D_Time){
          D_Sample = Samples[Count - D_Time];    
          asm{
            NOP;
          }
        }else{
          D_Sample = Samples[(word)(15000 - (D_Time - Count))];
        }             

        Byte1_PutVal((byte)(D_Sample));
        Bits1_PutVal((byte)(D_Sample >> 8));    
      
      break;
    
    /*********************************/
    case 3:
      if (!T_Slice){
        /*  15ms - 200ms  - 18KHz - 1 Step do AD = 55us */
        if(Count < 14999){  /* Verifica se estourou o contador */
          Samples[Count] = Medidas[0];
          Count++;  /* Incrementa contador */
        } else{
          Samples[14999] = Medidas[0];
          Count = 0;
          D_Time = (word)(Medidas[1] >> 2);  /* Guarda Valor do Off-Set */
        }
      }    
      
      Rv_Time = (word)((D_Time + Tab_K0[T_Slice]) & 0x3ff);
      
      if (Count >= Tab_K4[Rv_Time]){
        Rv_Sample = Samples[Count - Tab_K4[Rv_Time]];
        asm{
          NOP;
        }        
      }else{
        Rv_Sample = Samples[(word)(15000 - (Tab_K4[Rv_Time] - Count))];
      } 
      
      T_Slice = (byte)((T_Slice + 1) & 0x03);

        Byte1_PutVal((byte)(Rv_Sample));
        Bits1_PutVal((byte)(Rv_Sample >> 8));    
    
      break;
  
    
    /*********************************/
    case 4:
    
      break;
      
    /*********************************/
    case 5:
    
      break;
      
    /*********************************/
    case 6:
    
      break;
      
    /*********************************/
    case 7:
    
      break;
      
    /*********************************/
    default:
      break;
  }
  
}


/* END Events */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 3.03 [04.07]
**     for the Freescale ColdFireV1 series of microcontrollers.
**
** ###################################################################
*/
