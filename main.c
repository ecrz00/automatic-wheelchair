#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "tm4c1294ncpdt.h"
#include "SSD1306.h"
float X, Y;
uint32_t cta, D=0;
int data, A=11500, B=-3000, ind=1;
bool firstF=true, firstB=true, firstL=true, firstR=true, firstS=true, firstI=true, firstO=true;

//---------------------------------------------------------------------------------Inicialización---------------------------------------------------------------------------------
void ADC_Init(void);
void UART_Init(void);
void PortC_Init(void);
void PWM_Init(void);
void General_Init(void);
//------------------------------------------------------Interupciones---------------------------------------------------------------------------------
int UART6_Handler(void);
void PortC_Handler(void);

//---------------------------------------------------------------------------------De movimiento---------------------------------------------------------------------------------
void Forward(float Y);
void Backward(float Y);
void Rightward(void);
void Leftward(void);
void Stop(void);
//-----------------------------------------------------------------------------------------------------------Varias------------------------------------------------------------------------------------------------------------
int ADCSumary(float X, float Y);
void movementCase(int n, float Y);
//------------------------------------------------------------------------------------------------------------------------SPI---------------------------------------------------------------------------------------------
void SSI3_init (void);
void SSI3_sendData (uint16_t dat);
void pot_setVal (uint8_t slider);


int main(void) {
    General_Init();
    while(1){
    GPIO_PORTC_AHB_DATA_R |= 0X10;                                                                          //Enciende el puerto para disparar la señal de trigger
    for(i = 0; i< 50; i++);                                                                                 //  Retardo > 10us
    GPIO_PORTC_AHB_DATA_R &= ~0X10;                                                                         //Apaga el puerto para deterner el disparo,
    ADC0_PSSI_R = 0x0004;                                                                                   //Inicia conversión del SS2
    while((ADC0_RIS_R&0x04)==0);                                                                            // Espera a que SS2 termine conversión (polling)
    Y = (3.3/4096)*(ADC0_SSFIFO2_R&0xFFF);                                                                  // se lee el primer resultado asociado a PE5
    X = (3.3/4096)*(ADC0_SSFIFO2_R&0xFFF);                                                                  // se lee el segundo resultado asociado a PE4
    ADC0_ISC_R = 0x0004;                                                                                    //Limpia la bandera RIS del ADC0
    movementCase(ADCSumary(X,Y)*ind,Y);                                                                     //Función de determinará hacia donde desplazar
    }
}



