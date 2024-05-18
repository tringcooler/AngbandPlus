
#DEBUG       := 1
#TEST        := 1

SRC_PATH    := src

TARGET      := target.exe

TAR_PLT     := win

SRC_ALL     := $(wildcard $(SRC_PATH)/*.c)
SRC_TST     := $(filter $(SRC_PATH)/test_%.c, $(SRC_ALL))
SRC_TPLT    := $(SRC_PATH)/main-$(TAR_PLT).c
SRC_APLT    := $(filter-out $(SRC_TPLT), $(wildcard $(SRC_PATH)/main-*.c))
SRC_DST     := $(filter-out $(SRC_APLT), $(filter-out $(SRC_TST), $(SRC_ALL)))

ifneq ($(DEBUG),)
TEST        := 1
endif

ifeq ($(TEST),)
SRC         := $(SRC_DST)
else
SRC         := $(SRC_ALL)
endif
OBJ         := $(patsubst %.c, %.o, $(SRC))

CC = gcc
CC_INC := -I.
CC_FLAG :=
#CC_FLAG := -DUSE_TRANSPARENCY -DWINDOWS
CC_DEF :=

LD := gcc
LD_FLAG := -lgdi32 -lcomdlg32

ifeq ($(DEBUG),)
CC_FLAG_OPT := -O2
else
CC_FLAG_OPT := -g -DDEBUG -Wno-unused-label -Wno-unused-variable -Wno-unused-function
endif

ifneq ($(TEST),)
CC_FLAG_OPT += -DTEST
endif

#CC_FLAG += $(CC_FLAG_OPT) -Werror -Wall

target: $(TARGET)

%.o: %.c
	$(CC) $(CC_FLAG) $(CC_INC) $(CC_DEF) -c $< -o $@
	
$(TARGET): $(OBJ)
	$(LD) $^ $(LD_FLAG) -o $@

clean:
	find . -name "*.[oa]" -exec rm -rf {} \;
	find . -name "*.dep" -exec rm -rf {} \;
	find . -name "*.bak" -exec rm -rf {} \;
	find . -name "*.ld" -exec rm -rf {} \;
	find . -name "*.out" -exec rm -rf {} \;
	find . -name "*.exe" -exec rm -rf {} \;
	find . -name "*.stackdump" -exec rm -rf {} \;
	@echo done
	
.PHONY: target clean
