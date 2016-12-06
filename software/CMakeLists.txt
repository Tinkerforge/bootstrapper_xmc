CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake_config_custom.txt)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_config_custom.txt)
else()
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_config_default.txt)
endif()

SET(PROJECT_NAME bootstrapper)
SET(CHIP_FAMILY XMC1)
SET(MCU cortex-m0\ -mthumb)
PROJECT(${PROJECT_NAME})

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
#SET(OPTIMIZATION_LEVEL 0)
#SET(DEBUG "-g -ggdb")

SET(OPTIMIZATION_LEVEL s)
SET(DEBUG "")

SET(CMAKE_BUILD_TYPE None)
ENABLE_LANGUAGE(C ASM)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/XMCLib/inc/"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/CMSIS/Include/"
)

if(CHIP MATCHES "^XMC11.*$")
INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/CMSIS/Infineon/XMC1100_series/Include/"
)
elseif(CHIP MATCHES "^XMC14.*")
INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/CMSIS/Infineon/XMC1400_series/Include/"
)
endif()

# find source files
SET(SOURCES
	"${PROJECT_SOURCE_DIR}/src/main.c"
	"${PROJECT_SOURCE_DIR}/src/entry.S"
	
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/XMCLib/src/xmc_gpio.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/XMCLib/src/xmc1_gpio.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/xmclib/XMCLib/src/xmc1_flash.c"
)

MESSAGE(STATUS "\nFound following source files:\n ${SOURCES}\n")

# define executable
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_define_executable.txt)

# define standard compile and linker flags
SET(LINKER_SCRIPT_NAME xmc1_bootstrapper.ld)
SET(FLASH_LENGTH ${CHIP_FLASH_LENGTH}) 
SET(SRAM_LENGTH 0x3E00) # Set SRAM length and origin according to ROM bootloader definitions for bootstrapper
SET(SRAM_ORIGIN 0x20000200)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_comcu_add_standard_flags.txt)

# add custom build commands
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_comcu_add_standalone_custom_commands.txt)
 
# add preprocessor defines
ADD_DEFINITIONS(-D${CHIP} -D__${CHIP_FAMILY}__ -DNOSTARTFILES)
