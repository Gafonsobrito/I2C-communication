#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"

int main(void)
{

//Variáveis para armazernar a leitura do i2c
	unsigned char i2c_reg1 = 0;
	unsigned char i2c_reg2 = 0;

	DDRC = (1 << 4) | (1 << 5); //Habilita SDA(4) e SCL(5) como saída
	TWSR |= 0b00000001; //Habilita o prescaler = 4

	TWBR = 18; //Fator de divisão para geração de 100khz
	TWCR = 0b00000100; //Habilita o módulo TWI (TWEN = 1)

	UART_config(); //Configura a comunicação UART com 9600bps
    
//ESCRITA NO DISPOSITIVO

	TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN)); //Envia a condição de start
	while (!(TWCR & (1 << TWINT))); //Aguarda TWINT(interrupção) ser setado, confirmação do escravo

//Envia o endereço (0000 000)e o bit de R/W(0 - W)
	TWDR = 0x40 ; //0b0100 000 0

	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT))); 

//Envia o dado do registrador que será utilizado
	TWDR = 0x06 ; //Registrador 6 para configurar as portas

	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Envia o dado para o primeiro registrador
	TWDR = 0x01 ; //Só IO0.0 = entrada

	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Envia o dado para o segundo registrador
	TWDR = 0x00 ; //IO1 = Saída

	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Envia uma condição de STOP
	TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));

	_delay_ms(100);
    
//LEITURA DO DISPOSITIVO

//Envia a condição de START
	TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN)); //Condição de start
	while (!(TWCR & (1 << TWINT))); //Aguarda confirmação

//Envia o endereço do escravo com o bit R/W em 0(W - Escrita)
	TWDR = 0x40 ; //0b0100 000 0
	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Envia o endereço do registrador que será lido
	TWDR = 0x00 ;
	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Envia um Re-Start (START REPEATED)
	TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Envia o endereço com o bit RW em 1 (R - Leitura))
	TWDR = 0x41 ; //0b0100 000 1
	TWCR = ((1 << TWINT) | (1 << TWEN));
	while (!(TWCR & (1 << TWINT)));

//Confirmação do recebimento do primeiro byte
	TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWEA)); //TWEA - confirmação do mestre
    while (!(TWCR & (1 << TWINT)));
	i2c_reg1 = TWDR; //Armazena o primeiro byte (IO0)

//Aguarda a confirmação do recebimento do segundo byte
	TWCR = ((1 << TWINT) | (1 << TWEN) | (0 << TWEA)); //TWEA notack - para de receber informação
	while (!(TWCR & (1 << TWINT)));
	i2c_reg2 = TWDR; //Armazena o segundo byte (IO1)

//Envia a condição de STOP
	TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));

	UART_enviaCaractere(i2c_reg1);
	UART_enviaCaractere(i2c_reg2);
	while (1)
	{
	}
}