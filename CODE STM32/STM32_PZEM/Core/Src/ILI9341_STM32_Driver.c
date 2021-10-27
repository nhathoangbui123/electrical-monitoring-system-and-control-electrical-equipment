
/* Includes ------------------------------------------------------------------*/
#include "ILI9341_STM32_Driver.h"
#include "spi.h"
#include "gpio.h"
#include "main.h"

/* Global Variables ------------------------------------------------------------------*/
volatile uint16_t LCD_HEIGHT            = ILI9341_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH	        = ILI9341_SCREEN_WIDTH;

/* Initialize SPI */
void ILI9341_SPI_Init(void)
{
  MX_SPI1_Init();																							//SPI INIT
  MX_GPIO_Init();																							//GPIO INIT
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);	//CS OFF
}

/*Send data (char) to LCD*/
void ILI9341_SPI_Send(unsigned char SPI_Data)
{
  //HAL_SPI_Transmit(HSPI_INSTANCE, &SPI_Data, 1, 1);
  HAL_SPI_Transmit_IT(HSPI_INSTANCE,&SPI_Data,1);
  //HAL_SPI_Transmit_DMA(HSPI_INSTANCE,&SPI_Data,1);
}

/* Send command (char) to LCD */
void ILI9341_Write_Command(uint8_t Command)
{
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);	
  ILI9341_SPI_Send(Command);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);		
}

/* Send Data (char) to LCD */
void ILI9341_Write_Data(uint8_t Data)
{
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  ILI9341_SPI_Send(Data);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/* Set Address - Location block - to draw into */
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
  ILI9341_Write_Command(0x2A);
  ILI9341_Write_Data(X1>>8);
  ILI9341_Write_Data(X1);
  ILI9341_Write_Data(X2>>8);
  ILI9341_Write_Data(X2);

  ILI9341_Write_Command(0x2B);
  ILI9341_Write_Data(Y1>>8);
  ILI9341_Write_Data(Y1);
  ILI9341_Write_Data(Y2>>8);
  ILI9341_Write_Data(Y2);

  ILI9341_Write_Command(0x2C);
}

/*HARDWARE RESET*/
void ILI9341_Reset(void)
{
  HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
  //HAL_Delay(200);
  delay_ms(200);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  //HAL_Delay(200);
  delay_ms(200);
  HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);	
}

/*Ser rotation of the screen - changes x0 and y0*/
void ILI9341_Set_Rotation(uint8_t Rotation) 
{
	
  uint8_t screen_rotation = Rotation;

  ILI9341_Write_Command(0x36);
  //HAL_Delay(1);
  delay_ms(1);
	
  switch(screen_rotation) 
    {
      case SCREEN_VERTICAL_1:
      ILI9341_Write_Data(0x40|0x08);
      LCD_WIDTH = 240;
      LCD_HEIGHT = 320;
      break;
      case SCREEN_HORIZONTAL_1:
      ILI9341_Write_Data(0x20|0x08);
      LCD_WIDTH  = 320;
      LCD_HEIGHT = 240;
      break;
      case SCREEN_VERTICAL_2:
      ILI9341_Write_Data(0x80|0x08);
      LCD_WIDTH  = 240;
      LCD_HEIGHT = 320;
      break;
      case SCREEN_HORIZONTAL_2:
      ILI9341_Write_Data(0x40|0x80|0x20|0x08);
      LCD_WIDTH  = 320;
      LCD_HEIGHT = 240;
      break;
      default:
      //EXIT IF SCREEN ROTATION NOT VALID!
      break;
    }
}

/*Enable LCD display*/
void ILI9341_Enable(void)
{
  HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
}

