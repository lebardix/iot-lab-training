APPLICATION = hash

BOARD ?= native

# Add required modules here
USEMODULE += crypto
USEMODULE += cipher_modes
USEMODULE += fmt
USEMODULE += shell

# Specify we are using AES crypto
CFLAGS += -DCRYPTO_AES

RIOTBASE ?= $(CURDIR)/../../RIOT

include $(RIOTBASE)/Makefile.include