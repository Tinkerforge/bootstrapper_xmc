/* XMC Bootstrapper
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Bricklet firmware boostrapper for XMC1x00 devices
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include "xmc_gpio.h"
#include "xmc_flash.h"
#include "xmc_usic.h"
#include "xmc_uart.h"
#include "xmc_wdt.h"

#include "configs/config.h"

// ********************** IMPORTANT **********************
// * By default we use an absurdly simple linker script  *
// * without BSS or DATA section or similar.             *
// * This keeps the bootstraper very small.              *
// * If we add variables that need to be initialized     *
// * we have to add these sections to the linker script  *
// * and we have to initialize the sections (see below). *
// *******************************************************

#ifdef BOOTSTRAPPER_NEEDS_RAM_INIT
extern uint32_t DataLoadAddr;
extern uint32_t __data_start;
extern uint32_t __data_end;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

inline void ram_init(void) {
	// Initialize relocate segment
	uint32_t *src  = &DataLoadAddr;
	uint32_t *dest = &__data_start;

	if(src != dest) {
		for(; dest < &__data_end;) {
			*dest++ = *src++;
		}
	}

	// Clear zero segment
	for(dest = &__bss_start; dest < &__bss_end;) {
		*dest++ = 0;
	}
}
#endif


static inline void led_init(void) {
	XMC_GPIO_CONFIG_t led;
	led.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
	led.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
	XMC_GPIO_Init(BOOTSTRAPPER_STATUS_LED_PIN, &led);
}

static inline void usic_init(void) {
	// Configure buffer size
	WR_REG(BOOTSTRAPPER_USIC_CHANNEL->DX0CR, USIC_CH_DX0CR_DSEL_Msk, 0, 0);
	WR_REG(BOOTSTRAPPER_USIC_CHANNEL->TBCTR, 0x0700003FU, 0, 0x01000000);
	WR_REG(BOOTSTRAPPER_USIC_CHANNEL->RBCTR, 0x0700003FU, 0, 0x01000000);


	// Configure pins, so bootloader can always use full-duplex (bootstrapper may be half-duplex)

	// TX pin configuration
	const XMC_GPIO_CONFIG_t tx_pin_config = {
		.mode             = BOOTSTRAPPER_TX_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// RX pin configuration
	const XMC_GPIO_CONFIG_t rx_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_PULL_UP,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};

	// Configure  pins
	XMC_GPIO_Init(BOOTSTRAPPER_TX_PIN, &tx_pin_config);
	XMC_GPIO_Init(BOOTSTRAPPER_RX_PIN, &rx_pin_config);

	// Set input source path
	XMC_UART_CH_SetInputSource(BOOTSTRAPPER_USIC, BOOTSTRAPPER_RX_INPUT, BOOTSTRAPPER_RX_SOURCE);

	// Start UART
	XMC_UART_CH_Start(BOOTSTRAPPER_USIC);
}

int main(void) {
#ifdef BOOTSTRAPPER_NEEDS_RAM_INIT
	ram_init();
#endif

	led_init();
	usic_init();

	// Remove data from buffer
	while(!((BOOTSTRAPPER_USIC->TRBSR & (0x01UL << 3) ) >> 3)) {
		volatile __attribute__((unused)) uint8_t _ = (BOOTSTRAPPER_USIC->OUTR & 0xFF);
	}

	uint16_t page_num = 0;
	uint8_t page[BOOTSTRAPPER_PAGE_SIZE];
	while(true) {
		XMC_WDT_Service();
		// Toggle status LED for every page write
		XMC_GPIO_ToggleOutput(BOOTSTRAPPER_STATUS_LED_PIN);

		// Get one page and calculate CRC
		uint8_t crc = 0;
		for(uint16_t i = 0; i < BOOTSTRAPPER_PAGE_SIZE; i++) {
			while(((BOOTSTRAPPER_USIC->TRBSR & (0x01UL << 3) ) >> 3)) {
				XMC_WDT_Service();
			}
			page[i] = (BOOTSTRAPPER_USIC->OUTR & 0xFF);
			crc ^= page[i];
		}

		// Write page
		uint32_t *page_address = (uint32_t*)(BOOTSTRAPPER_FLASH_START + page_num * BOOTSTRAPPER_PAGE_SIZE);
		XMC_FLASH_ErasePage(page_address);
		XMC_FLASH_ProgramVerifyPage(page_address, (uint32_t*)page);

		// Return CRC for every page
		while(!((BOOTSTRAPPER_USIC->TRBSR & (0x01UL << 11)) >> 11)) {
			XMC_WDT_Service();
		}
		BOOTSTRAPPER_USIC->IN[0] = crc;

		// Check if we are done with writing of bootloader
		page_num++;
		if(page_num == (BOOTSTRAPPER_BOOTLOADER_SIZE/BOOTSTRAPPER_PAGE_SIZE)) {
			// Make sure that we always invalidate firmware
			uint32_t *page_address = (uint32_t*)(BOOTSTRAPPER_FLASH_START + page_num * BOOTSTRAPPER_PAGE_SIZE);
			XMC_FLASH_ErasePage(page_address);
			for(uint16_t i = 0; i < BOOTSTRAPPER_PAGE_SIZE; i++) {
				page[i] = 0xFF;
			}
			XMC_FLASH_ProgramVerifyPage(page_address, (uint32_t*)page);
			break;
		}

	}


#if BOOTSTRAPPER_BMI_WITH_CAN == 1
	uint32_t bmi = 0b1010000 << 0  | // ASC BSL with timeout
	               0b1       << 7  | // Boot configuration type selection (boot from BMI)
	               0b1       << 8  | // DAP Type Selection
	               0b00      << 9  | // SWD/SPD Input/Output Selection
	               0b1       << 11 | // CAN Clock Source for CAN BSL Mode
	               0b0001    << 12;  // Timeout = 333ms
#else
	uint32_t bmi = 0b010000 << 0  | // ASC BSL with timeout
	               0b11     << 6  | // Reserved must programmed to 1
	               0b1      << 8  | // DAP Type Selection
	               0b00     << 9  | // SWD/SPD Input/Output Selection
	               0b1      << 11 | // Reserved must programmed to 1
	               0b0001   << 12;  // Timeout = 333ms
#endif

	// Set BMI (automatically restarts the MCU)
	XMC1000_BmiInstallationReq(bmi);
	while(true)

	return 0;
}