/*Initialize LCD display*/
void ILI9341_Init(void)
{
  ILI9341_Enable();
  ILI9341_SPI_Init();
  ILI9341_Reset();

  //SOFTWARE RESET
  ILI9341_Write_Command(0x01);
  //HAL_Delay(1000);
  delay_ms(1000);
          
  //POWER CONTROL A
  ILI9341_Write_Command(0xCB);
  ILI9341_Write_Data(0x39);
  ILI9341_Write_Data(0x2C);
  ILI9341_Write_Data(0x00);
  ILI9341_Write_Data(0x34);
  ILI9341_Write_Data(0x02);

  //POWER CONTROL B
  ILI9341_Write_Command(0xCF);
  ILI9341_Write_Data(0x00);
  ILI9341_Write_Data(0xC1);
  ILI9341_Write_Data(0x30);

  //DRIVER TIMING CONTROL A
  ILI9341_Write_Command(0xE8);
  ILI9341_Write_Data(0x85);
  ILI9341_Write_Data(0x00);
  ILI9341_Write_Data(0x78);

  //DRIVER TIMING CONTROL B
  ILI9341_Write_Command(0xEA);
  ILI9341_Write_Data(0x00);
  ILI9341_Write_Data(0x00);

  //POWER ON SEQUENCE CONTROL
  ILI9341_Write_Command(0xED);
  ILI9341_Write_Data(0x64);
  ILI9341_Write_Data(0x03);
  ILI9341_Write_Data(0x12);
  ILI9341_Write_Data(0x81);

  //PUMP RATIO CONTROL
  ILI9341_Write_Command(0xF7);
  ILI9341_Write_Data(0x20);

  //POWER CONTROL,VRH[5:0]
  ILI9341_Write_Command(0xC0);
  ILI9341_Write_Data(0x23);

  //POWER CONTROL,SAP[2:0];BT[3:0]
  ILI9341_Write_Command(0xC1);
  ILI9341_Write_Data(0x10);

  //VCM CONTROL
  ILI9341_Write_Command(0xC5);
  ILI9341_Write_Data(0x3E);
  ILI9341_Write_Data(0x28);

  //VCM CONTROL 2
  ILI9341_Write_Command(0xC7);
  ILI9341_Write_Data(0x86);

  //MEMORY ACCESS CONTROL
  ILI9341_Write_Command(0x36);
  ILI9341_Write_Data(0x48);

  //PIXEL FORMAT
  ILI9341_Write_Command(0x3A);
  ILI9341_Write_Data(0x55);

  //FRAME RATIO CONTROL, STANDARD RGB COLOR
  ILI9341_Write_Command(0xB1);
  ILI9341_Write_Data(0x00);
  ILI9341_Write_Data(0x18);

  //DISPLAY FUNCTION CONTROL
  ILI9341_Write_Command(0xB6);
  ILI9341_Write_Data(0x08);
  ILI9341_Write_Data(0x82);
  ILI9341_Write_Data(0x27);

  //3GAMMA FUNCTION DISABLE
  ILI9341_Write_Command(0xF2);
  ILI9341_Write_Data(0x00);

  //GAMMA CURVE SELECTED
  ILI9341_Write_Command(0x26);
  ILI9341_Write_Data(0x01);

  //POSITIVE GAMMA CORRECTION
  ILI9341_Write_Command(0xE0);
  ILI9341_Write_Data(0x0F);
  ILI9341_Write_Data(0x31);
  ILI9341_Write_Data(0x2B);
  ILI9341_Write_Data(0x0C);
  ILI9341_Write_Data(0x0E);
  ILI9341_Write_Data(0x08);
  ILI9341_Write_Data(0x4E);
  ILI9341_Write_Data(0xF1);
  ILI9341_Write_Data(0x37);
  ILI9341_Write_Data(0x07);
  ILI9341_Write_Data(0x10);
  ILI9341_Write_Data(0x03);
  ILI9341_Write_Data(0x0E);
  ILI9341_Write_Data(0x09);
  ILI9341_Write_Data(0x00);

  //NEGATIVE GAMMA CORRECTION
  ILI9341_Write_Command(0xE1);
  ILI9341_Write_Data(0x00);
  ILI9341_Write_Data(0x0E);
  ILI9341_Write_Data(0x14);
  ILI9341_Write_Data(0x03);
  ILI9341_Write_Data(0x11);
  ILI9341_Write_Data(0x07);
  ILI9341_Write_Data(0x31);
  ILI9341_Write_Data(0xC1);
  ILI9341_Write_Data(0x48);
  ILI9341_Write_Data(0x08);
  ILI9341_Write_Data(0x0F);
  ILI9341_Write_Data(0x0C);
  ILI9341_Write_Data(0x31);
  ILI9341_Write_Data(0x36);
  ILI9341_Write_Data(0x0F);

  //EXIT SLEEP
  ILI9341_Write_Command(0x11);
  //HAL_Delay(120);
  delay_ms(120);

  //TURN ON DISPLAY
  ILI9341_Write_Command(0x29);

  //STARTING ROTATION
  ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
}