//---------------------------Función para inicializar el puerto UART6 asociado a P0 como receptor-----------------------------------------------------------------------------------------------------------------------------
void UART_Init(void){
    //Usaremos U6Rx en P0 (1)
    SYSCTL_RCGCUART_R |= 0x00000040;                                                                        //Reloj al UART6 (p. 388)
    while((SYSCTL_PRUART_R & 0x00000040)== 0);                                                              //Reloj listo ?
    UART6_CTL_R &= ~0x00000001;                                                                             //Deshabilita UART pre-configuración (p.1188)
    UART6_IBRD_R = 8;                                                                                       //Parte entera del divisor de baudios. IBRD = int(16,000,000 / (16 * 115,200)) = int(8.681) (p.1184)
    UART6_FBRD_R = 44;                                                                                      //Parte fraccionaria FBRD = round(0.6806 * 64) = 44 (p. 1185)
    UART6_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);                                                        //Registro line control: palabra de 8 bits habilitando la fifo (0x60|0x10).
    UART6_CC_R =  (UART6_CC_R&~UART_CC_CS_M)+UART_CC_CS_PIOSC;                                              //Configuración del reloj. Fuente alterna del reloj (0x5) definida en ALTCLKCFG (p.1213)
    SYSCTL_ALTCLKCFG_R = (SYSCTL_ALTCLKCFG_R&~SYSCTL_ALTCLKCFG_ALTCLK_M)+SYSCTL_ALTCLKCFG_ALTCLK_PIOSC;     //Fuente PIOSC, oscilador interno 16 MHz

    UART6_IFLS_R &= ~0x0000003F;                                                                            //Selector de nivel FIFO: Limpia interrupcion de FIFO Rx y Tx poniendo 0 a lo largo del registro
    UART6_IFLS_R += (0x00000000|0x00000000);                                                                // Habilita interrupcion para RX, TX FIFO >= 1/8
    UART6_ICR_R = 0x10;                                                                                     //Limpia bandera de interrupción de recepción (p.1206)
    UART6_IM_R |= 0x00000010;                                                                               //Desenmascara interrupción de recepción (p.1194)

    UART6_CTL_R &= ~0x00000020;                                                                             //Define reloj del UART como reloj del sistema dividido en 16 (p.1188)
    UART6_CTL_R |= 0x00000001;                                                                              //Habilita UART (p. 1188)
    while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0);                                                       //Espera que el reloj se estabilice
    GPIO_PORTP_DIR_R &= ~0x01;                                                                              //Definimos P0 como entrada
    GPIO_PORTP_DEN_R |= 0x01;                                                                               //Función digital habilitada (p 781)
    GPIO_PORTP_AFSEL_R |= 0x01;                                                                             //Habilita función alterna de P0
    GPIO_PORTP_PCTL_R = 0x00000001;                                                                         //Controla P0 con su función de UART6
    GPIO_PORTP_AMSEL_R &= ~0x01;                                                                            //Deshabilita función analógica de P0 (p. 786)

    NVIC_PRI14_R = (NVIC_PRI14_R&0x00FFFFFF)|0x00000000;                                                    //Define mayor prioridad, numero de interrupcion 59 (4(14)+3)
    NVIC_EN1_R = 1<<(59-32);                                                                               //Habilita interrupción, en la casilla 27 del registro
}
//------------------------------------Función para el ADC asociado a E4-5----------------------------------------------------------------------------------------------------------------------------------------
void ADC_Init(void){
//-------------------------Para el puerto asociado al ADC---------------------------------
   GPIO_PORTE_AHB_DIR_R &= ~0x30;                                                                           // E5-4 entradas (analógica)
   GPIO_PORTE_AHB_AFSEL_R |= 0x30;                                                                          // Habilita Función Alterna en E5, E4 (p. 770)
   GPIO_PORTE_AHB_DEN_R &= ~0x30;                                                                           // Deshabilita Función Digital en E5, E4 (p 781)
   GPIO_PORTE_AHB_AMSEL_R |= 0x30;                                                                          // Habilita Función Analógica de E5, E4 (p. 786)


// ------------------------Para los controladores del motor----------------------------------------
    GPIO_PORTA_AHB_DIR_R =0x30;                                                                             //Define A4, A5 como salida
    GPIO_PORTA_AHB_DEN_R |=0x30;                                                                            //Define A4, A5 como digital

    GPIO_PORTB_AHB_DIR_R =0x30;                                                                             //Define B4, B5 como salida
    GPIO_PORTB_AHB_DEN_R |=0x30;                                                                            //Define B4, B5 como digital

    SYSCTL_RCGCADC_R |= 0x01;                                                                               // Habilita reloj para ADC0(p. 396)
    while((SYSCTL_PRADC_R & 0x01) == 0);                                                                    // Se espera a que el reloj se estabilice
    ADC0_PC_R = 0x01;                                                                                       // Configuración del periférico (tasa de muestreo). Configura para 125Ksamp/s (p.1159) 1/(Nhs+12)Tclk con Nhs=112 + 4
    ADC0_SSPRI_R = 0x0123;                                                                                  // SS3 con la más alta prioridad
    ADC0_ACTSS_R &= ~0x0004;                                                                                // Control que activa los secuenciadores: Deshabilita SS2 antes de cambiar configuración de registros (p. 1076)
    ADC0_EMUX_R = 0x0000;                                                                                   // Evento que dispara el trigger: Se configura SS2 para disparar muestreo por software, PSSI sin efecto (default) (p.1091)
    ADC0_SAC_R = 0x0;                                                                                       // Se configura para no tener sobremuestreo por hardware(default)(p. 1105)
    ADC0_CTL_R = 0x0;                                                                                       // Se configura con referencias internas (default VDDA and GNDA) (p. 1107)
    ADC0_SSOP2_R = 0x0000;                                                                                  // Registro de operación: Se configure para salvar los resultados del ADC en FIFO (default)(p. 1134)
    ADC0_SSTSH2_R = 0x000;                                                                                  // Se configure el ADC para un periodo de 4. TSHn=0x0 -> Nhs=4 (default) (p. 1139)
    ADC0_SSMUX2_R = 0x0089;                                                                                 // Se configura entradas 1 muestra=AIN9 2 muestra=AIN8 (p.1128)
    ADC0_SSEMUX2_R &= ~0x0011;                                                                              // La cuarta muestra es seleccionada de AIN(15:0) (p.1137)
    ADC0_SSCTL2_R = 0x0060;                                                                                 // Define la segunda muestra como la última de la secuencia e interrupción al final de segundo muestreo(p.1130)
    ADC0_IM_R &= ~0x0004;                                                                                   // Deshabilita interrupción SS2 porque el estado del 2do secuenciador no afecta el SS2 interrupt status(p. 1081)
    ADC0_ACTSS_R |= 0x0004;                                                                                 // Habilita SS2 (p. 1076)

    SYSCTL_PLLFREQ0_R |= 0x00800000;                                                                        // encender PLL. PLL oscilará basado en los valores de los registros PLLFRECQ0-1
    while((SYSCTL_PLLSTAT_R&0x01)==0);                                                                      // espera a que el PLL fije su frecuencia
    SYSCTL_PLLFREQ0_R &= ~0x00800000;                                                                       // apagar PLL
    ADC0_ISC_R = 0x0004;                                                                                    // Limpia INR2 y ADCRIS
}
//-------------------------------------------------sensor ultrasónico y su inicializazión asociada a PC4-5---------------------------------------------------------------------------------------------------------------------------------------------------------
void PortC_Init(void){
    GPIO_PORTC_AHB_DIR_R = 0X10;                                                                            //Definimos C4 como salida y C5 como entrada
    GPIO_PORTC_AHB_DEN_R |= 0X30;                                                                           //Función digital en C4, C5
    GPIO_PORTC_AHB_DATA_R &= ~0X10;                                                                         //

    GPIO_PORTC_AHB_IS_R &= ~0x20;                                                                           //C5 sensible por flanco
    GPIO_PORTC_AHB_IBE_R &= ~0x20;                                                                          //C5 sensible a un solo flanco
    GPIO_PORTC_AHB_IEV_R |=  0x20;                                                                          //C5 sensible por flanco de subida
    GPIO_PORTC_AHB_ICR_R = 0x20;                                                                            //Limpiamos bandera C5
    GPIO_PORTC_AHB_IM_R |= 0x20;                                                                            //Desenmascaramos C5

    NVIC_EN0_R = 0x00000004;                                                                                //Numero de interrupción 2, en EN0 casilla 2
    NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00200000;                                                      //Esta en 4n+2 [23:21]. Prioridad 1
}
//-------------------------------------------------------------------------------------------------- Señales PWM en TIMER0---------------------------------------------------------------------------------------------------------------------------------------------------
void PWM_Init(void){
             SYSCTL_RCGCTIMER_R |= 0x01;                                                                    //Habilita timer 0
             GPIO_PORTD_AHB_DIR_R |= 0x03;                                                                  //Define D0, D1 como salidas
             GPIO_PORTD_AHB_DEN_R |= 0x03;                                                                  //Habilita función digital en D0, D1
             GPIO_PORTD_AHB_DATA_R = 0x00;
             GPIO_PORTD_AHB_AFSEL_R = 0x03;                                                                 //Función alterna en D0, D1 (p. 770)
             GPIO_PORTD_AHB_PCTL_R = 0x00000033;                                                            //MUX0, MUX1 controla D0, D1 respectivamente (p 787)

             TIMER0_CTL_R =0x00000000;                                                                      //Reset en TIMER0 para configurar (p. 986)
             TIMER0_CFG_R =0x00000004;                                                                      //Selecciona configuración de 16-bit (p. 976)
//-------------------------------------Para subtimer A--------------------------------------------------------------------------------------
             TIMER0_TAMR_R = 0x0000000A;                                                                    //Habilita modo PWM/ Modo periódico de TIMER0-A (p. 977)
             TIMER0_TAILR_R = 16000;                                                                        //Valor de carga para TIMER0-A (p. 1004)
             TIMER0_TAMATCHR_R = 8000;                                                                      //Valor de match para el ciclo de trabajo TIMER0-A (p.1006)
             TIMER0_TAPR_R=0X00;                                                                            //Sin preescala (p. 1008)
//-------------------------------------Para subtimer B--------------------------------------------------------------------------------------
             TIMER0_TBMR_R = 0x0000000A;                                                                     //Habilita modo PWM/ Modo periódico de TIMER0-B (p. 982)
             TIMER0_TBILR_R = 16000;                                                                         //Valor de carga para TIMER0-B (p. 1005)
             TIMER0_TBMATCHR_R = 8000;                                                                       //Valor de match para el ciclo de trabajo TIMER0-B (p.1007)
             TIMER0_TBPR_R=0X00;                                                                             //Sin preescala (p. 1008)

             TIMER0_CTL_R |=0X00000101;                                                                      //Habilita Timer0 A y Timer0 B

}
//---------------------------------------------------------------------------------------------Para SPI Q[0:2]-------------------------------------------------------------------------------------------------------------------------------------------------------
void SSI3_init (void) {
    SYSCTL_RCGCSSI_R = SYSCTL_RCGCSSI_R3;                                                                   // Activa reloj al SSI3
    while ((SYSCTL_PRSSI_R & SYSCTL_PRSSI_R3) == 0);                                                        // Espera a que este listo

    GPIO_PORTQ_DIR_R = 0x07;                                                                                // Configura Q0 [reloj], Q1[frame signal], Q2[Bi-directional data pin 0] como salidas (p.760)
    GPIO_PORTQ_DEN_R |= 0x07;                                                                               // Habilita la función digital (p. 781)
    GPIO_PORTQ_AFSEL_R |= 0x07;                                                                             // Seleciona la función alterna de Q[0:2] (p. 770)
    GPIO_PORTQ_PCTL_R = 0x00000EEE;                                                                         // Configura las terminales de Q a su función de SSI3 (p 787)
    GPIO_PORTQ_AMSEL_R &= ~0x07;                                                                            // Deshabilita la función analogica

    SSI3_CR1_R = 0x00;                                                                                      // QSSI Control 1: Deshabilita SSI3 y establece como maestro (p. 1247)
    SSI3_CPSR_R = 0x02;                                                                                     // QSSI Clock prescale: preescalador (CPSDVSR=2) del reloj SSI (p. 1252)

    SSI3_CR0_R = (0x0100 | SSI_CR0_FRF_MOTO | SSI_CR0_DSS_16) & ~(SSI_CR0_SPO | SSI_CR0_SPH);               // configura para Freescale SPI; 16bit; 4 Mbps (16MHz/(CPSDVSR=2)(1+(SCR=1))); SPH = 0 (captura de datos en la transición del primer flanco); SPO = 0 (estado libre bajo) (p. 1245)

    SSI3_CR1_R |= SSI_CR1_SSE;                                                                              // Habilita SSI3 (p. 1247)
}

