#define PORTD_BASE_ADDRESS (0x0021C300)

#define DDIR_D (PORTD_BASE_ADDRESS + 0)
#define OCR2_D (PORTD_BASE_ADDRESS + 0x08)
#define DR_D (PORTD_BASE_ADDRESS + 0x1C)
#define GIUS_D (PORTD_BASE_ADDRESS + 0x20)

#define TCTL1 (0x202000)
#define TPRER1 (0x202004)
#define TCMP1 (0x202008)
#define TCN1 (0x202010)
#define TSTAT1 (0x202014)

#define INTENNUM (0x00223008)

#define IRQEn (0b10000)
#define TimerEn (1)
#define Clk_Source_32k (0b1000)
#define Clk_Source_PERCLK1_16 (0b0100)


#define irq_enable()           \
asm volatile(               \
"mrs    r0,cpsr\t\n"     \
"bic    r0,r0,#0x80\t\n" \
"msr    cpsr_c,r0\t\n"   \
:                        \
:                        \
: "r0")

#define irq_disable()          \
asm volatile(               \
"mrs    r0,cpsr\t\n"     \
"orr    r0,r0,#0x80\t\n" \
"msr    cpsr_c,r0\t\n"   \
:                        \
:                        \
: "r0")





#define Allumer (*((unsigned int*)DR_D) &= ~0x80000000)
#define Eteindre (*((unsigned int*)DR_D) |= 0x80000000)


unsigned int allumee=0;

//char *s = "Salut";


void init_Timer(void)
{
	// Activer l'écoute du timer dans l'AITC
	*((unsigned int*)INTENNUM) = 0x3Bu;
	// Activer les interruptions IRQ dans le processeur
	irq_enable();

	// Initialisation du timer à 0
	*((unsigned int*)TCN1) = 0;
	// Prédiviseur = 1 : pas de prédiviseur
	*((unsigned int*)TPRER1) = 0;
	// Valeur limite = fréquence => Période=1s
	*((unsigned int*)TCMP1) = 32000;
	// Définition de la clock source + IRQEnable
	*((unsigned int*)TCTL1) = Clk_Source_32k|IRQEn;
	// Démarrage du timer
	*((unsigned int*)TCTL1) |= TimerEn;
}

void __attribute__((interrupt("IRQ"))) IRQ_Handler(void)
{
	//if(*((unsigned int*)TSTAT1)&1)
	//{
		if(!allumee)
			*((unsigned int*)DR_D) &= ~0x80000000; //Allumer
		else
			*((unsigned int*)DR_D) |= 0x80000000; //Eteindre

		allumee = !allumee;
		*((unsigned int*)TSTAT1) &= ~1; //Clear le flag
	//}

}


int main(void){
//  c=42;
//  a = a + b + c + s[1] + f();



	init_Timer();

	*((unsigned int*)GIUS_D) |= 0x80000000;
	*((unsigned int*)OCR2_D) |= 0xC0000000;
	*((unsigned int*)DDIR_D) |= 0x80000000;

//	while(1){
//		*((unsigned int*)DR_D) &= ~0x80000000;	//Allumer
//		for(i=0;i<1000000;i++);
//		*((unsigned int*)DR_D) |= 0x80000000;		//Eteindre
//		for(i=0;i<1000000;i++);
//	}

	while(1);
//	{
//		if(*((unsigned int*)TSTAT1) & 1)
//		{
//			if(allumee)
//				*((unsigned int*)DR_D) |= 0x80000000; //Eteindre
//			else
//				*((unsigned int*)DR_D) &= ~0x80000000; //Allumer
//			allumee=!allumee;
//			*((unsigned int*)TSTAT1) =0; //Clear le flag
//		}
//	}
	return 1;
} ;
