
/*
if(TP_Touchpad_Pressed())
	{					
		uint16_t x_pos = 0;
		uint16_t y_pos = 0;
		
		uint16_t position_array[2];
		if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
		{
		x_pos = position_array[0];
		y_pos = position_array[1];
		}							
	}
*/
//-----------------------------------


#include "ILI9341_Touchscreen.h"
#include "stm32f4xx_hal.h"

//Internal Touchpad command, do not call directly
uint16_t TP_Read(void)
{
    uint8_t i = 16;
    uint16_t value = 0;

    while(i > 0x00)
    {
        value <<= 1;

				HAL_GPIO_WritePin(T_CLK_PORT, T_CLK_PIN, GPIO_PIN_SET);
				HAL_GPIO_WritePin(T_CLK_PORT, T_CLK_PIN, GPIO_PIN_RESET);
			
        if(HAL_GPIO_ReadPin(T_MISO_PORT, T_MISO_PIN) != 0)
        {
            value++;
        }

        i--;
    };

    return value;
}

//Internal Touchpad command, do not call directly
void TP_Write(uint8_t value)
{
    uint8_t i = 0x08;

		HAL_GPIO_WritePin(T_CLK_PORT, T_CLK_PIN, GPIO_PIN_RESET);	
	
    while(i > 0)
    {
        if((value & 0x80) != 0x00)
        {
						HAL_GPIO_WritePin(T_MOSI_PORT, T_MOSI_PIN, GPIO_PIN_SET);
        }
        else
        {
						HAL_GPIO_WritePin(T_MOSI_PORT, T_MOSI_PIN, GPIO_PIN_RESET);
        }

        value <<= 1;
				HAL_GPIO_WritePin(T_CLK_PORT, T_CLK_PIN, GPIO_PIN_SET);
				HAL_GPIO_WritePin(T_CLK_PORT, T_CLK_PIN, GPIO_PIN_RESET);        
        i--;
    };
}



//Read coordinates of touchscreen press. Position[0] = X, Position[1] = Y
uint8_t TP_Read_Coordinates(uint16_t Coordinates[2])
{
		HAL_GPIO_WritePin(T_CLK_PORT, T_CLK_PIN, GPIO_PIN_SET);		
		HAL_GPIO_WritePin(T_MOSI_PORT, T_MOSI_PIN, GPIO_PIN_SET);		
		HAL_GPIO_WritePin(T_CS_PORT, T_CS_PIN, GPIO_PIN_SET);		

	
	
    uint32_t avg_x, avg_y = 0;		
		uint16_t rawx, rawy = 0;	
		uint32_t calculating_x, calculating_y = 0;
	
    uint32_t samples = NO_OF_POSITION_SAMPLES;
    uint32_t counted_samples = 0;

		HAL_GPIO_WritePin(T_CS_PORT, T_CS_PIN, GPIO_PIN_RESET);

	
    while((samples > 0)&&(HAL_GPIO_ReadPin(T_IRQ_PORT, T_IRQ_PIN) == 0))
    {			
        TP_Write(CMD_RDY);

				rawy = TP_Read();	
				avg_y += rawy;
				calculating_y += rawy;

				
        TP_Write(CMD_RDX);
        rawx = TP_Read();
				avg_x += rawx;
				calculating_x += rawx;
        samples--;
				counted_samples++;
    };
		
		HAL_GPIO_WritePin(T_CS_PORT, T_CS_PIN, GPIO_PIN_SET);

		
		if((counted_samples == NO_OF_POSITION_SAMPLES)&&(HAL_GPIO_ReadPin(T_IRQ_PORT, T_IRQ_PIN) == 0))
		{
		
		calculating_x /= counted_samples;
		calculating_y /= counted_samples;
		
		rawx = calculating_x;
		rawy = calculating_y;		
		
		rawx *= -1;
		rawy *= -1;
		
		//CONVERTING 16bit Value to Screen coordinates
    // 65535/273 = 240!
		// 65535/204 = 320!
                Coordinates[0] = (uint16_t)(((240 - (rawx/X_TRANSLATION)) - X_OFFSET)*X_MAGNITUDE);
		Coordinates[1] = (uint16_t)(((rawy/Y_TRANSLATION)- Y_OFFSET)*Y_MAGNITUDE);
		
		return TOUCHPAD_DATA_OK;			
		}
		else
		{
			Coordinates[0] = 0;
			Coordinates[1] = 0;
			return TOUCHPAD_DATA_NOISY;
		}
}

//Check if Touchpad was pressed. Returns TOUCHPAD_PRESSED (1) or TOUCHPAD_NOT_PRESSED (0)
uint8_t TP_Touchpad_Pressed(void)
{
	if(HAL_GPIO_ReadPin(T_IRQ_PORT,T_IRQ_PIN) == 0)
	{
		return TOUCHPAD_PRESSED;
	}
	else
	{
		return TOUCHPAD_NOT_PRESSED;
	}
}


