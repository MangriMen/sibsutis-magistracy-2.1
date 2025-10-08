CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2 -DNDEBUG
LDFLAGS := -lgmp
NETWORK_LDFLAGS := -lws2_32

BUILD_DIR := build
TARGET_DIR := bin

ifdef WIN32
    NETWORK_LDFLAGS = -lws2_32 -lgmp
    CXXFLAGS += -D_WIN32_WINNT=0x0600
else ifdef WINDIR
    NETWORK_LDFLAGS = -lws2_32 -lgmp
    CXXFLAGS += -D_WIN32_WINNT=0x0600
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        NETWORK_LDFLAGS = -lgmp
    endif
    ifeq ($(UNAME_S),Darwin)
        NETWORK_LDFLAGS = -lgmp
    endif
endif


# =======================
# Lib
# =======================
LIB_DIR := lib
LIB_SRC := $(wildcard $(LIB_DIR)/src/*.cpp)
LIB_OBJ := $(patsubst $(LIB_DIR)/src/%.cpp,$(BUILD_DIR)/lib_%.o,$(LIB_SRC))
LIB_DEP := $(LIB_OBJ:.o=.d)
LIB_INC := -I$(LIB_DIR)/include

# =======================
# Visual
# =======================
VISUAL_DIR := visual
VISUAL_SRC := $(wildcard $(VISUAL_DIR)/src/*.cpp)
VISUAL_OBJ := $(patsubst $(VISUAL_DIR)/src/%.cpp,$(BUILD_DIR)/visual_%.o,$(VISUAL_SRC))
VISUAL_DEP := $(VISUAL_OBJ:.o=.d)
VISUAL_INC := -I$(VISUAL_DIR)/include

# =======================
# Demo
# =======================
DEMO_DIR := demo
DEMO_SRC := $(wildcard $(DEMO_DIR)/src/*.cpp)
DEMO_OBJ := $(patsubst $(DEMO_DIR)/src/%.cpp,$(BUILD_DIR)/demo_%.o,$(DEMO_SRC))
DEMO_DEP := $(DEMO_OBJ:.o=.d)
DEMO_TARGET := $(TARGET_DIR)/demo
DEMO_INC := -I$(DEMO_DIR)/include

# =======================
# Generator
# =======================
GEN_DIR := generator
GEN_SRC := $(wildcard $(GEN_DIR)/src/*.cpp)
GEN_OBJ := $(patsubst $(GEN_DIR)/src/%.cpp,$(BUILD_DIR)/generator_%.o,$(GEN_SRC))
GEN_DEP := $(GEN_OBJ:.o=.d)
GEN_TARGET := $(TARGET_DIR)/generator
GEN_INC := -I$(GEN_DIR)/include

# =======================
# Duo Client
# =======================
DUO_CLIENT_DIR := duo_client
DUO_CLIENT_SRC := $(wildcard $(DUO_CLIENT_DIR)/src/*.cpp)
DUO_CLIENT_OBJ := $(patsubst $(DUO_CLIENT_DIR)/src/%.cpp,$(BUILD_DIR)/duo_client_%.o,$(DUO_CLIENT_SRC))
DUO_CLIENT_DEP := $(DUO_CLIENT_OBJ:.o=.d)
DUO_CLIENT_TARGET := $(TARGET_DIR)/duo_client
DUO_CLIENT_INC := -I$(DUO_CLIENT_DIR)/include

# =======================
# All
# =======================
all: $(DEMO_TARGET) $(GEN_TARGET) $(DUO_CLIENT_TARGET)

# --- Link targets ---
$(DEMO_TARGET): $(DEMO_OBJ) $(VISUAL_OBJ) $(LIB_OBJ) | $(TARGET_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(GEN_TARGET): $(GEN_OBJ) $(VISUAL_OBJ) $(LIB_OBJ) | $(TARGET_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(DUO_CLIENT_TARGET): $(DUO_CLIENT_OBJ) $(VISUAL_OBJ) $(LIB_OBJ) | $(TARGET_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS) $(NETWORK_LDFLAGS)

# --- Compile objects with dependency generation ---
$(BUILD_DIR)/lib_%.o: $(LIB_DIR)/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIB_INC) -MMD -MP -c $< -o $@

$(BUILD_DIR)/visual_%.o: $(VISUAL_DIR)/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(VISUAL_INC) -MMD -MP -c $< -o $@

$(BUILD_DIR)/demo_%.o: $(DEMO_DIR)/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DEMO_INC) $(VISUAL_INC) $(LIB_INC) -MMD -MP -c $< -o $@

$(BUILD_DIR)/generator_%.o: $(GEN_DIR)/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(GEN_INC) $(VISUAL_INC) $(LIB_INC) -MMD -MP -c $< -o $@

$(BUILD_DIR)/duo_client_%.o: $(DUO_CLIENT_DIR)/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(DUO_CLIENT_INC) $(VISUAL_INC) $(LIB_INC) -MMD -MP -c $< -o $@

# --- Include dependency files ---
-include $(LIB_DEP)
-include $(VISUAL_DEP)
-include $(DEMO_DEP)
-include $(GEN_DEP)
-include $(DUO_CLIENT_DEP)

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
