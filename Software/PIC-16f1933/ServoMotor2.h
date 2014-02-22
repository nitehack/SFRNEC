/*******************************************************************************
 Driver para motorizacion de móviles con 2 servomotores. Concretamente para 
 sistemas con dos servomotores para el avance y giro. En este driver se 
 definen tres modos de giro:
 
    1. - Giro sobre si mismo. El eje de giro es el del propio dispositivo
    2. - Giro amplio. El eje de giro es una de las ruedas
    3. - Giro durante el desplazamiento
    
*******************************************************************************/

#define PI 3.14159

//ASIGNACIÓN DE LOS MÓDULOS CCP A CADA MOTOR
#define CCP_Mot1 CCP_5
#define CCP_Mot2 CCP_4


//!typedef struct{
//!   int32 fosc = 1000000;            //frecuecnia del oscilador del microcontrolador (hercios)
//!   int8  presc = 16;                //Prescaler del TIMER comun a los módulos para PWM
//!      
//!   float angVis = PI/6;                    //Angulo de vision del sensor (radianes)
//!   int dEje = 180;                  //distancia entre los ejes (mismas unidades que rRueda)
//!   int rRueda = 80;                    //radio de la rueda (mismas unidades que dEje)
//!   float ts = 0.055;                        //Periodo de muestreo (segundos)
//!   float radSgMot = 11.63553;       //Velocidad angular de salida del motor (rad/sg)
//!   int angParada;                   //Desviacion en el ángulo correspondiente a la orden de parada.
//!}conf;
//!
//!//int16 angMedio,angMax,angMin;
//!float p;

/***********************DEFINICIÓN DE FUNCIONES********************************/
//FUNCIÓN DE CONFIGURACIÓN
//!void config(conf *c){    
//!   
//!   //p = (c->dEje*c->angVis)/(4*c->ts*c->radSgMot*c->rRueda);
//!   p = 0.4821427886;
//!   
//!}

/*******************************************************************************
funciones de conversion para dado un angulo obtener el valor a cargar en el 
registro (AngToReg) y dado el valor del registro obtener el ángulo (RegToAng)
*******************************************************************************/

//!int AngToReg(conf *c,float ang){
//!   int a;
//!   a = (c->fosc/c->presc)*(ang/90.0*0.0005 + 0.001)/4;
//!   return a;
//!}
//!
//!float RegToAng(conf *c,int reg){
//!   int a;
//!   a = (90.0/0.0005)*(c->presc*reg/c->fosc - 0.001);
//!   return a;
//!}

//FUNCIONES PARA EL INICIO Y LA PARADA DEL MOVIMIENTO
/*******************************************************************************
La funcion de inicio situa los motores en el máximo teórico.
*******************************************************************************/

void inic(){
   //CCP_Mot1 = (c->fosc/c->presc)*(180/90*0.0005 + 0.001)/4;
   //CCP_Mot2 = (c->fosc/c->presc)*(180/90*0.0005 + 0.001)/4;
   CCP_Mot1 = 32;
   CCP_Mot2 = 20;
}

void parar(){
   //CCP_Mot1 = (c->fosc/c->presc)*(c->angParada/90*0.0005 + 0.001)/4;
   //CCP_Mot2 = (c->fosc/c->presc)*(c->angParada/90*0.0005 + 0.001)/4;
   CCP_Mot1 = 26;
   CCP_Mot2 = 26;
   //CCP_Mot1 = 24;
   //CCP_Mot2 = 24;
}

//FUNCIONES PARA EL CAMBIO DE DIRECCIÓN
/*******************************************************************************
dir = 1 -> izquierda           dir = 0 -> derecha
Para cesar el giro se debe hacer una llamada a las funciones de inicio o
parada defnidas de forma previa a estas.
*******************************************************************************/

void giroCerrado(short dir){
   //int8 valor1;
   //int8 valor2;
   
   //valor2 = (c->fosc/c->presc)*((c->angParada*(1+p/2.0))/90*0.0005 + 0.001)/4;
   //valor1 = (c->fosc/c->presc)*((c->angParada*(p/2.0))/90*0.0005 + 0.001)/4;
   
   if(dir){
      CCP_Mot1 = 29;//valor1;
      CCP_Mot2 = 29;//valor2;
      //CCP_Mot1 = 17;
      //CCP_Mot2 = 26;
   }else{
      CCP_Mot1 = 23;//valor2;
      CCP_Mot2 = 23;//valor1;
      //CCP_Mot1 = 26;
      //CCP_Mot2 = 17;
   }
}

void giroAbierto(short dir){
   //int8 valor1;
   //int8 valor2;

   //valor1 = (c->fosc/c->presc)*((c->angParada*1.05)/90*0.0005 + 0.001)/4;
   //valor2 = (c->fosc/c->presc)*((c->angParada*p)/90*0.0005 + 0.001)/4;

   if(dir){
      CCP_Mot1 = 30;//valor1;
      CCP_Mot2 = 26;//valor2;
      //CCP_Mot1 = 25;
      //CCP_Mot2 = 29;
   }else{
      CCP_Mot1 = 26;//valor2;
      CCP_Mot2 = 22;//valor1;
      //CCP_Mot1 = 29;
      //CCP_Mot2 = 25;
   }
}

void giroMov(short dir){
   //int8 valor1;
   //int8 valor2;
   //float aux1 = (270/0.0015)*(c->presc*CCP_Mot2/c->fosc - 0.001);
   //float aux2 = (270/0.0015)*(c->presc*CCP_Mot1/c->fosc - 0.001);

   //valor1 = (p*CCP_Mot2) + (c->fosc/c->presc)*(1 - p)*0.001;
   //valor2 = (p*CCP_Mot1) + (c->fosc/c->presc)*(1 - p)*0.001;
   //(c->fosc/c->presc)*((aux2*p)/90*0.0005 + 0.001)/4;
   
   if(dir){
      CCP_Mot1 = 28;
      CCP_Mot2 = 20;
   }else{
      CCP_Mot1 = 32;
      CCP_Mot2 = 24;
   }
}

