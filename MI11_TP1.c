#include <stdint.h>
#define PORT_A_BASE_ADDRESS (0x0021C000)
#define PORT_B_BASE_ADDRESS (0x0021C100)
#define PORT_C_BASE_ADDRESS (0x0021C200)
#define PORT_D_BASE_ADDRESS (0x0021C300)

typedef enum {PORT_A=0, PORT_B, PORT_C, PORT_D} PORT_X;
typedef enum {INPUT=0, OUTPUT=1} DIRECTION;
typedef enum {LOW=0, HIGH=1} VALUE;

typedef
  struct GpioConfigRegisters {
    uint32_t DDIR;        // Data Direction
    uint32_t OCR1;        // Output configuration pins 0 à 15
    uint32_t OCR2;        // Output configuration 16 à 31
    uint32_t ICONFA1;     // Input configuration A1
    uint32_t ICONFA2;     // Input configuration A2
    uint32_t ICONFB1;     // Input configuration B1
    uint32_t ICONFB2;     // Input configuration B2
    uint32_t DR;          // Data register => écrire les sorties
    uint32_t GIUS;        // GPIO In Use (map des gpio utilisées par le soft)
    uint32_t SSR;         // Sample Status => lire les entrées
    uint32_t ICR1;        // Interrupt configuration 1
    uint32_t ICR2;        // Interrupt configuration 2
    uint32_t IMR;         // Interrupt Mark
    uint32_t ISR;         // Interrupt Status
    uint32_t GPR;         // General Purpose
    uint32_t SWR;         // Software reset
    uint32_t PUEN;        // Pull up enable register
  }
GpioConfigRegisters_t;

void usePin(PORT_X port, uint32_t GPIO_N, DIRECTION direction)
{
  if (GPIO_N > 31)
    return;

  GpioConfigRegisters_t* PortConfig = (GpioConfigRegisters_t*)(PORT_A_BASE_ADDRESS + port*0x100);

  PortConfig->GIUS |= (1<<GPIO_N);
  PortConfig->DDIR &= ~(direction<<GPIO_N);
  PortConfig->DDIR |= (direction<<GPIO_N);


  uint32_t* RegisterToUse = &(PortConfig->OCR1) + (GPIO_N>=16);
  (*RegisterToUse) &= (0b11<<((GPIO_N%16)*2));

  if(direction==OUTPUT)
    *RegisterToUse |= (0b11<<((GPIO_N%16)*2));

}

void setPinOutput(PORT_X port, uint32_t GPIO_N, VALUE value)
{
  if (GPIO_N > 31)
    return;

  GpioConfigRegisters_t* PortConfig = (GpioConfigRegisters_t*)(PORT_A_BASE_ADDRESS + port*0x100);

  PortConfig->DR &= ~(value<<GPIO_N);
  PortConfig->DR |= (value<<GPIO_N);
}

VALUE readPinInput(PORT_X port, uint32_t GPIO_N)
{
  if (GPIO_N > 31)
    return;

  GpioConfigRegisters_t* PortConfig = (GpioConfigRegisters_t*)(PORT_A_BASE_ADDRESS + port*0x100);

  return ((PortConfig->SSR)>>GPIO_N) & 1;
}
