#ifndef SSD1306_H_
#define SSD1306_H_

//REGISTROS DE RELOJ
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608)) //Reloj del puerto
#define SYSCTL_RCGCI2C_R        (*((volatile uint32_t *)0x400FE620)) //Reloj de I2C
#define SYSCTL_PRGPIO_R        (*((volatile uint32_t *)0x400FEA08)) //Bandera de "Peripherial Ready"

//REGISTROS DEL PUERTO B
#define GPIO_PORTB_AHB_DATA_R   (*((volatile uint32_t *)0x400593FC))
#define GPIO_PORTB_AHB_DIR_R    (*((volatile uint32_t *)0x40059400))
#define GPIO_PORTB_AHB_AFSEL_R  (*((volatile uint32_t *)0x40059420))
#define GPIO_PORTB_AHB_ODR_R    (*((volatile uint32_t *)0x4005950C))
#define GPIO_PORTB_AHB_DEN_R    (*((volatile uint32_t *)0x4005951C))
#define GPIO_PORTB_AHB_PCTL_R   (*((volatile uint32_t *)0x4005952C))
#define GPIO_PORTB_AHB_PUR_R    (*((volatile uint32_t *)0x40059510))

//REGISTROS DEL MÓDULO I2C
#define I2C0_MSA_R              (*((volatile uint32_t *)0x40020000)) //I2C Master Slave Adress
#define I2C0_MCS_R              (*((volatile uint32_t *)0x40020004)) //I2C Master Control Status
#define I2C0_MDR_R              (*((volatile uint32_t *)0x40020008)) //I2C Master Data Register
#define I2C0_MTPR_R             (*((volatile uint32_t *)0x4002000C)) //I2C Master Time Period
#define I2C0_MCR_R              (*((volatile uint32_t *)0x40020020)) //I2C Master Congirutation Register

//DEFINICION DE BITS DEL I2C Master Control Status
#define I2C_MCS_ACK 0x00000008 //Transmmitter Acknowledge Enable
#define I2C_MCS_DATACK 0x00000008 // Data Acknowledge Enable
#define I2C_MCS_ADRACK 0x00000004 // Acknowledge Address
#define I2C_MCS_STOP 0x00000004 // Generate STOP
#define I2C_MCS_START 0x00000002 // Generate START
#define I2C_MCS_ERROR 0x00000002 // Error
#define I2C_MCS_RUN 0x00000001 // I2C Master Enable
#define MAXRETRIES 5 // number of receive attempts before giving up
//DEFINICIONES DE SSD1306
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet
#define SSD1306_SETPAGE0 0XB0
#define SSD1306_SETPAGE1 0XB1
#define SSD1306_SETPAGE2 0XB2
#define SSD1306_SETPAGE3 0XB3
#define SSD1306_SCROLLINGL_ 0X27
#define SSD1306_SCROLLINGR_ 0X26
//SSD1306
int AdreSSD1306 = 0x3C;
int ControlbC = 0x00;
int ControlbD = 0x40;

/*El cÃ¡lculo del Time Period Register (TPR) se especifica en la pÃ¡gina 1284
 Asumiendo un reloj de 16 MHz y un modo de operaciÃ³n estÃ¡ndar (100 kbps):*/
int TPR = 1;
uint8_t error;
uint32_t i;
/*********************************************
        DECLARACION DE FUNCIONES
**********************************************/
void SSD1306_Init(void);
void I2C_Init(void);
void SSD1306_Command(int comand);
void SSD1306_Data(int dat);
void SSD1306_SCROLLINGL(void);
void SSD1306_WriteString(unsigned char *cadena);

//Letras
void Letra_A(void);
void Letra_B(void);
void Letra_C(void);
void Letra_D(void);
void Letra_E(void);
void Letra_F(void);
void Letra_G(void);
void Letra_H(void);
void Letra_I(void);
void Letra_J(void);
void Letra_K(void);
void Letra_L(void);
void Letra_M(void);
void Letra_O(void);
void Letra_P(void);
void Letra_Q(void);
void Letra_R(void);
void Letra_S(void);
void Letra_T(void);
void Letra_U(void);
void Letra_V(void);
void Letra_W(void);
void Letra_X(void);
void Letra_Y(void);
void Letra_Z(void);
void Espacio (void);
void lineaALta(void);
void lineaBaja(void);
void flechaFS(void);
void flechaF1(void);
void flechaF2(void);
void flechaB1(void);
void flechaB2(void);
void barra(void);
void flechaV1(void);
void flechaV2(void);
void SS(void);
//
void SSD1306_WriteChar(char C);
void SSD1306_WriteString(unsigned char *cadena);

