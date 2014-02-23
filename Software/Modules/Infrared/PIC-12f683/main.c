#include <12F683.h>


#FUSES NOWDT                    //No Watch Dog Timer
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES NOCPD                    //No EE protection
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES PUT                      //Power Up Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES IESO                     //Internal External Switch Over mode enabled
#FUSES FCMEN                    //Fail-safe clock monitor enabled

#use delay(clock=4000000)
//TX RX
#define PIN_IR    PIN_A5   // INPUT modulo infrarrojo
#define PIN_SrTX  PIN_A0   // TX serie para conectar al PC

// Estados del autómata
#define IR_RESET    0
#define IR_INHIBIDO 127
#define IR_COMPLETO 32

// En otro caso: error.
#define IR_MIN_T 400
#define IR_MAX_T 1500
#define IR_AVERA 843


#use rs232(baud=8929,FORCE_SW,parity=N,xmit=PIN_SrTX,rcv=PIN_SrTX,bits=8)

unsigned char IR_Estado = IR_RESET;
unsigned int32 IR_comando;

short last_port_IR = 0;

#int_TIMER1
void  TIMER1_isr(void) 
{
   // No borrará un comando que se haya recibido bien
   if (IR_Estado != IR_INHIBIDO && IR_Estado != IR_COMPLETO) {
      IR_Estado = IR_RESET;
      IR_comando = 0;
   }
}

#int_RA
void  RA_isr(void) 
{
   short port_IR; // Estado del pin receptor IR

   // No usar input_a() aqui porque cambia a lectura
   // todos los pines del puerto.
   port_IR = input(PIN_IR);

   /* La interruptción la ha generado el modulo IR */
   // La Transición debe ser hacia 0, si es hacia 1 no hacemos nada
   if (last_port_IR && !port_IR && (IR_Estado != IR_INHIBIDO)){
      unsigned int16 lapso;
      lapso = get_timer1();

      if (lapso > IR_MAX_T || lapso < IR_MIN_T) {
         // ERROR o inicialización
         IR_Estado = IR_RESET;
         IR_comando = 0;
      } 
      else if   (lapso < IR_AVERA) {
         // Es un 0
         IR_comando <<= 1;
         bit_clear(IR_comando,0); // opcional
         IR_Estado++;
      } 
      else if (lapso > IR_AVERA) {
         // Es un 1
         IR_comando <<= 1;
         bit_set(IR_comando,0); // opcional
         IR_Estado++;
      }   
      set_timer1(0);
   }

      last_port_IR = port_IR;
   clear_interrupt(INT_RA);
}


void main(){
   int8 dir=0;
   int8 prueba;
   int8 i;
   /****************** INICIALIZAR **********************/
   setup_oscillator(OSC_4MHZ);
   IR_Estado = IR_RESET;
   
   // Deshabilitamos periféricos
   port_a_pullups(FALSE);
   setup_adc_ports(NO_ANALOGS|VSS_VDD);
   setup_adc(ADC_OFF);
   setup_ccp1(CCP_OFF);
   
   // Timer 1 controla el receptor IR.
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_2);


   enable_interrupts(INT_TIMER1);    // reset estado IR
   enable_interrupts(INT_RA);      // RX pulso IR
   enable_interrupts(GLOBAL);
 

   
   while(true){
      if (IR_Estado == IR_COMPLETO) {
         /* Se han recibido 32 bits de comando IR */
         IR_Estado = IR_INHIBIDO;
         //printf("Comando recibido: %LX\n", IR_comando);
         putc(IR_comando>>8);
         prueba=0; //para perder el tiempo
         for(i=0;i<10;i++){
            prueba++;
         }
     //    delay_us(10);
         putc(IR_comando);
//!         delay_ms(10);
         write_eeprom(dir,IR_comando>>8);
         dir++;
         write_eeprom(dir,IR_comando);
         dir++;
         /* Reseteamos la máquina de estados */
         IR_comando = 0;
         IR_Estado = IR_RESET;
      }
   
   
//!      putc('k');
//!      delay_ms(500);
   }
   
   }
