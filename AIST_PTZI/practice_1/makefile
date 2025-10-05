CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 -Iinclude
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2 -DNDEBUG
LDFLAGS := -lgmp

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
TARGET_DIR := bin

DEMO_SOURCE := $(SRC_DIR)/demo.cpp
DEMO_GENERATOR_SOURCE := $(SRC_DIR)/demo_generator.cpp

SOURCES := $(filter-out $(DEMO_SOURCE) $(DEMO_GENERATOR_SOURCE), $(wildcard $(SRC_DIR)/*.cpp))
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

DEMO_OBJECT := $(BUILD_DIR)/demo.o
DEMO_GENERATOR_OBJECT := $(BUILD_DIR)/demo_generator.o

DEMO_TARGET := $(TARGET_DIR)/demo
DEMO_GENERATOR_TARGET := $(TARGET_DIR)/demo_generator

all: $(DEMO_TARGET) $(DEMO_GENERATOR_TARGET)

$(DEMO_TARGET): $(OBJECTS) $(DEMO_OBJECT) | $(TARGET_DIR)
	$(CXX) $(OBJECTS) $(DEMO_OBJECT) -o $@ $(LDFLAGS)

$(DEMO_GENERATOR_TARGET): $(OBJECTS) $(DEMO_GENERATOR_OBJECT) | $(TARGET_DIR)
	$(CXX) $(OBJECTS) $(DEMO_GENERATOR_OBJECT) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEMO_OBJECT): $(DEMO_SOURCE) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEMO_GENERATOR_OBJECT): $(DEMO_GENERATOR_SOURCE) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

release: CXXFLAGS += $(RELEASE_FLAGS)
release: all

demo: $(DEMO_TARGET)
demo_generator: $(DEMO_GENERATOR_TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET_DIR)

rebuild: clean all

help:
	@echo "Available targets:"
	@echo "  all       - build demo application"
	@echo "  demo      - build demo application"
	@echo "  generator - build demo generator application"
	@echo "  debug     - build with debug information"
	@echo "  release   - build with optimizations"
	@echo "  clean     - remove build artifacts"
	@echo "  rebuild   - clean and rebuild everything"
	@echo "  help      - show this help"

.PHONY: all debug release demo generator clean rebuild install help

-include $(OBJECTS:.o=.d)
-include $(DEMO_OBJECT:.o=.d)
-include $(DEMO_GENERATOR_OBJECT:.o=.d)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@