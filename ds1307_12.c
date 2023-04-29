/////////////////////////////////////////////////////////////////////////////////////// 
///                                   DS1307_12.h                                   /// 
///                     Modified Driver for Real Time Clock DS1307                  /// 
///                             12 hour(AM PM) format                               /// 
///                                                                                 /// 
/// ds1307_init() - Enable oscillator without clearing the seconds register         /// 
///                 used when PIC loses power and DS1307 run from 3V BAT            /// 
///               - Disable squarewave output                                       /// 
///                                                                                 /// 
/// ds1307_set_date_time(day,mth,year,dow,hour,am_pm,min,sec)  Set the date/time    /// 
///                                                                                 /// 
/// ds1307_get_date(day,mth,year,dow)               Get the date                    /// 
///                                                                                 /// 
/// ds1307_get_time(hr,am_pm,min,sec)                     Get the time              /// 
///                                                                                 /// 
///  ****** am_pm returns                                                           /// 
///         1 for PM and 0 for AM                                                   /// 
///                                                                                 /// 
/// Developed by Christian Rebogio                                                  /// 
///   suggestions and request please email me at                                    /// 
///   christian.rebogio@gmail.com                                                   /// 
/// credits to http://www.ccsinfo.com/forum/viewtopic.php?t=23255                   /// 
/////////////////////////////////////////////////////////////////////////////////////// 


#define RTC_SDA  PIN_B0 
#define RTC_SCL  PIN_B1 

#use i2c(master, sda=RTC_SDA, scl=RTC_SCL,FORCE_SW) 

unsigned int8 bin2bcd(unsigned int8 binary_value); 
unsigned int8 bcd2bin(unsigned int8 bcd_value); 

void ds1307_init() 
{ 
   unsigned int8 sec = 0; 
   unsigned int8 hrs= 0; 
  
   i2c_start(); 
   i2c_write(0xD0); 
   i2c_write(0x00); 
   i2c_start(); 
   i2c_write(0xD1); 
   sec = i2c_read(0); 
   i2c_stop(); 
   bit_clear(sec,7); 

   i2c_start(); 
   i2c_write(0xD0); 
   i2c_write(0x00);  
   i2c_write(sec);    
   i2c_start(); 
   i2c_write(0xD0); 
   i2c_write(0x07);      
   i2c_write(0x80);      
   i2c_stop(); 
}

////////////////////////////////////////////////////////////////MODIFICO PARA 24 HORAS//////////////////////////////////////////////////////////////////////
void ds1307_set_date_time_completo(unsigned int8 day, unsigned int8 mth, unsigned int8 year, unsigned int8 dow, unsigned int8 hr, unsigned int8 min, unsigned int8 sec) 
{ 
  sec &= 0x7F; 
  hr &= 0x3F;
  i2c_start(); 
  i2c_write(0xD0);          
  i2c_write(0x00);          
  i2c_write(bin2bcd(sec));    
  i2c_write(bin2bcd(min));  
  i2c_write(bin2bcd(hr));  
  i2c_write(bin2bcd(dow));    
  i2c_write(bin2bcd(day));  
  i2c_write(bin2bcd(mth));    
  i2c_write(bin2bcd(year));      
  i2c_write(0x80);        
  i2c_stop(); 
}

void ds1307_get_time_completo(unsigned int8 &hr, unsigned int8 &min, unsigned int8 &sec) 
{ 
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x00);    
  i2c_start(); 
  i2c_write(0xD1); 
  sec = bcd2bin(i2c_read() & 0x7f); 
  min = bcd2bin(i2c_read() & 0x7f); 
  hr  = bcd2bin(i2c_read(0) & 0x3f);
  i2c_stop();  
}
/////////////////////////////////////////////TERMINO MODIFICAR PARA 24 HORAS//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////MODIFICAR PARA ESTABLECER FORMATO DE 12 HORAS//////////////////////////////////////////////////
void ds1307_set_date_time_medio(unsigned int8 day, unsigned int8 mth, unsigned int8 year, unsigned int8 dow, unsigned int8 hr,int1 am_pm, unsigned int8 min, unsigned int8 sec) 
{ 
  sec &= 0x7F; 
  hr &= 0x1F; 
  hr=bin2bcd(hr); 
  bit_set(hr,6); 
  if(am_pm == 1)
  {
   bit_set(hr,5); 
  } 
  else
  {
   bit_clear(hr,5); 
  } 
  i2c_start(); 
  i2c_write(0xD0);          
  i2c_write(0x00);          
  i2c_write(bin2bcd(sec));    
  i2c_write(bin2bcd(min));  
  i2c_write(hr);  
  i2c_write(bin2bcd(dow));    
  i2c_write(bin2bcd(day));  
  i2c_write(bin2bcd(mth));    
  i2c_write(bin2bcd(year));      
  i2c_write(0x80);        
  i2c_stop(); 
} 