//INTERNAL FUNCTION OF LIBRARY, USAGE NOT RECOMENDED, USE Draw_Pixel INSTEAD
/*Sends single pixel colour information to LCD*/
void ILI9341_Draw_Colour(uint16_t Colour)
{
  //SENDS COLOUR
  unsigned char TempBuffer[2] = {Colour>>8, Colour};	
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  //HAL_SPI_Transmit(HSPI_INSTANCE, TempBuffer, 2, 1);
  HAL_SPI_Transmit_IT(HSPI_INSTANCE,TempBuffer,2);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

//INTERNAL FUNCTION OF LIBRARY
/*Sends block colour information to LCD*/
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size)
{
  //SENDS COLOUR
  uint32_t Buffer_Size = 0;
  if((Size*2) < BURST_MAX_SIZE)
  {
    Buffer_Size = Size;
  }
  else
  {
    Buffer_Size = BURST_MAX_SIZE;
  }
          
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);

  unsigned char chifted = 	Colour>>8;
  unsigned char burst_buffer[500];
  for(uint32_t j = 0; j < 500; j+=2)
    {
      burst_buffer[j] = 	chifted;
      burst_buffer[j+1] = Colour;
    }

  uint32_t Sending_Size = Size*2;
  uint32_t Sending_in_Block = Sending_Size/500;
  uint32_t Remainder_from_block = Sending_Size%500;

  if(Sending_in_Block != 0)
  {
    for(uint32_t j = 0; j < (Sending_in_Block); j++)
      {
        //HAL_SPI_Transmit(HSPI_INSTANCE, (unsigned char *)burst_buffer, 500, 10);
        HAL_SPI_Transmit_IT(HSPI_INSTANCE,(unsigned char *)burst_buffer,500);
        //HAL_SPI_Transmit_DMA(HSPI_INSTANCE,(unsigned char *)burst_buffer,500);
        	
      }
  }
//REMAINDER!
  //HAL_SPI_Transmit(HSPI_INSTANCE, (unsigned char *)burst_buffer, Remainder_from_block, 10);
  HAL_SPI_Transmit_IT(HSPI_INSTANCE,(unsigned char *)burst_buffer,Remainder_from_block);
  //HAL_SPI_Transmit_DMA(HSPI_INSTANCE,(unsigned char *)burst_buffer,Remainder_from_block);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

//FILL THE ENTIRE SCREEN WITH SELECTED COLOUR (either #define-d ones or custom 16bit)
/*Sets address (entire screen) and Sends Height*Width ammount of colour information to LCD*/
void ILI9341_Fill_Screen(uint16_t Colour)
{
  uint16_t XX=LCD_WIDTH;
  uint16_t YY=LCD_HEIGHT;
  ILI9341_Set_Address(0,0,XX,YY);	
  ILI9341_Draw_Colour_Burst(Colour, XX*YY);	
}