void SSI3_sendData (uint16_t dat) {
    // Envia dato de 8-bit
    while ((SSI3_SR_R & SSI_SR_BSY) != 0);                                                                  // Verifico si el bus está ocupado en transmición/recepción. Espero que esté vacio. (1=busy;0=idle)
    SSI3_DR_R = dat;                                                                                        // envia dato.
}

void pot_setVal(uint8_t slider) {
    //Combine el valor del control deslizante con el código de comando de escritura.
    // Estructura del mensaje SPI: [comando (8-bits)][deslizador (8-bits)]
    SSI3_sendData(0x1100 | slider);
}


//--------------------------------------------------------------------------------------------------Función de inicialización general-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void General_Init(void){
    SYSCTL_RCGCGPIO_R |= (0x201F|SYSCTL_RCGCGPIO_R14);                                                      //Habilita reloj para Puerto E, A, B, C,  D, P y Q (p.382)
    while((SYSCTL_PRGPIO_R & (0x201F|SYSCTL_RCGCGPIO_R14)) == 0);                                           //Se espera a que el reloj se estabilice para cada puerto (p.499)
    UART_Init();                                                                                            //Función que inicializa UART
    ADC_Init();                                                                                             //Función que inicializa el ADC
    PortC_Init();                                                                                           //Función que inicializa el puerto C (asociado a la interrupción)
    PWM_Init();                                                                                             //Función que inicializa los PWM de cada motor
    SSI3_init();                                                                                            //Función que inicializa la comunicación SPI
    I2C_Init();                                                                                             //Función que inicializa I2C
    while(I2C0_MCS_R&0x00000001){};                                                                         // espera que el I2C esté listo
    SSD1306_Init();                                                                                         //Función que inicializa los datos necesarios al display
}
//--------------------------------------------------------------------------------------------------Función que define la posición del Joystick----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ADCSumary(float X, float Y){                                                                            //Recibe los valores del ADC asociados a X, Y
    int inst=0;
    if(Y>1.8){                                                                                              //Si debe ir hacia adelante
        inst=10;
    }else{
        if(Y<1.5){                                                                                          //Si debe ir hacia atrás
            inst=5;
        }else{
            if(X>1.8){                                                                                      //Si debe ir hacia la izquierda
                inst=8;
            }else{
                if(X<1.5){                                                                                  //Si debe ir hacia la derecha
                    inst=2;
                }else{
                    inst=4;                                                                                 //Si debe estar detenido
                }
            }
        }

    }
    return inst;                                                                                            //Regresa el valor asociada a la direccióm de desplazamiento
}
//--------------------------------------------------------------------------------------------------Funciones para el movimiento----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Forward(float Y){
        TIMER0_TBMATCHR_R = A+B*Y;                                                                          //Ciclo de trabajo dependiendo de la inclinación del Joystick
        TIMER0_TAMATCHR_R = A+B*Y;                                                                          //Ciclo de trabajo dependiendo de la inclinación del Joystick
    if(firstF){                                                                                             //Condición de estado del movimiento hacia adelante
        GPIO_PORTA_AHB_DATA_R = 0x20;                                                                       //Asigna 1 en A5 y un 0 en A4
        GPIO_PORTB_AHB_DATA_R = 0x20;                                                                       //Asigna 1 en B5 y un 0 en B4
        if(firstI){                                                                                         //Condición de interrupción
            SSD1306_Clear();                                                                                //Limpia pantalla
            SSD1306_SCROLLINGL();                                                                           //Desplaza a la izquierda
            SSD1306_Command(SSD1306_SETPAGE0);                                                              //Pagína 0
            SSD1306_WriteString("[_______________");                                                        //Mensaje para pagina 0
            SSD1306_Command(SSD1306_SETPAGE1);                                                              //pagina 1
            SSD1306_WriteString("   ]---------------");                                                     //Mensaje para pagina 1
        }
        firstF=false;                                                                                       //Cambia estado de fordward
        firstB=true;                                                                                        //resetea los otros estados
        firstL=true;
        firstR=true;
        firstS=true;
    }
}
void Backward(float Y){
    TIMER0_TBMATCHR_R = A+B*(3.3-Y);                                                                        //Ciclo de trabajo dependiendo de la inclinación del Joystick
    TIMER0_TAMATCHR_R = A+B*(3.3-Y);                                                                         //Ciclo de trabajo dependiendo de la inclinación del Joystick
    if(firstB){                                                                                              //Conducuón de backward
        GPIO_PORTA_AHB_DATA_R = 0x10;                                                                        //Asigna 1 a A4 y 0 a B4
        GPIO_PORTB_AHB_DATA_R = 0x10;                                                                       //Asigna 1 a B5 y 0 a B4
        if(firstI){                                                                                         //Condición de interrupción
            SSD1306_Clear();                                                                                 //Limpia pantalla
            SSD1306_SCROLLINGR();                                                                           //Desplaza a la derecha
            SSD1306_Command(SSD1306_SETPAGE0);                                                              //Define página 0
            SSD1306_WriteString("________________(");                                                       //Cadena para pagina 0
            SSD1306_Command(SSD1306_SETPAGE1);                                                              //Define pagina 1
            SSD1306_WriteString("  ----------------)");                                                     //Cadena pagina 1
        }
        firstB=false;                                                                                       //Cambia estado backward
        firstF=true;                                                                                        //Resetea los otros edos
        firstL=true;
        firstR=true;
        firstS=true;
    }
}
void Leftward(void){
    if(firstL){
        GPIO_PORTB_AHB_DATA_R = 0x20;                                                                   //Indica desplazamiento hacie adelante de la rueda asociada a B
        GPIO_PORTA_AHB_DATA_R = 0x30;                                                                   //Impide el movimiento de motor A
        TIMER0_TBMATCHR_R = 6400;                                                                       //Ciclo de trabajo del 60% asociado al motor
        if(firstI){
            SSD1306_Clear();                                                                            //Limpia pantalla
            SSD1306_SCROLLINGL();                                                                       //Desplazamiento a la izquierda
            SSD1306_Command(SSD1306_SETPAGE0);                                                          //Establece pagina 0
            SSD1306_WriteString("|----------");                                                         //Cadena a pagina 0
            SSD1306_Command(SSD1306_SETPAGE1);                                                          //Define pagina 1
            SSD1306_WriteString("                                   ^");                                //Cadena a pagina 1
        }
        firstF=true;                                                                                    //Reset a los demas edos
        firstR=true;
        firstB=true;
        firstS=true;
        firstL=false;                                                                                   //Cambia edo leftward
    }

}
void Rightward(void){
    if(firstR){                                                                                         //Valida edo right
        GPIO_PORTA_AHB_DATA_R = 0x20;                                                                   //Movimiendo de A hacia adelante
        GPIO_PORTB_AHB_DATA_R = 0x30;                                                                   //Impide movimiento de B
        TIMER0_TAMATCHR_R = 6400;                                                                       //Ciclo de trabajo del 60% asociado al motor
        if(firstI){                                                                                     //Valida eso de interrupción
            SSD1306_Clear();                                                                            //Limpia pantalla
            SSD1306_SCROLLINGL();                                                                       //Desplaza hacia la izquierda
            SSD1306_Command(SSD1306_SETPAGE0);                                                          //Define pagina 0
            SSD1306_WriteString("*");                                                                   //Envia cadena a pagina 0
            SSD1306_Command(SSD1306_SETPAGE1);                                                          //Define pagina 1
            SSD1306_WriteString("                       |___________");                                 //Cadena pagina 1
        }
        firstF=true;                                                                                    //reset a todos los demás edos
        firstS=true;
        firstB=true;
        firstL=true;
        firstR=false;                                                                                   //edo de right cambiado
    }
}
void Stop(void){
    if(firstS){                                                                                         //evalua edo de stop
        GPIO_PORTA_AHB_DATA_R = 0x30;                                                                   //Pone 1 en A4 y A5
        GPIO_PORTB_AHB_DATA_R = 0x30;                                                                   //Pone 1 en B4 y B5
        TIMER0_TBMATCHR_R = 16000;                                                                      //Ciclo de trabajo A
        TIMER0_TAMATCHR_R = 16000;                                                                      //Ciclo de trabajo B
        SSD1306_Clear();                                                                                //Limpia pantalla
        SSD1306_Command(SSD1306_SETPAGE1);                                                              //Define pagina 1
        SSD1306_WriteString("ALTO");                                                                    //Envia dato
        firstF=true;                                                                                    //reset a los demás edos
        firstB=true;
        firstL=true;
        firstR=true;
        firstS=false;                                                                                   //cambia edo del stop
    }
}
//--------------------------------------------------------------------------------------------------Función de determina la dirección de desplazamiento dependiendo del valor obtenido en el ADCSumary--------------------------------------------------------------------------------------------------
void movementCase(int n, float Y){
    switch(n){
    case 10:                                                                                            //evalua si inst=10
        Forward(Y);                                                                                     //Llama a la función fordward
        break;
    case 5:                                                                                             //evalua si inst=5
        Backward(Y);                                                                                    //llama a la función backward
        break;
    case 8:                                                                                             //evalua si inst=8
        Rightward();                                                                                    //llama a la función rightward
        break;
    case 2:                                                                                             //evalua si inst=2
        Leftward();                                                                                     //llama a la función leftward
        break;
    case 4:                                                                                             //evalua si inst=4
        Stop();                                                                                         //llama a la función stop
        break;
    case 50:
        ind=1;
        break;
    default:                                                                                            //por dejecto no ejecuta ninguna función
        break;
    }
}
//--------------------------------------------------------------------------------------------------Interrupción del puerto C (asociado al ultrasónico)--------------------------------------------------------------------------------------------------//------------------------------------------------------------------------
void PortC_Handler(void){
    GPIO_PORTC_AHB_ICR_R = 0x20;                                                                    //Limpia la bandera
    cta=0;                                                                                          //Inicializa la cuenta a 0
       do{
           cta++;                                                                                   //Incrementa la cuenta mientras cumpla la condición
       }while(((GPIO_PORTC_AHB_DATA_R & 0x20)==0x20) & (cta  < 0x00003416));                        //mientras el eco no haya regresado o mientras la cuenta sea menor a una distancia equivalente a 400 cm
       D=0.03*cta;                                                                                 //Convierte la cuenta en su equivalente en distancia

       if(D<30){                                                                                    //Condición para deter el movimiento
           pot_setVal(255);                                                                         //enviamos el dato al potenciometro digital pasar voltaje
           for (i = 0; i < 20000; i++);                                                             //Tiempo en alto
           pot_setVal(0);                                                                           //Enviamos dato para que en potenciometro no deje pasar nada
           for (i = 0; i < 200000; i++);                                                            //tiempo en bajo
           if(firstI){                                                                              //Evalua si es la primera vez que entramos al ciclo de la interrupción
               GPIO_PORTA_AHB_DEN_R &= ~0x20;                                                       //Deshabilita A5
               GPIO_PORTB_AHB_DEN_R &= ~0x20;                                                       //Deshabilita B5
               SSD1306_Clear();                                                                     //Limpiamos pantalla
               SSD1306_Command(SSD1306_SETPAGE1);                                                   //Establecemos pagina
               SSD1306_WriteString("CUIDADO");                                                      //Enviamos texto
               firstI=false;                                                                        //Decimos que no es la primera vez que está en la interrupción
               firstO=true;                                                                         //Reiniciamos el valor del indicar de estar fuera de la interrupción
           }
       }
       else{
           if(firstO){                                                                              //Evalua en el indicador de estar fuera
               pot_setVal(0);                                                                       //Envía dato 0 al potenciometro
               SSD1306_Clear();                                                                     //Limpia la pantalla
               GPIO_PORTA_AHB_DEN_R |=0x20;                                                         //Habilita A5
               GPIO_PORTB_AHB_DEN_R |=0x20;                                                         //Habilita B5
               firstI=true;                                                                         //Resetea indicador inside
               firstO=false;                                                                        //Cambia indicador out
               firstS=true;
           }
       }
}
//--------------------------------------------------------------------------------------------------Interrupción del UART6--------------------------------------------------------------------------------------------------//--------------------------------------------------------------------------------------------------
int UART6_Handler(void){
        while((UART6_FR_R&UART_FR_RXFE) != 0);                                                      //Se espera a que el FIFO RX tenga un valor
        ind=0;                                                                                      //Cambia el estado del indicador
        data = UART6_DR_R&0xFF;                                                                     //guarda en valor de la fifo en una variable
        movementCase(data, 1.9);                                                                    //usa el valor de esa variable para enivarlo a la función de movimiento
        UART6_ICR_R = 0x00000010;                                                                   // Limpia bandera de interrupción de recepción
        return ind;
}