/***************************************************************
        FUNCION QUE INICIALIZA LOS RELOJES, EL GPIO E I2C
***************************************************************/
void I2C_Init(void){
    //CONFIGURACIÃ“N DE LOS RELOJ
    SYSCTL_RCGCI2C_R |= 0x0001;                                                     // Activamos el reloj de I2C0
    while((SYSCTL_PRGPIO_R&0x0002) == 0){};                                         //Espero a que se active el reloj del puerto B

    GPIO_PORTB_AHB_DIR_R |= 0x0C;                                                   //Activo al PB2 (SCL) y al PB3 (SDA) como salidas
    GPIO_PORTB_AHB_DEN_R |= 0x0C;                                                   //Activo la función digital de PB3 y PB2
    GPIO_PORTB_AHB_AFSEL_R |= 0x0C;                                                 // Activo la función alterna del PB2 y PB3
    GPIO_PORTB_AHB_ODR_R |= 0x08;                                                   // Activo el OPEN DRAIN para el PB3, ya que el PB2 ya tiene uno por preconfig.
    GPIO_PORTB_AHB_PCTL_R|=0x00002200;

    //CONFIGURACIÓN DEL MODULO I2C0
    I2C0_MCR_R = 0x00000010;                                                        // Habilitar función MASTER para el I2C0
    I2C0_MTPR_R = TPR;                                                              // Se establece una velocidad estándar de 400kbps = 1/2(1+TPR)(6+4)Tclk ---- SCL_LP = 6 ; SCL_HP = 4
}
/***************************************************************
      FUNCION PARA INICIALIZAR SSD1306
***************************************************************/
void SSD1306_Init(void){

    for(i=0;i<400;i++){}
SSD1306_Command(SSD1306_DISPLAYOFF);
SSD1306_Command(SSD1306_SETMULTIPLEX);
SSD1306_Command(0x1F);
SSD1306_Command(SSD1306_SETDISPLAYOFFSET);
SSD1306_Command(0x00);
SSD1306_Command(SSD1306_SETSTARTLINE);
SSD1306_Command(SSD1306_SEGREMAP|0x1);
SSD1306_Command(SSD1306_COMSCANDEC);
SSD1306_Command(SSD1306_SETCOMPINS);
SSD1306_Command(0x02);
SSD1306_Command(SSD1306_SETCONTRAST);
SSD1306_Command(0x8F);
SSD1306_Command(SSD1306_DISPLAYALLON_RESUME);
SSD1306_Command(SSD1306_NORMALDISPLAY);
SSD1306_Command(SSD1306_SETDISPLAYCLOCKDIV);
SSD1306_Command(0x80);
SSD1306_Command(SSD1306_CHARGEPUMP);
SSD1306_Command(0x14);
SSD1306_Command(SSD1306_DISPLAYON);

}
/***************************************************************
      FUNCION ESPERAR, MONITOREO DE EL BIT BUSY
***************************************************************/
int esperar(){
    while(I2C0_MCS_R&0x00000001){};                                                 //Estado del control maestro: Espero a que la transmisión acabe
      if(I2C0_MCS_R&0x00000002==1){                                                 //Error?
          error=1;
          return error;
      };
      return 0;
}
/***************************************************************
      FUNCION COMANDO, RECIBE UN ENTERO
Se crea una trama de datos correspondiente a cada comando
***************************************************************/
void SSD1306_Command(int command){

    while(I2C0_MCS_R&0x00000001){};                                                 //i2C RADY?
    //Para Transmitir
    I2C0_MSA_R=(AdreSSD1306<<1)&0xFE;                                               //Cargo la dirección del DS1307 e indico que se va a transmitir
    I2C0_MDR_R=ControlbC&0x0FF;                                                     //Dato transmitido durante la transacción
    I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_START);                                         //Condición de START y corre

    for(i=0;i<300;i++){} //Delay
    esperar();

    I2C0_MDR_R=command;                                                             //Asignamos el dato
    I2C0_MCS_R=(I2C_MCS_STOP|I2C_MCS_RUN);                                          //Inicio la ultima transmisiónn y STOP
    for(i=0;i<300;i++){} //Delay
    esperar();
}
void SSD1306_Data(int dat){
    while(I2C0_MCS_R&0x00000001){};                                                 //i2C RADY?
        //Para Transmitir
        I2C0_MSA_R=(AdreSSD1306<<1)&0xFE;                                           //Cargo la dirección del DS1307 e indico que se va a transmitir
        I2C0_MDR_R=ControlbD&0x0FF;                                                 //Dato transmitido durante la transacción
        I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_START);                                     // Condición de START e iniciamos

        for(i=0;i<300;i++){}                                                        //Delay
        esperar();

        I2C0_MDR_R=dat;
        I2C0_MCS_R=(I2C_MCS_STOP|I2C_MCS_RUN);                                      //Inicio la ultima transmisión y detengo
        for(i=0;i<300;i++){}                                                        //Delay
        esperar();

}
//LETRAS
void Letra_A(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x33);
    SSD1306_Data(0x33);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
}
void Letra_B(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x99);
    SSD1306_Data(0x99);
    SSD1306_Data(0xFF);
    SSD1306_Data(0XFF);
    SSD1306_Data(0x00);
}
void Letra_C(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0XC3);
    SSD1306_Data(0x00);
}
void Letra_D(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xFF);
    SSD1306_Data(0XFF);
    SSD1306_Data(0x00);
}