//DRAW PIXEL AT XY POSITION WITH SELECTED COLOUR
//
//Location is dependant on screen orientation. x0 and y0 locations change with orientations.
//Using pixels to draw big simple structures is not recommended as it is really slow
//Try using either rectangles or lines if possible
//
void ILI9341_Draw_Pixel(uint16_t X,uint16_t Y,uint16_t Colour) 
{
  if((X >=LCD_WIDTH) || (Y >=LCD_HEIGHT)) return;	//OUT OF BOUNDS!
          
  //ADDRESS
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  ILI9341_SPI_Send(0x2A);
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);		

  //XDATA
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);	
  unsigned char Temp_Buffer[4] = {X>>8,X, (X+1)>>8, (X+1)};
  //HAL_SPI_Transmit(HSPI_INSTANCE, Temp_Buffer, 4, 1);
  HAL_SPI_Transmit_IT(HSPI_INSTANCE,Temp_Buffer,4);
  //HAL_SPI_Transmit_DMA(HSPI_INSTANCE,Temp_Buffer,4);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);

  //ADDRESS
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);	
  ILI9341_SPI_Send(0x2B);
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);			
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);			

  //YDATA
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  unsigned char Temp_Buffer1[4] = {Y>>8,Y, (Y+1)>>8, (Y+1)};
  //HAL_SPI_Transmit(HSPI_INSTANCE, Temp_Buffer1, 4, 1);
  HAL_SPI_Transmit_IT(HSPI_INSTANCE,Temp_Buffer1,4);
  //HAL_SPI_Transmit_DMA(HSPI_INSTANCE,Temp_Buffer1,4);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);

  //ADDRESS	
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);	
  ILI9341_SPI_Send(0x2C);
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);			
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);			

  //COLOUR	
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
  unsigned char Temp_Buffer2[2] = {Colour>>8, Colour};
  //HAL_SPI_Transmit(HSPI_INSTANCE, Temp_Buffer2, 2, 1);
  HAL_SPI_Transmit_IT(HSPI_INSTANCE,Temp_Buffer2,2);
  //HAL_SPI_Transmit_DMA(HSPI_INSTANCE,Temp_Buffer2,4);
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
	
}

//DRAW RECTANGLE OF SET SIZE AND HEIGTH AT X and Y POSITION WITH CUSTOM COLOUR
//
//Rectangle is hollow. X and Y positions mark the upper left corner of rectangle
//As with all other draw calls x0 and y0 locations dependant on screen orientation
//

void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour)
{
  if((X >=LCD_WIDTH) || (Y >=LCD_HEIGHT)) return;
  if((X+Width-1)>=LCD_WIDTH)
          {
                  Width=LCD_WIDTH-X;
          }
  if((Y+Height-1)>=LCD_HEIGHT)
          {
                  Height=LCD_HEIGHT-Y;
          }
  ILI9341_Set_Address(X, Y, X+Width-1, Y+Height-1);
  ILI9341_Draw_Colour_Burst(Colour, Height*Width);
}

//DRAW LINE FROM X,Y LOCATION to X+Width,Y LOCATION
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour)
{
  if((X >=LCD_WIDTH) || (Y >=LCD_HEIGHT)) return;
  if((X+Width-1)>=LCD_WIDTH)
          {
                  Width=LCD_WIDTH-X;
          }
  ILI9341_Set_Address(X, Y, X+Width-1, Y);
  ILI9341_Draw_Colour_Burst(Colour, Width);
}

//DRAW LINE FROM X,Y LOCATION to X,Y+Height LOCATION
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour)
{
  if((X >=LCD_WIDTH) || (Y >=LCD_HEIGHT)) return;
  if((Y+Height-1)>=LCD_HEIGHT)
          {
                  Height=LCD_HEIGHT-Y;
          }
  ILI9341_Set_Address(X, Y, X, Y+Height-1);
  ILI9341_Draw_Colour_Burst(Colour, Height);
}

