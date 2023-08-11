# EDIT HERE:
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURDIR := $(patsubst %/, %, $(dir $(MK_PATH)))

C_SOURCES_EXCEPTION_HDR := $(CURDIR)/src/exception_handler.c
C_INCLUDES_EXCEPTION_HDR := -I$(CURDIR)/inc/
S_SOURCE_EXCEPTION_HDR := $(CURDIR)/portable/ARM_CM4F/GCC/exception.s
################################################################################


