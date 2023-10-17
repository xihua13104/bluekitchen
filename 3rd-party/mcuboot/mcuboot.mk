# EDIT HERE:
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURDIR := $(patsubst %/, %, $(dir $(MK_PATH)))

mcuboot = none

MCUBOOT_IMG_TOOL := $(CURDIR)/scripts/imgtool.py
MCUBOOT_SIGNING_KEY :=  $(CURDIR)/root-rsa-2048.pem

MCUBOOT_IMG_HEADER_SIZE := 0x200
MCUBOOT_IMG_SLOT_SIZE	:= 0x20000
MCUBOOT_IMG_ALIGN		:= 4
MCUBOOT_IMG_VERSION		:= 1.0.0
C_DEFS_MCUBOOT += -DMCUBOOT_IMG_PAYLOAD_OFFSET=${MCUBOOT_IMG_HEADER_SIZE}+${MCUBOOT_IMG_SLOT_SIZE}
C_DEFS_MCUBOOT += -DMCUBOOT_IMG_SLOT_SIZE=${MCUBOOT_IMG_SLOT_SIZE}
C_DEFS_MCUBOOT += -DMCUBOOT_IMG_ALIGN=${MCUBOOT_IMG_ALIGN}

C_SOURCES_MCUBOOT := $(CURDIR)/boot/bootutil/src/boot_record.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/bootutil_misc.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/bootutil_public.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/caps.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/encrypted.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/fault_injection_hardening.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/image_ec.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/image_ec256.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/image_ed25519.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/image_rsa.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/image_validate.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/loader.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/swap_misc.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/swap_move.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/swap_scratch.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/bootutil/src/tlv.c

C_SOURCES_MCUBOOT += $(CURDIR)/boot/embedded/mcuboot_main.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/embedded/keys.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/embedded/port/stm32-f4discovery/boot_flash_port.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/embedded/port/stm32-f4discovery/boot_startup_port.c
C_SOURCES_MCUBOOT += $(CURDIR)/boot/embedded/port/stm32-f4discovery/system_port.c
C_SOURCES_MCUBOOT += $(CURDIR)/ext/tinycrypt/lib/source/sha256.c
C_SOURCES_MCUBOOT += $(CURDIR)/ext/tinycrypt/lib/source/utils.c
C_INCLUDES_MCUBOOT := -I$(CURDIR)/boot/bootutil/include
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/bootutil
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/bootutil/include/bootutil/crypto
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/boot_serial/include/boot_serial
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/embedded/include
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/embedded/include/mcuboot_config
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/embedded/include/flash_map_backend
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/embedded/include/sysflash
C_INCLUDES_MCUBOOT += -I$(CURDIR)/boot/embedded/include/port
C_INCLUDES_MCUBOOT += -I$(CURDIR)/sim/mcuboot-sys/csupport
C_INCLUDES_MCUBOOT += -I$(CURDIR)/ext/tinycrypt/lib/include
C_INCLUDES_MCUBOOT += -I$(CURDIR)/ext/tinycrypt/lib/include/tinycrypt

ifeq (${mcuboot}, boot)
C_DEFS_MCUBOOT += -DMCUBOOT_IMG_BOOTLOADER
endif
ifeq (${mcuboot}, app1)
C_DEFS_MCUBOOT += -DMCUBOOT_IMG_APPLICATION1
endif
ifeq (${mcuboot}, app2)
C_DEFS_MCUBOOT += -DMCUBOOT_IMG_APPLICATION2
endif
################################################################################