void ds1307_get_time_medio(unsigned int8 &hr, int1 &am_pm, unsigned int8 &min, unsigned int8 &sec) 
{ 
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x00);    
  i2c_start(); 
  i2c_write(0xD1); 
  sec = bcd2bin(i2c_read() & 0x7f); 
  min = bcd2bin(i2c_read() & 0x7f); 
  hr = i2c_read(0); 
  i2c_stop(); 
  am_pm = bit_test(hr,5); 
  hr  = bcd2bin(hr & 0x1f); 
} 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////MODIFICO PARA CONVERTIR DE FORMATO 24HRS A 12HRS/////////////////////////////////////////////
void ds1307_get_time_formato_medio(unsigned int8 &hr, int1 &am_pm)
{
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x02); 
  i2c_start(); 
  i2c_write(0xD1);
  hr  = bcd2bin(i2c_read(0) & 0x3f); 
  i2c_stop();
  if(hr > 12)
  {
  hr = hr - 12;
  am_pm = 1;
  }
  else if(hr == 12)
  {
  am_pm = 1;
  }
  else if(hr < 12 && hr >= 1)
  {
  am_pm = 0;
  }
  else
  {
  hr = hr + 12;
  am_pm = 0;
  }
}

 void ds1307_set_time_formato_medio(unsigned int8 hr, int1 am_pm)
{
  hr &= 0x1F;
  hr=bin2bcd(hr); 
  bit_set(hr,6); 
  if(am_pm)
  {
   bit_set(hr,5); 
  } 
  else
  {
   bit_clear(hr,5); 
  } 
  i2c_start(); 
  i2c_write(0xD0);          
  i2c_write(0x02);
  i2c_write(hr);
  i2c_write(0x80);        
  i2c_stop(); 
}

/////////////////////////////////////////////MODIFICO PARA CONVERTIR DE FORMATO 12HRS A 24HRS/////////////////////////////////////////////////
void ds1307_get_time_formato_completo(unsigned int8 &hr, int1 &am_pm)
{
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x02);    
  i2c_start(); 
  i2c_write(0xD1); 
  hr  = i2c_read(0);//hr  = bcd2bin(i2c_read(0) & 0x1f); //0x1f
  i2c_stop();
  am_pm = bit_test(hr,5);
  hr  = bcd2bin(hr & 0x1f);
  
  if(am_pm == 0 && hr == 12)
  {
  hr = hr - 12;
  }
  else if(am_pm == 1 && hr < 12)
  {
  hr = hr + 12;
  }
}

void ds1307_set_time_formato_completo(unsigned int8 hr, int1 am_pm)
{
  hr &= 0x3F;
  bit_clear(hr,6);
  hr=bin2bcd(hr);
  
  i2c_start(); 
  i2c_write(0xD0);          
  i2c_write(0x02);
  i2c_write(hr);
  i2c_write(0x80);        
  i2c_stop(); 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////FUNCION PARA OBTENER LA FECHA/////////////////////////////////////////////////////////////////////////////
void ds1307_get_date(unsigned int8 &day, unsigned int8 &mth, unsigned int8 &year, unsigned int8 &dow) 
{ 
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x03); 
  i2c_start(); 
  i2c_write(0xD1); 
  dow  = bcd2bin(i2c_read() & 0x7f);  
  day  = bcd2bin(i2c_read() & 0x3f); 
  mth  = bcd2bin(i2c_read() & 0x1f);  
  year = bcd2bin(i2c_read(0));        
  i2c_stop(); 
} 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////FUNCION PARA MODIFICAR DE BINARIO A BCD////////////////////////////////////////////////////
unsigned int8 bin2bcd(unsigned int8 binary_value) 
{ 
  unsigned int8 temp; 
  unsigned int8 retval; 

  temp = binary_value; 
  retval = 0; 

  while(true) 
  { 
    if(temp >= 10) 
    { 
      temp -= 10; 
      retval += 0x10; 
    } 
    else 
    { 
      retval += temp; 
      break; 
    } 
  } 

  return(retval); 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////FUNCION PARA MODIFICAR DE BCD A BINARIO///////////////////////////////////////////////
unsigned int8 bcd2bin(unsigned int8 bcd_value) 
{ 
  unsigned int8 temp; 
  temp = bcd_value; 
  temp >>= 1; 
  temp &= 0x78; 
  return(temp + (temp >> 2) + (bcd_value & 0x0f)); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
