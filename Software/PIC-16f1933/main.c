#include <16F1933.h>
#include "ServoMotor2.h"
//Fuses
#FUSES INTRC_IO        //Oscilador interno
#FUSES NOWDT
#FUSES NOPROTECT 
//Use
#use delay(clock=1000000)
#use rs232(baud=8929,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8, ERRORS)

//Defines
#define trigger1 PIN_C0//PIN_A1
#define trigger2 PIN_A7

int primero=1;
int16 dato=0;
int flanco_sub1=1;
int flanco_sub2=1;
int16 Tbajada;
int16 Tsubida;
int1 Nuevopulso=0;
int1 sfr=1;

//sfr=1 es el sfr1, sfr=0 es el sfr2

void disparo(){
   if(sfr){
      enable_interrupts(INT_ccp1);
      set_timer1(0);
      output_high(trigger1);
      delay_us(10);
      output_low(trigger1);
      delay_ms(100);
   }
   else{
      enable_interrupts(INT_ccp2);
      set_timer1(0);
      output_high(trigger2);
      delay_us(10);
      output_low(trigger2);
      delay_ms(100);
   }
}




#INT_RDA
void RDA(){
   if(primero){
      dato=getc();
      dato=dato<<8;
      primero=0;
   }
   else{
      dato=dato+getc();
      primero=1;
   }


}

#INT_CCP1
void captura(){
   if(flanco_sub1){
      Tsubida=CCP_1;
      setup_ccp1(CCP_CAPTURE_FE);
      flanco_sub1=0;
   }
   else{
      Tbajada=CCP_1;
      setup_ccp1(CCP_CAPTURE_RE);
      flanco_sub1=1;
      Nuevopulso=1;
   }
}

#INT_CCP2
void captura2(){
   if(flanco_sub2){
      Tsubida=CCP_2;
      setup_ccp2(CCP_CAPTURE_FE);
      flanco_sub2=0;
   }
   else{
      Tbajada=CCP_2;
      setup_ccp2(CCP_CAPTURE_RE);
      flanco_sub2=1;
      Nuevopulso=1;
   }
}

void main()
{
/*Variables*/
   int dir=0;
   int8 orden=0;
   int16 tiempo;
   int16 distancia;
   int8 i=0;
   int16 distancia1=1, distancia2=1, D_critica=15, D_alerta=40;

/* Configuracion*/
   setup_oscillator(OSC_1MHZ);   //Config osc
   setup_timer_2(T2_DIV_BY_16,255,4);
   setup_CCP5(CCP_PWM);//
   setup_CCP4(CCP_PWM);//
   setup_timer_1(T1_INTERNAL);   //Config timer
   setup_ccp1(CCP_CAPTURE_RE);   //Config CCP1
   setup_ccp2(CCP_CAPTURE_RE);   //Config CCP2
   disable_interrupts (INT_ccp1); //CCP1
   disable_interrupts(INT_ccp2); //CCP2
   enable_interrupts(INT_RDA);     //interrupcion INT_RDA activada. comunicacion RS-485
   enable_interrupts (global); 
/*---------------*/

   while(TRUE){
      for(i=0;i<=1;i++){//Leemos la distancia del objeto que esta delante
         disparo();
         
         if(Nuevopulso){
            disable_interrupts (INT_ccp1);
            disable_interrupts (INT_ccp2);
            tiempo=Tbajada-Tsubida;
            distancia=(tiempo*4)/58;
            if(sfr==1){
               distancia1=distancia;
            }
            else{
               distancia2=distancia;
            }
   
            Nuevopulso=0;
            if(sfr==0){
               sfr=1;
            }
            else{
               sfr=0;
            }
            delay_ms(50);
         }
      }
      
     if(primero){ //Si ha leido un dato
        //Comprobamos cual es
         switch(dato){
            case 0x3067 :  orden = 1; break;//A ->
            case 0xD097 :  orden = 2; break;//M ->
            case 0x9037 :  orden = 3; break;//C ->
            case 0xD017 :  orden = 4; break;//A <-
            case 0x50D7 :  orden = 5; break;//M <-
            case 0x1077 :  orden = 6; break;//C <-
            case 0x90B7 :  orden = 7; break;//I 
            case 0x5057 :  orden = 8; break;//P 
            default    :   orden =0;  break;
         }
         
         //Actuamos
         
         if((distancia1 > D_alerta)&&(distancia2 > D_alerta)){
         switch(orden){
            case 1   :  giroAbierto(1); break;
            case 2   :  giroCerrado(1); break;
            case 3   :  giroMov(1);     break;
            case 4   :  giroAbierto(0); break;
            case 5   :  giroCerrado(0); break;
            case 6   :  giroMov(0);     break;
            case 7   :  inic();         break;
            case 8   :  parar();        break;
            default  :  inic();         break;}
     }
     else{
         if(distancia1 < distancia2) { //Obstáculo a la derecha (Giro a la izquierda)
            
            switch(orden){
               case 4   :  giroAbierto(0); break;
               case 5   :  giroCerrado(0); break;
               case 6   :  giroMov(0);     break;
               case 8   :  parar();        break;
               default  :  if (distancia1 > D_critica){ 
                              giroMov(1);
                           }
                           else if (distancia1 <= D_critica){
                              giroCerrado(0);
                           }          
                           break;}
         }
         else if (distancia1 > distancia2) { //Obstáculo a la izquierda (Girar a la derecha)
             
             switch(orden){
               case 1   :  giroAbierto(1); break;
               case 2   :  giroCerrado(1); break;
               case 3   :  giroMov(1);     break;
               case 8   :  parar();        break;
               default  :  if (distancia2 > D_critica){
                              giroMov(0); 
                           }
                           else if (distancia2 <= D_critica){ 
                              giroCerrado(1);
                              delay_ms(3000);
                           }          
                           break;}
         }       
         else { //Obstáculo delante
         
            switch(orden){
               case 2   :  giroCerrado(1); break;
               case 5   :  giroCerrado(0); break;
               case 8   :  parar();        break;
               default  :  if (distancia1 <= D_critica){ 
                              giroCerrado(0);
                              delay_ms(3000);
                           }         
                           break;}
         }
       }      
     }
       
       
   }

}
