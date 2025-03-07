################################################################################
#                                     CONFIG                                   #
################################################################################

NAME        := SOchain
CC          := gcc
FLAGS       := -Wall -Wextra -Werror 
################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

SRCS        := src/main.c
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
RM          := rm -f

all:        ${NAME}

${NAME}:    ${OBJS}
			@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}$(NAME) ${CLR_RMV}..."
			@${CC} ${FLAGS} -o ./bin/${NAME} ${OBJS}
			@echo "$(GREEN)$(NAME) created[0m ✔️"

obj/%.o: src/%.c
			@mkdir -p obj
			@${CC} ${FLAGS} -c $< -o $@

clean:
			@ ${RM} obj/*.o
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✔️"

fclean:     clean
			@${RM} ./bin/${NAME}
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV) ✔️"

re:         fclean all

run: all
	@ echo "$(BLUE)compilação de $(CYAN)$(NAME) $(GREEN) feita $(CLR_RMV) ✔️"

	@./bin/$(NAME)

.PHONY:     all clean fclean re
