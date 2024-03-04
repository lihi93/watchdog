
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -fPIC
CPPFLAGS := -Iinclude
LDFLAGS := -Wl,-rpath,.
CDEBUG := -g
CRELEASE = -DNDEBUG -O3
VLG := valgrind -s --leak-check=yes --track-origins=yes
HLG := valgrind --tool=helgrind -s

# Directory for object files
OBJ_DIR := obj
TEST_DIR := test
BIN_DIR := bin
SRC_DIR := src
INC_DIR := include

LIB_SRC := $(wildcard $(SRC_DIR)/*.c)
LIB_OBJ := $(addprefix $(OBJ_DIR)/, $(notdir $(LIB_SRC:.c=.o)))

# Shared library
LIBRARY := libwd.so

# Main executables
MAIN_EXE := main

#Run
RUN := $(BIN_DIR)/wd_client

# Default target
all: $(LIBRARY) $(MAIN_EXE) clean

run: 
	$(RUN)
	
# Build the shared library
$(LIBRARY): $(LIB_OBJ)
	$(CC) -shared -o $@ $(LIB_OBJ) -pthread

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Link main executables with the shared library
main: $(BIN_DIR)/wd.out $(BIN_DIR)/wd_client

# Compile wd.c with links to the shared library
$(BIN_DIR)/wd.out: $(TEST_DIR)/wd.c $(LIBRARY) | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lwd $(LDFLAGS)

# Compile wd_client.c with links to the shared library
$(BIN_DIR)/wd_client: $(TEST_DIR)/wd_client.c $(LIBRARY) | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lwd $(LDFLAGS)

#Make DEBUG
debug: $(CFLAGS) += $(CDEBUG)
debug: all	
	
#Make RELEASE
release: CFLAGS += $(CRELEASE)
release: all

#Make vlg
vlg:
	$(VLG) $(RUN)

#Make helgrind
hlg:
	$(HLG) $(RUN)

# Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean
