#include <main.h>

#define LCD_ENABLE_PIN PIN_E0
#define LCD_RS_PIN PIN_E1
#define LCD_RW_PIN PIN_E2
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
#define ADDEEPROM 0x01

#include <lcd.c>

int1 turnLed = 0;
unsigned int8 fixCounter;

#INT_EXT
void ext_isr(void){
  clear_interrupt(INT_EXT);  // Clear external interrupt flag bit
  turnLed = !turnLed;
}

void main()
{
   setup_timer_2(T2_DIV_BY_16, 155, 1);
   setup_ccp1(CCP_PWM);
//!   set_pwm1_duty((int16)312);
   
   SETUP_ADC(ADC_CLOCK_DIV_2);
   SETUP_ADC_PORTS(AN0);
   lcd_init();
   
   setup_timer_0(T0_EXT_L_TO_H|T0_DIV_1);
   
   if (read_eeprom(ADDEEPROM) > 10) {
      fixCounter = 0;
   } else {
      fixCounter = read_eeprom(ADDEEPROM);
   }
   set_timer0(fixCounter);

   lcd_putc("Loading...");
   
   ext_int_edge(H_TO_L);
   clear_interrupt(INT_EXT); // Clear external interrupt flag bit
   enable_interrupts(INT_EXT); // Enable external interrupt
   enable_interrupts(GLOBAL);  
   
   int16 counter = 0;
   
   while(TRUE)
   {
      delay_ms(100);
      
      SET_ADC_CHANNEL(0);
      delay_us(10);
      
      signed int8 temp = read_adc() / 2.046;
      fixCounter = get_timer0();
      write_eeprom(ADDEEPROM, fixCounter);
      
      lcd_putc("\f");
      lcd_gotoxy(1, 1);
      printf(lcd_putc, "Nhiet do: %d", temp);
      
      lcd_gotoxy(1, 2);
      if (fixCounter > 10) {
         lcd_putc("! CAN BAO TRI !");
      } else {
         printf(lcd_putc, "%02U", fixCounter);
      }
      
      if(temp > 50 || turnLed){
         output_high(pin_D0);
         while(counter < 624){
         set_pwm1_duty(counter);
         counter++;
         delay_ms(1);
         }
         delay_ms(100);
         while(counter > 0){
            set_pwm1_duty(counter);
            counter--;
            delay_ms(1);
         }
         delay_ms(100);
      }else{
         output_low(pin_D0);
      }
            
      
//!      delay_ms(200);
      //TODO: User Code
   }

}
