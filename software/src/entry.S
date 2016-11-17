/* XMC Bootstrapper
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * entry.S: Small XMC bootstrapper entry function that calls main
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

// program sits at 0x20000200, thus the bootstrapper can have a
// max size of 0x1800 (which is plenty).
#define STACK_POINTER 0x20002000

// Vector Table
// This is indirectly branched to through the veneers
.syntax unified
.cpu cortex-m0

.section ".entry"

.thumb

// Entry function
.thumb_func
.globl entry
.type   entry, %function

entry:
.fnstart

	// Set stack pointer
	LDR R0,=STACK_POINTER
	MOV SP,R0

	// Jimp to main
	MOVS R0,#0
	MOVS R1,#0
	LDR R2, =main
	MOV PC,R2

.pool
.cantunwind
.fnend
.size entry,.-entry
