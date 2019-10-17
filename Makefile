.DEFAULT_GOAL=help
.PHONY=all

#-----------------------------------------------#
#                                               #
#    		Compilation options             #
#                                               #
#-----------------------------------------------#

# Compiler
CXX=g++

# Compiler flags
CXX_FLAGS=-lm -Wall

#-----------------------------------------------#
#                                               #
#                    Recipes                    #
#                                               #
#-----------------------------------------------#

help: ## Show this help.
	@printf "\033[32m%-30s     \033[32m %s\n" "VARIABLE NAME" "DEFAULT_VALUE"
	@grep -E '^[a-zA-Z_-]+(\?=|=).*$$' $(MAKEFILE_LIST) |sort | awk 'BEGIN {FS = "(?=|=)"}; {printf "\033[36m%-30s     \033[0m %s\n", $$1, $$2}'
	@printf "\n\033[32m%-30s     \033[32m %s\033[0m\n" "RECIPE" "DESCRIPTION"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "make \033[36m%-30s\033[0m %s\n", $$1, $$2}'

parser.tab.c: parser.y ## Build scanner
	@echo "Building parser..."
	bison parser.y
	@echo "Done" && echo 

lex.yy.c: parser.tab.c scanner.flex ## Build scanner
	@echo "Building scanner..."
	flex scanner.flex
	@echo "Done" && echo 

clean: ## Clean builded files
	@echo "Cleaning..."
	rm -rf parser.tab.c
	rm -rf lex.yy.c
	rm -rf program.exe
	rm -rf program
	@echo "Done" && echo 

program: build ## Build and fix program executable
	@[ -f "program.exe" ] && mv "program.exe" "program" || return 0

build: parser.tab.c lex.yy.c ## Build the program
	@echo "Building program..."
	@echo "=============== G++ Output ==============="
	$(CXX) lex.yy.c -o program $(CXX_FLAGS)
	@echo "=========================================="
	@echo "Done" && echo 

run: program ## Run the program
	./program source.ect

all:
	rm -rf calculette
	flex -o calculette.lex.cpp calculette.l
	bison -d calculette.y -o calculette.bison.cpp
	g++ calculette.lex.cpp calculette.bison.cpp -o calculette
	rm -rf calculette.lex.cpp calculette.bison.cpp calculette.bison.hpp
	./calculette