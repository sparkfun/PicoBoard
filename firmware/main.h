//*******************************************************
//					GPIO Definitions
#define SLIDER PINC0
#define RD	PINC1
#define RC	PINC2
#define SOUND	PINC3
#define RB	PINC4
#define RA	PINC5
#define LIGHT	PINC6

#define RXI	PIND0
#define TXO	PIND1
#define BUTTON PIND2


//*******************************************************
//						Macros
//*******************************************************
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))


//*******************************************************
//					General Definitions
//*******************************************************
#define MYUBRR 51	//Used to set the AVR Baud Rate TO 38400 (External 16MHz Oscillator)

#define SCRATCH_DATA_REQUEST	0x01
#define SCRATCH_FW_VER			0x04

//=======================================================
//					Function Definitions
//=======================================================
static int uart_putchar(char c, FILE *stream);
uint8_t uart_getchar(void);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
void ioinit(void);
void delay_ms(uint16_t x);
void delay_us(uint16_t x);
void buildScratchPacket(char * packet, int channel, int value);
void sendScratchPacket(char * packet);
