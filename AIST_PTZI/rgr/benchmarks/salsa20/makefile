CXX := gcc
CXXFLAGS := -Wall -Wextra -pedantic -std=c11
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2 -DNDEBUG
LDFLAGS :=

BUILD_DIR := build
TARGET_DIR := bin

ifdef WIN32
    CXXFLAGS += -D_WIN32_WINNT=0x0600
else ifdef WINDIR
    CXXFLAGS += -D_WIN32_WINNT=0x0600
endif

# =======================
# Lib
# =======================
LIB_DIR := ../../stream-ciphers/salsa20
LIB_SRC := $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ := $(patsubst $(LIB_DIR)/%.c,$(BUILD_DIR)/lib_%.o,$(LIB_SRC))
LIB_DEP := $(LIB_OBJ:.o=.d)
LIB_INC := -I$(LIB_DIR)

# =======================
# Demo
# =======================
BENCHMARK_SRC := $(wildcard src/*.c)
BENCHMARK_OBJ := $(patsubst src/%.c,$(BUILD_DIR)/benchmark_%.o,$(BENCHMARK_SRC))
BENCHMARK_DEP := $(BENCHMARK_OBJ:.o=.d)
BENCHMARK_TARGET := $(TARGET_DIR)/salsa20
BENCHMARK_INC := -I$src/include

# =======================
# All
# =======================
all: $(BENCHMARK_TARGET)

# --- Link targets ---
$(BENCHMARK_TARGET): $(BENCHMARK_OBJ) $(LIB_OBJ) | $(TARGET_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

# --- Compile objects with dependency generation ---
$(BUILD_DIR)/lib_%.o: $(LIB_DIR)/%.c | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIB_INC) -MMD -MP -c $< -o $@

$(BUILD_DIR)/benchmark_%.o: src/%.c | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(BENCHMARK_INC) $(LIB_INC) -MMD -MP -c $< -o $@

# --- Include dependency files ---
-include $(LIB_DEP)
-include $(BENCHMARK_DEP)

# --- Directories ---
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# --- Clean ---
clean:
	rm -rf $(BUILD_DIR) $(TARGET_DIR)

# --- Debug / Release ---
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

release: CXXFLAGS += $(RELEASE_FLAGS)
release: all

.PHONY: all debug release clean
