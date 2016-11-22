/* XMC Bootstrapper
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.c: Default XMC bootstrapper configurations (for test compilation)
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

#ifndef CONFIG_DEFAULT_BOOTSTRAPPER_H
#define CONFIG_DEFAULT_BOOTSTRAPPER_H

#include "xmc_gpio.h"

// GPS V2
#if 0
#define BOOTSTRAPPER_STATUS_LED_PIN P2_1
#define BOOTSTRAPPER_USIC           USIC0_CH0
#define BOOTSTRAPPER_PAGE_SIZE      256
#define BOOTSTRAPPER_FLASH_START    0x10001000
#define BOOTSTRAPPER_FLASH_SIZE     (16*1024)

#define BOOTSTRAPPER_BMI_WITH_CAN   0
#endif

// RS485
#if 1
#define BOOTSTRAPPER_STATUS_LED_PIN P4_5
#define BOOTSTRAPPER_USIC           USIC0_CH1
#define BOOTSTRAPPER_PAGE_SIZE      256
#define BOOTSTRAPPER_FLASH_START    0x10001000
#define BOOTSTRAPPER_FLASH_SIZE     (64*1024)

#define BOOTSTRAPPER_BMI_WITH_CAN   1
#endif

#endif
