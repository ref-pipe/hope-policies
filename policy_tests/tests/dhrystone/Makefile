RUNTIME ?= bare
include ../isp-runtime-$(RUNTIME).mk

LINKER_SCRIPT = dhrystone-baremetal.lds
TEST_CFLAGS := -DDHRY_LOOPS=5000
TEST_LDFLAGS := --sysroot=${ISP_PREFIX}/riscv32-unknown-elf -L${ISP_PREFIX}/riscv32-unknown-elf/lib
TEST_HELPER_DIR = ..
TEST_INCLUDES += -I$(TEST_HELPER_DIR)
TEST_SOURCES := dhrystone-baremetal.c
TEST_OBJECTS := $(patsubst %.c, %.o, $(TEST_SOURCES))

TEST_TARGET := $(OUTPUT_DIR)/dhrystone

include ../common.mk