DIR_BUILD	:= build

CMD_MKDIR	:= mkdir -p
CMD_RM := rm

.PHONY: all
all: build

.PHONY: build
build: generate
	cmake --build ./$(DIR_BUILD)

.PHONY: generate
generate:
	mkdir -p ./$(DIR_BUILD)
	cmake -B ./$(DIR_BUILD)

.PHONY: clean
clean:
	$(CMD_RM) --force --recursive ./$(DIR_BUILD)/*

.PHONY: unittest
unittest:
	@./build/test/unit_testsuite

.PHONY: memcheck
memcheck:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --error-exitcode=1 ./build/unit_testsuite-text 2>&1 | sed -n "/SUMMARY/,$$$$p"
