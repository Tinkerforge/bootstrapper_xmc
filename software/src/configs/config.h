/* XMC Bootstrapper
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.c: All configurations for bootstrapper
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

#ifndef CONFIG_BOOTLOADER_H
#define CONFIG_BOOTLOADER_H

#include "xmc_device.h"

#if __has_include("config_custom_bootstrapper.h")
#include "config_custom_bootstrapper.h"
#else
#include "config_default_bootstrapper.h"
#endif

#define UARTBB_TX_PIN P0_5

#define BOOTSTRAPPER_VERSION_MAJOR 1
#define BOOTSTRAPPER_VERSION_MINOR 0
#define BOOTSTRAPPER_VERSION_REVISION 0

#define BOOTSTRAPPER_BOOTLOADER_SIZE (8*1024) // Bootloader should always have size 8kb

#endif
