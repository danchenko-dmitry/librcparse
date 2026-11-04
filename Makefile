# Build crsf-stream-viewer without OpenTelemetry.
#   make build
# ETL headers come from a sibling uart-rp-test checkout.
# Override with: make build UART_RP_TEST_ROOT=/path/to/uart-rp-test

UART_RP_TEST_ROOT ?= $(abspath ../uart-rp-test)

CXX ?= c++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Werror
CPPFLAGS := -Icrsf -Ipacket_catcher -I$(UART_RP_TEST_ROOT)/modules/etl/include

BUILD := build
TARGET := $(BUILD)/bin/crsf-stream-viewer

SRCS := \
	crsf-stream-viewer/main.cpp \
	crsf/crsf.cpp \
	crsf/crsf_callbacks.cpp \
	crsf/crc.cpp \
	packet_catcher/packet_catcher.cpp

OBJS := $(patsubst %.cpp,$(BUILD)/obj/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.DEFAULT_GOAL := build

.PHONY: build clean

build: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD)/obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