void Letra_E(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xDB);
    SSD1306_Data(0x00);
}
void Letra_F(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x33);
    SSD1306_Data(0x33);
    SSD1306_Data(0x03);
    SSD1306_Data(0X03);
    SSD1306_Data(0x00);
}
void Letra_G(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xF8);
    SSD1306_Data(0xF8);
    SSD1306_Data(0x00);
}
void Letra_H(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x18);
    SSD1306_Data(0x18);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
}
void Letra_I(void){
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0x00);
}
void Letra_J(void){
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x00);
}
void Letra_K(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x18);
    SSD1306_Data(0x3C);
    SSD1306_Data(0x66);
    SSD1306_Data(0xC3);
    SSD1306_Data(0x00);
}

void Letra_L(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0x00);
}
void Letra_M(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x0C);
    SSD1306_Data(0x1C);
    SSD1306_Data(0x0C);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
    //6 julio

}
void Letra_N(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x38);
    SSD1306_Data(0x1C);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);

}
void Letra_O(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xC3);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
}
void Letra_P(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x1B);
    SSD1306_Data(0x1B);
    SSD1306_Data(0x1F);
    SSD1306_Data(0x1F);
    SSD1306_Data(0x00);
}
void Letra_Q(void){
    SSD1306_Data(0x7F);
    SSD1306_Data(0x7F);
    SSD1306_Data(0x63);
    SSD1306_Data(0x63);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
}
void Letra_R(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x1B);
    SSD1306_Data(0x7B);
    SSD1306_Data(0x6F);
    SSD1306_Data(0xEF);
    SSD1306_Data(0x00);
}
void Letra_S(void){
    SSD1306_Data(0xCF);
    SSD1306_Data(0xCF);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xDB);
    SSD1306_Data(0xF3);
    SSD1306_Data(0xF3);
    SSD1306_Data(0x00);
}
void Letra_T(void){
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x00);
}
void Letra_U(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
}
void Letra_V(void){

    SSD1306_Data(0x1F);
    SSD1306_Data(0x7F);
    SSD1306_Data(0xF0);
    SSD1306_Data(0xF0);
    SSD1306_Data(0x7F);
    SSD1306_Data(0x1F);
    SSD1306_Data(0x00);

}
void Letra_W(void){
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x70);
    SSD1306_Data(0x38);
    SSD1306_Data(0x70);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x00);
}
void Letra_X(void){
SSD1306_Data(0xC3);
SSD1306_Data(0xE7);
SSD1306_Data(0x3C);
SSD1306_Data(0x3C);
SSD1306_Data(0xE7);
SSD1306_Data(0xC3);
SSD1306_Data(0x00);
}
void Letra_Y(void){
    SSD1306_Data(0x03);
    SSD1306_Data(0x0F);
    SSD1306_Data(0xFC);
    SSD1306_Data(0xFC);
    SSD1306_Data(0x0F);
    SSD1306_Data(0x03);
    SSD1306_Data(0x00);
}

void Letra_Z(void){
    SSD1306_Data(0xC3);
    SSD1306_Data(0xE3);
    SSD1306_Data(0xFB);
    SSD1306_Data(0xDF);
    SSD1306_Data(0xC7);
    SSD1306_Data(0xC3);
    SSD1306_Data(0x00);
}

void Espacio (void){
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
}

