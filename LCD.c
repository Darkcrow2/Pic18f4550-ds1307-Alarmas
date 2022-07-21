#include <LCD.h>
#include <LCD_ports.c>
#include <TECLADO4X4.c>
#include <ds1307_12.c>

#use delay(clock=8000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#use i2c(Master,Fast,sda=PIN_B0,scl=PIN_B1)

char letra, tecla, time;
char flecha[8]= {0b00000, 0b00100, 0b00010, 0b11111, 0b00010, 0b00100, 0b00000, 0b00000};
unsigned int8 sec, min, hrs, day, mth, year, dow, formato, contador, contador1, pantalla, i, tiempo; 
unsigned int1 am_pm, completo; 

void casos();
void teclado();
void reloj();
void menu();
void submenu();
void subreloj();
void configfecha();
void subalarma();
void arriba();
void abajo();
void izquierda();
void derecha();
void configreloj();

#INT_TIMER0                   //habilito la interrupcion del timer0
void TIMER0_isr()             //declaro lo que haré al momento de la interrupcion del timer
{
   if(tiempo == 1)
   {
   tiempo = 2;
   }
   else if(tiempo == 3)
   {
   tiempo = 0;
   }
}

//////////////////////////////////////////////////////////////////////////////
void main()
{
   setup_adc_ports(AN0|VSS_VDD);
   setup_adc(ADC_OFF);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_oscillator(OSC_8MHZ|OSC_INTRC|OSC_31250|OSC_PLL_OFF);
   enable_interrupts(GLOBAL);
   
   set_tris_A(0b00000000);
   set_tris_B(0b00000011);
   set_tris_C(0b00000000);
   set_tris_D(0b00001111);
   set_tris_E(0b00000000);
   
   contador = 0;
   contador1 = 0;
   i = 0;
   tiempo = 0;
   letra = 0;
   tecla = 0;
   formato = 0;
   pantalla = 0;

   ds1307_init(); 
   ds1307_set_date_time_completo(1,1,1,1,15,59,59); //formato 24 horas
   lcd_init(); 
   
   while(1)
   {
      teclado();
   }
}

