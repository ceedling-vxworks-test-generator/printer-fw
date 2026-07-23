# printer-fw 簡易 Makefile（Linux/POSIX 向け。cmake が無い環境の代替）
#   make            … 静的/共有ライブラリ + デモ + テストをビルド
#   make test       … テスト実行
#   make run        … デモ実行（Ubuntu 端末 / TeraTerm にログ表示）
#   sudo make install PREFIX=/usr/local
#   make clean
#
# 実装は組込み向けイディオマティックC++（-fno-exceptions -fno-rtti。動的確保・STLコンテナ不使用）。
# 公開ヘッダ・APIは C ABI 互換（extern "C"）のため、C からもリンク可能（要 libstdc++ リンク）。
# 本格運用は CMake を推奨（find_package / pkg-config を提供）。本 Makefile は動作確認・簡易組込み用。

CXX     ?= g++
AR      ?= ar
CXXFLAGS ?= -std=c++17 -Wall -Wextra -fno-exceptions -fno-rtti -O2
PREFIX  ?= /usr/local

INC      := -Iinclude
BUILD    := build

CORE_SRC := src/pf_core.cpp src/pf_data.cpp src/pf_state.cpp src/pf_fsm.cpp \
            src/pf_monitor.cpp src/pf_observer.cpp src/pf_result.cpp src/pf_log.cpp
CORE_OBJ := $(patsubst src/%.cpp,$(BUILD)/obj/%.o,$(CORE_SRC))

SAMPLE_SRC := models/model_sample/model_sample.cpp \
              port/pf_port_baremetal.cpp port/pf_port_linux.cpp
SAMPLE_INC := -Iport -Imodels/model_sample

.PHONY: all lib test run install clean
all: $(BUILD)/libprinter_fw.a $(BUILD)/libprinter_fw.so $(BUILD)/app_demo $(BUILD)/pf_tests

lib: $(BUILD)/libprinter_fw.a $(BUILD)/libprinter_fw.so

$(BUILD)/obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -fPIC -c $< -o $@

$(BUILD)/libprinter_fw.a: $(CORE_OBJ)
	@mkdir -p $(BUILD)
	$(AR) rcs $@ $(CORE_OBJ)

$(BUILD)/libprinter_fw.so: $(CORE_OBJ)
	@mkdir -p $(BUILD)
	$(CXX) -shared -o $@ $(CORE_OBJ)

$(BUILD)/app_demo: $(BUILD)/libprinter_fw.a
	$(CXX) $(CXXFLAGS) $(INC) $(SAMPLE_INC) examples/app_demo.cpp $(SAMPLE_SRC) \
	    -L$(BUILD) -lprinter_fw -lpthread -o $@

$(BUILD)/pf_tests: $(BUILD)/libprinter_fw.a
	$(CXX) $(CXXFLAGS) $(INC) $(SAMPLE_INC) tests/test_printer_fw.cpp $(SAMPLE_SRC) \
	    -L$(BUILD) -lprinter_fw -lpthread -o $@

test: $(BUILD)/pf_tests
	./$(BUILD)/pf_tests

run: $(BUILD)/app_demo
	./$(BUILD)/app_demo

install: lib
	install -d $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include/printer_fw
	install -m644 $(BUILD)/libprinter_fw.a  $(DESTDIR)$(PREFIX)/lib/
	install -m644 $(BUILD)/libprinter_fw.so $(DESTDIR)$(PREFIX)/lib/
	install -m644 include/printer_fw/*.h    $(DESTDIR)$(PREFIX)/include/printer_fw/

clean:
	rm -rf $(BUILD)
