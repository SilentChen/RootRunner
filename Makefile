CC				=	gcc
OBJ_PATH	=	build
BIN_PATH	=	bin

INCLUDE		=	
LIBPATH		=
LIBNAME		=

ifdef GDB
	FLAGS		= -Wall -g 
else
	FLAGS   = -Wall -O3
endif

ifdef CONFIGFILE
	FLAGS		+= -DCONFIG_FILE=\"$(CONFIGFILE)\"
endif


SRCDIR		=	src
C_SOURCE	=	$(foreach d, $(SRCDIR), $(wildcard $(d)/*.c))
C_OBJECT	=	$(patsubst %.c, $(OBJ_PATH)/%.o, $(C_SOURCE))
C_TARGET	=	$(patsubst %.c, %, $(C_SOURCE))	
TARGET		=	$(notdir $(C_TARGET))

all:init $(TARGET) clean_obj

$(TARGET) : % : $(OBJ_PATH)/src/%.o
	$(CC) $^ -o $(BIN_PATH)/$@ $(LIBPATH) $(LIBNAME)


$(C_OBJECT) : $(OBJ_PATH)/%.o : %.c
	$(CC) -c $(FLAGS) $(INCLUDE) $< -o $@

.PHONE:init clean help

#info:
#	@echo "SRCDIR: $(SRCDIR)"
#	@echo "C_SOURCE: $(C_SOURCE)"
#	@echo "C_OBJECT: $(c_object)"
#	@echo "OBJ_PATH: $(OBJ_PATH)"
#	@echo "BIN_PATH: $(BIN_PATH)"
#	@echo "C_TARGETL: $(C_TARGET)"
#	@echo "TARGET: $(TARGET)"
#	@echo "FLAGS: $(FLAGS)"

init:
	$(foreach d, $(SRCDIR), mkdir -p $(OBJ_PATH)/$(d);)

help:
	@echo "###################### Useage: ###########################"
	@echo "|- use GDB=1 to compile with gdb debug mode." 
	@echo "|- use CONFIGFILE=/tmp/config to reset config file" 
	@echo	"|- default config is current dir's config file."
	@echo	"|- eg: make GDB=1, make GDB=1 CONFIGFILE=/tmp/config"

clean: clean_obj clean_bin

clean_obj:
	rm -rf ./$(OBJ_PATH)

clean_bin:
	rm -rf ./$(BIN_PATH)/*
