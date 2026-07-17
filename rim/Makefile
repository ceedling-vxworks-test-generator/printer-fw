# RIM (ReactiveInfoManager) skeleton build
#   - 内部実装は C++（-fno-exceptions -fno-rtti）
#   - 公開IFは extern "C"。デモは .c（gcc）でビルドし、C ABI越しに使用する。
#   - C++実体を含むため、Cデモのリンクには -lstdc++ を付す。

CXX      ?= g++
CC       ?= gcc
CXXFLAGS ?= -std=c++17 -fno-exceptions -fno-rtti -Wall -Wextra -O2 -Iinclude
CFLAGS   ?= -std=c11 -Wall -Wextra -O2 -Iinclude

BUILD    := build
SRCS     := src/rim_list.cpp src/rim_adapter.cpp src/rim_datastore.cpp \
            src/rim_capability.cpp src/rim_publisher.cpp src/rim_core.cpp \
            src/rim_port.cpp
OBJS     := $(patsubst src/%.cpp,$(BUILD)/%.o,$(SRCS))
LIB      := $(BUILD)/librim.a

.PHONY: all run clean
all: $(BUILD)/rim_demo

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB): $(OBJS)
	ar rcs $@ $(OBJS)

# CデモをCコンパイラでビルドし、C++実体ライブラリ＋libstdc++とリンク
$(BUILD)/rim_demo: examples/rim_demo.c $(LIB)
	$(CC) $(CFLAGS) examples/rim_demo.c $(LIB) -lstdc++ -o $@

run: $(BUILD)/rim_demo
	$(BUILD)/rim_demo

clean:
	rm -rf $(BUILD)