void lineaAlta(void){
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
    SSD1306_Data(0x03);
}
void lineaBaja(void){
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xC0);
}
void flechaF1(void){
    SSD1306_Data(0xFE);
    SSD1306_Data(0xFC);
    SSD1306_Data(0xF8);
    SSD1306_Data(0xF0);
    SSD1306_Data(0xE0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0x80);
}
void flechaF2(void){
    SSD1306_Data(0x7F);
    SSD1306_Data(0x3F);
    SSD1306_Data(0x1F);
    SSD1306_Data(0x0F);
    SSD1306_Data(0x07);
    SSD1306_Data(0x03);
    SSD1306_Data(0x01);
}
void flechaB1(void){
    SSD1306_Data(0x80);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xE0);
    SSD1306_Data(0xF0);
    SSD1306_Data(0xF8);
    SSD1306_Data(0xFC);
    SSD1306_Data(0xFE);
}
void flechaB2(void){
    SSD1306_Data(0x01);
    SSD1306_Data(0x03);
    SSD1306_Data(0x07);
    SSD1306_Data(0x0F);
    SSD1306_Data(0x1F);
    SSD1306_Data(0x3F);
    SSD1306_Data(0x7F);
}
void barra(void){
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0x00);
    SSD1306_Data(0xFF);
    SSD1306_Data(0xFF);
}
void flechaV1(void){
    SSD1306_Data(0x01);
    SSD1306_Data(0x03);
    SSD1306_Data(0x07);
    SSD1306_Data(0x0F);
    SSD1306_Data(0x07);
    SSD1306_Data(0x03);
    SSD1306_Data(0x01);
}
void flechaV2(void){
    SSD1306_Data(0x80);
    SSD1306_Data(0xC0);
    SSD1306_Data(0xE0);
    SSD1306_Data(0xF0);
    SSD1306_Data(0xE0);
    SSD1306_Data(0xC0);
    SSD1306_Data(0x80);
}
void SS(void){
    SSD1306_Data(0x08);
    SSD1306_Data(0x06);
    SSD1306_Data(0x7F);
    SSD1306_Data(0xFF);
    SSD1306_Data(0x3F);
    SSD1306_Data(0x1E);
    SSD1306_Data(0x00);
}


void SSD1306_Clear(void){
    int c;
    SSD1306_Command(SSD1306_SETPAGE0);
for(c=0;c<128;c++){
    SSD1306_Data(0x00);
}
SSD1306_Command(SSD1306_SETPAGE1);
for(c=0;c<128;c++){
    SSD1306_Data(0x00);
}
SSD1306_Command(SSD1306_SETPAGE2);
for(c=0;c<128;c++){
    SSD1306_Data(0x00);
}
SSD1306_Command(SSD1306_SETPAGE3);
for(c=0;c<128;c++){
    SSD1306_Data(0x00);}
SSD1306_Command(SSD1306_SETPAGE0);
}


void SSD1306_SCROLLINGL(void){

SSD1306_Command(SSD1306_SCROLLINGL_);
SSD1306_Command(0x00);
SSD1306_Command(0x00);
SSD1306_Command(0x00);
SSD1306_Command(0x03);
SSD1306_Command(0x00);
SSD1306_Command(0xFF);
SSD1306_Command(0x2F);


}


void SSD1306_SCROLLINGR(void){
SSD1306_Command(SSD1306_SCROLLINGR_);
SSD1306_Command(0x00);
SSD1306_Command(0x00);
SSD1306_Command(0x00);
SSD1306_Command(0x03);
SSD1306_Command(0x00);
SSD1306_Command(0xFF);
SSD1306_Command(0x2F);
}

void SSD1306_WriteChar(char C){

    switch (C){
        case 'A':
            Letra_A();
            break;
        case 'B':
            Letra_B();
            break;
        case 'C':
            Letra_C();
            break;
        case 'D':
            Letra_D();
            break;
        case 'E':
            Letra_E();
            break;
        case 'F':
            Letra_F();
            break;
        case 'G':
            Letra_G();
            break;
        case 'H':
            Letra_H();
            break;
        case 'I':
            Letra_I();
            break;
        case 'J':
            Letra_J();
            break;
        case 'K':
            Letra_K();
            break;
        case 'L':
            Letra_L();
            break;
        case 'M':
            Letra_M();
            break;
        case 'N':
            Letra_N();
            break;
        case 'O':
            Letra_O();
            break;
        case 'P':
            Letra_P();
            break;
        case 'Q':
            Letra_Q();
            break;
        case 'R':
            Letra_R();
            break;
        case 'S':
            Letra_S();
            break;
        case 'T':
            Letra_T();
            break;
        case 'U':
            Letra_U();
            break;
        case 'V':
            Letra_V();
            break;
        case 'X':
            Letra_X();
            break;
        case 'Y':
            Letra_Y();
            break;
        case 'Z':
            Letra_Z();
            break;
        case '-':
            lineaAlta();
            break;
        case ')':
            flechaF2();
            break;
        case '(':
            flechaF1();
            break;
        case '_':
            lineaBaja();
            break;
        case '[':
            flechaB1();
            break;
        case ']':
            flechaB2();
            break;
        case '|':
            barra();
            break;
        case '^':
            flechaV1();
            break;
        case '*':
            flechaV2();
            break;
        case '+':
            SS();
            break;
        case ' ':
            Espacio();
            break;
    }

}



void SSD1306_WriteString(unsigned char *cadena)
{
    unsigned int i = 0;
    unsigned int l = 0;

    l = cadena[i];
    while ((l) != 0)
    {
SSD1306_WriteChar(l);
        i++;
        l = cadena[i];
    }
}
/*********************************************************************
LIESE           LIESE           LIESE           LIESE           LIESE
***********************************************************************/

#endif /* SSD1306_H_ */