////////////////////////////////////////////////////////////////////////////////
void teclado()
{
   switch(pantalla)
   {
   case 0:
   reloj();
   break;
   
   case 1:
   menu();
   break;
   
   case 2:
   submenu();
   break;
   }
   
   tecla = kbd_getc();
   
   if( (tecla==0) && (letra!=0) )
   {
   casos();
   }
   else if (tecla != 0 && letra == 0)             //si tiene dato K
   {
   letra = tecla;
   }  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void menu()
{
   lcd_send_byte(0, 0x40);
   for(i = 0; i < 8; i++)
   {
   lcd_send_byte(1, flecha[i]);
   }

   switch(contador)
      {
      case 0:  //CONFIGURAR HORA
      lcd_gotoxy(1,2);
      lcd_send_byte(1,0);
      lcd_gotoxy(7,1);
      printf(lcd_putc,"MENU");
      lcd_gotoxy(2,2);
      printf(lcd_putc, "Config. hora");
      break;
 
      case 1:  //CONFIGURAR FECHA
      lcd_gotoxy(1,2);
      lcd_send_byte(1,0);
      lcd_gotoxy(2,1);
      printf(lcd_putc,"Config. hora");
      lcd_gotoxy(2,2);
      printf(lcd_putc, "Config. fecha");
      break;
 
      case 2:  //CONFIGURAR ALARMAS
      lcd_gotoxy(1,2);
      lcd_send_byte(1,0);
      lcd_gotoxy(2,1);
      printf(lcd_putc,"Config. fecha");
      lcd_gotoxy(2,2);
      printf(lcd_putc, "Config. alarmas");
      break;
 
      case 3:  //SALIR
      lcd_gotoxy(1,2);
      lcd_send_byte(1,0);
      lcd_gotoxy(2,1);
      printf(lcd_putc,"Config. alarmas");
      lcd_gotoxy(2,2);
      printf(lcd_putc, "Salir");
      break;
      }
}

////////////////////////////////////////////////////////////////////////////////
void submenu()
{     

   lcd_send_byte(0, 0x40);

   switch(contador1)
   {
      case 0:
      subreloj();
      break;
      
      case 1:
      configfecha();
      break;
      
      case 2:
      subalarma();
      break;
      
      case 3:
      configreloj();
      break;
      
      case 4:
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Desea salir?");
      lcd_gotoxy(1,2);
      printf(lcd_putc, "* = Si");
      lcd_gotoxy(11,2);
      printf(lcd_putc, "# = No");
      break;
      
      case 5:
      lcd_gotoxy(1,1);
      printf(lcd_putc, "Hora correcta?");
         if(formato == 0)
         {
         lcd_gotoxy(6,2);
         printf(lcd_putc,"%02d:\%02d", hrs,min);
         }
         else
         {
         lcd_gotoxy(5,2);
         printf(lcd_putc,"%02d:\%02d \%cM", hrs,min,time);
         }
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void subreloj()
{
   if(contador == 0)
   {
   lcd_gotoxy(1,2);
   printf(lcd_putc, " ");
   lcd_gotoxy(1,1);
   }
   else
   {
   lcd_gotoxy(1,1);
   printf(lcd_putc, " ");
   lcd_gotoxy(1,2);
   }
   lcd_send_byte(1,0);
   lcd_gotoxy(2,1);
   if(formato == 0)
   {
   printf(lcd_putc, "Formato 24horas");
   }
   else
   {
   printf(lcd_putc, "Formato 12horas");
   }
   lcd_gotoxy(2,2);
   printf(lcd_putc, "Establecer hora");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void configreloj()
{  
   lcd_gotoxy(2,1);
   printf(lcd_putc, "Modificar hora");
   if(formato == 0)
   {
      if(i == 0)
      {
      ds1307_get_time_completo(hrs,min,sec);
      i = 1;
      }

      if(tiempo == 0)
      {
      lcd_gotoxy(6,2);
      printf(lcd_putc,"%02d:\%02d", hrs,min);
      setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32);   //la interrupcion es algo externo y no debe de ir en el ciclo while
      set_timer0(28035);
      enable_interrupts(INT_TIMER0);
      tiempo = 1;
      }
      else if(tiempo == 2)
      {
      lcd_gotoxy( (3*contador) + 6,2);
      printf(lcd_putc,"  ");
      setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32);   //la interrupcion es algo externo y no debe de ir en el ciclo while
      set_timer0(46785);
      enable_interrupts(INT_TIMER0);
      tiempo = 3;
      }
   }
   else
   {
      if(i == 0)
      {
      ds1307_get_time_medio(hrs,am_pm,min,sec);
      
         if(am_pm)
         {
         time='P';
         }
         else
         { 
         time='A';
         }
         
      i = 1;
      }
      
      if(tiempo == 0)
      {
      lcd_gotoxy(5,2);
      printf(lcd_putc,"%02d:\%02d \%cM", hrs,min,time);
      setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32);   //la interrupcion es algo externo y no debe de ir en el ciclo while
      set_timer0(28035);
      enable_interrupts(INT_TIMER0);
      tiempo = 1;
      }
      else if(tiempo == 2)
      {
      lcd_gotoxy( (3*contador) + 5,2);
      printf(lcd_putc,"  ");
      setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32);   //la interrupcion es algo externo y no debe de ir en el ciclo while
      set_timer0(46785);
      enable_interrupts(INT_TIMER0);
      tiempo = 3;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configfecha()
{
   if(i == 0)
   {
   ds1307_get_date(day, mth, year, dow);
   i = 1;
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Establecer fecha");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"%02d/\%02d/\%02d \%02d", day,mth,year,dow);
   }
   
}

///////////////////////////////////////////////////////////////////////////////////////////////
void subalarma()
{
}

////////////////////////////////////////////////////////////////////////////////////////////
void casos()
{
   switch(letra)
   {
   case '0':
   if(pantalla == 2 && contador1 == 3 && contador == 0)
   {
      if(i == 2)
      {
      hrs = 0;
      i = 3;
      }
      
      hrs = hrs*10;
      if(hrs > 23)
      {
      hrs = 0;
      hrs = hrs*10;
      }
   }
   else if(pantalla == 2 && contador1 == 3 && contador == 1)
   {
      if(i == 2)
      {
      min = 0;
      i = 3;
      }

      min = min*10;
      if(min > 59)
      {
      min = 0;
      min = min*10;
      }
   }
   letra = 0;
   break;
   
   case '1':
   if(pantalla == 2 && contador1 == 3 && contador == 0)
   {
      if(i == 2)
      {
      hrs = 0;
      i = 3;
      }
      
      hrs = hrs*10 + 1;
      if(hrs > 23)
      {
      hrs = 0;
      hrs = hrs*10 + 1;
      }
   }
   else if(pantalla == 2 && contador1 == 3 && contador == 1)
   {
      if(i == 2)
      {
      min = 0;
      i = 3;
      }
      
      min = min*10 + 1;
      if(min > 59)
      {
      min = 0;
      min = min*10 + 1;
      }   
   }
   letra = 0;
   break;
   
   case '2':
   if(pantalla == 2 && contador1 == 3 && contador == 0)
   {
      if(i == 2)
      {
      hrs = 0;
      i = 3;
      }
      
      hrs = hrs*10 + 2;
      if(hrs > 23)
      {
      hrs = 0;
      hrs = hrs*10 + 2;
      }
   }
   else if(pantalla == 2 && contador1 == 3 && contador == 1)
   {
      if(i == 2)
      {
      min = 0;
      i = 3;
      }
      
      min = min*10 + 2;
      if(min > 59)
      {
      min = 0;
      min = min*10 + 2;
      }   
   }
   letra = 0;
   break;
   
   case '3':
   if(pantalla == 2 && contador1 == 3 && contador == 0)
   {
      if(i == 2)
      {
      hrs = 0;
      i = 3;
      }
      
      hrs = hrs*10 + 3;
      if(hrs > 23)
      {
      hrs = 0;
      hrs = hrs*10 + 3;
      }
   }
   else if(pantalla == 2 && contador1 == 3 && contador == 1)
   {
      if(i == 2)
      {
      min = 0;
      i = 3;
      }
      
      min = min*10 + 3;
      if(min > 59)
      {
      min = 0;
      min = min*10 + 3;
      }   
   }
   letra = 0;
   break;
   
   case '4':
   letra = 0;
   break;
   
   case '5':
   letra = 0;
   break;
   
   case '6':
   letra = 0;
   break;
   
   case '7':
   letra = 0;
   break;
   
   case '8':
   letra = 0;
   break;
   
   case '9':
   letra = 0;
   break;
   
   case 'A':
   letra = 0;
   arriba();
   break;
         
   case 'B':
   letra = 0;
   abajo();
   break;
   
   case 'C':
   letra = 0;
   izquierda();
   break;
   
   case 'D':
   letra = 0;
   derecha();
   break;
         
   case '*':
   letra = 0;
   if(contador == 0 && pantalla == 1)  //ENTRA A SUBRELOJ
   {
   printf(lcd_putc, "\f");
   pantalla = 2;
   contador1 = 0;
   }
   else if(contador == 1 && pantalla == 1)   //ENTRA A CONFIGFECHA
   {
   printf(lcd_putc, "\f");
   pantalla = 2;
   contador1 = 1;
   }
   else if(contador == 2 && pantalla == 1)   //ENTRA A CONFIGALARMA
   {
   printf(lcd_putc, "\f");
   pantalla = 2;
   contador1 = 2;
   }
   else if(contador == 3 && pantalla == 1)  //ENTRA A MENU DE SALIR
   {
   printf(lcd_putc, "\f");
   pantalla = 2;
   contador1 = 4;
   }
   else if(contador1 == 0 && pantalla == 2)
   {
      if(contador == 0)
      {
         if(formato == 0)
         {
         ds1307_get_time_formato_medio(hrs, am_pm);
         ds1307_set_time_formato_medio(hrs,am_pm);
         formato = 1;
         }
         else
         {
         ds1307_get_time_formato_completo(hrs, completo);
         ds1307_set_time_formato_completo(hrs, completo);
         formato = 0;
         }
      }
      else
      {
         printf(lcd_putc, "\f");
         i = 0;
         tiempo = 0;
         contador1 = 3;
         contador = 0;
      }
   }
   else if(contador1 == 3 && pantalla == 2)
   {
   printf(lcd_putc, "\f");
   contador1 = 5;
   contador = 0;
   disable_interrupts(INT_TIMER0);
   }
   else if(contador1 == 4 && pantalla == 2) ///Regreso a pantalla de reloj
   {
   printf(lcd_putc, "\f");
   pantalla = 0;
   contador = 0;
   }
   break;
         
   case '#':
   letra = 0;
   if(contador1 == 0 && pantalla == 2)
   {
   printf(lcd_putc, "\f");
   pantalla = 1;
   contador = contador1;
   }
   else if(contador1 == 1 && pantalla == 2)
   {
   printf(lcd_putc, "\f");
   pantalla = 1;
   contador = contador1;
   }
   else if(contador1 == 2 && pantalla == 2)
   {
   printf(lcd_putc, "\f");
   pantalla = 1;
   contador = contador1;
   }
   else if(contador1 == 3 && pantalla == 2)
   {
   printf(lcd_putc, "\f");
   contador1 = 0;
   contador = 1;
   disable_interrupts(INT_TIMER0);
   }
   else if(contador1 == 4 && pantalla == 2)
   {
   printf(lcd_putc, "\f");
   pantalla = 1;
   contador = 3;
   }
   break;
       
   default:
   letra = 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
void arriba()
{
   switch(pantalla)
   {
   case 1:
   if(contador == 0)
   {
   contador = 0;
   }
   else
   {
   printf(lcd_putc,"\f");
   contador --;
   }
   break;
         
   case 2:
   if(contador == 0)
   {
   contador = 0;
   }
   else
   {
   contador --;
   }
   break;
   }
}

//////////////////////////////////////////////////////////////////////////////////
void abajo()
{
   switch(pantalla)
   {
   case 1:
   if(contador == 3)
   {
   contador = 3;
   }
   else
   {
   printf(lcd_putc,"\f");
   contador ++;
   }
   break;
       
   case 2:
   if(contador == 1)
   {
   contador = 1;
   }
   else
   {
   contador ++;
   }
   break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void izquierda()
{
   switch(pantalla)
   {
   case 2:
   if(contador1 == 3)
   {
      if(formato == 0 || formato == 1)
      {
         if(contador == 0)
         {
         contador = 0;
         }
         else
         {
         i = 2;
         contador --;
         }
      }
   }
   break;
   }
}

////////////////////////////////////////////////////////////////////////////////////////////
void derecha()
{
   switch(pantalla)
   {
   case 0:
   pantalla = 1;
   printf(lcd_putc,"\f");
   break;
   
   case 2:
   if(contador1 == 3)
   {
      if(formato == 0)
      {
         if(contador == 1)
         {
         contador = 1;
         }
         else
         {
         i = 2;
         contador ++;
         }
      }
      else
      {
         if(contador == 2)
         {
         contador = 2;
         }
         else
         {
         i = 2;
         contador ++;
         }
      }
   }
   break;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void reloj()
{
   if(formato == 0)
   {
   lcd_gotoxy(1,1);
   ds1307_get_time_completo(hrs,min,sec);
   ds1307_get_date(day, mth, year, dow);
   printf(lcd_putc,"%02d:\%02d:\%02d   ", hrs,min,sec);
   lcd_gotoxy(1,2);
   printf(lcd_putc,"%02d/\%02d/\%02d \%02d", day,mth,year,dow);
   }
   else
   {
   ds1307_get_time_medio(hrs,am_pm,min,sec);
   ds1307_get_date(day, mth, year, dow);
   if(am_pm)
   {
   time='P';
   }
   else
   { 
   time='A';
   }
   lcd_gotoxy(1,1);
   printf(lcd_putc,"%02d:\%02d:\%02d \%cM", hrs,min,sec,time);
   lcd_gotoxy(1,2);
   printf(lcd_putc,"%02d/\%02d/\%02d \%02d", day,mth,year,dow);
   }
}
