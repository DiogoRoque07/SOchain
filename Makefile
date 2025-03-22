################################################################################
#                                     CONFIG                                   #
################################################################################

NAME        := SOchain
CC          := gcc
FLAGS       := -g #-Wall -Wextra -Werror 
RUN			:= ./bin/$(NAME)
RUN_ARGS    := 200 3 1 5 10 
################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

SRCS        := $(wildcard src/*.c)
OBJS        := $(SRCS:src/%.c=obj/%.o)

################################################################################
#                                  Makefile  objs                              #
################################################################################

CLR_RMV     := \033[0m
RED         := \033[1;31m
GREEN       := \033[1;32m
YELLOW      := \033[1;33m
BLUE        := \033[1;34m
CYAN        := \033[1;36m
PINK        := \033[1;35m
RM          := rm -f

all:        ${NAME}

${NAME}:    ${OBJS}
			clear
			@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}$(NAME) ${CLR_RMV}..."
			@${CC} ${FLAGS} -o ./bin/${NAME} ${OBJS}
			@echo "$(GREEN)$(NAME) created$(CLR_RMV)✔️"

obj/%.o: src/%.c
			mkdir -p obj
			@${CC} ${FLAGS} -c $< -o $@

clean:
			@ ${RM} obj/*.o
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✔️"

fclean:     clean
			clear
			@${RM} ./bin/${NAME}
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV) ✔️"

re:         fclean all

run: all

	@ echo "$(BLUE)compilação de $(CYAN)$(NAME) $(GREEN) feita $(CLR_RMV) ✔️"
	@# @sleep 2
	@ # clear
	@$(RUN) $(RUN_ARGS) 

mem: all
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=val.log $(RUN) $(RUN_ARGS)

debug: all
	@gdbtui --args $(RUN) $(RUN_ARGS)

install_memTest:
	@sudo apt install valgrind

.PHONY:     all clean fclean re
