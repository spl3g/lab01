CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
LDFLAGS = -L$(BUILD_DIR)/third_party -L$(BUILD_DIR)
LDLIBS = -lchttp -lfrozen

SRC_DIR = src
BUILD_DIR = build
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
EXEC = app

TP_DIR = third_party

TP_LIBS = \
	$(BUILD_DIR)/third_party/libchttp.a \
	$(BUILD_DIR)/third_party/libfrozen.a


all: $(BUILD_DIR) $(EXEC)

$(EXEC): $(OBJ) | $(TP_LIBS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/third_party/libchttp.a: $(TP_DIR)/chttp
	@mkdir -p $(@D)
	make -C $(TP_DIR)/chttp
	mv $(<)/libchttp.a $@

$(BUILD_DIR)/third_party/libfrozen.a: $(TP_DIR)/frozen
	@mkdir -p $(@D)
	$(CC) -c $(TP_DIR)/frozen/frozen.c -o $(@D)/frozen.o
	ar rcs $@ $(@D)/frozen.o

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(EXEC)
	make -C $(TP_DIR)/chttp clean

.PHONY: all clean
