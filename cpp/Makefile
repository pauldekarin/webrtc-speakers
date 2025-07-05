BUILD_DIR=build/
CMAKE=cmake
BIN_FILE=webrtc-speakers

.PHONY:all
all:exec

.PHONY:
exec: build
	./$(BUILD_DIR)$(BIN_FILE)

.PHONY:build
build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) .. 
	cd $(BUILD_DIR) && $(MAKE)

.PHONY:clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY:rebuild
rebuild: clean build

