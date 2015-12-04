SRC_DIR = src
OBJ_DIR = obj

CC = g++

CFLAGS = -W -Wall -pedantic -ansi -MMD -g3
INCLUDES = -Iinclude

TARGET = bin/main
TARGET_MAIN = $(SRC_DIR)/*.c

SRC = ${wildcard $(SRC_DIR)/*.c}
OBJ = ${addprefix $(OBJ_DIR)/, ${notdir ${SRC:.c=.o}}} 

.PHONY: clean depend

all:
	@mkdir -p $(OBJ_DIR)
	@$(MAKE) $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Building $@
	@$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJ)
	@echo --- Done

clean:
	@echo Cleaning...
	@rm -rf obj/ $(TARGET) *~

-include $(OBJ:.o=.d)
