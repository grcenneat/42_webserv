# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yonglee <yonglee@student.42seoul.kr>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/04/27 14:54:02 by ijuhae            #+#    #+#              #
#    Updated: 2021/05/03 23:53:24 by yonglee          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# This is a minimal set of ANSI/VT100 color codes
_END		=	\e[0m
_BOLD		=	\e[1m
_UNDER		=	\e[4m
_REV		=	\e[7m
# Colors
_GREY		=	\e[30m
_RED		=	\e[31m
_GREEN		=	\e[32m
_YELLOW		=	\e[33m
_BLUE		=	\e[34m
_PURPLE		=	\e[35m
_CYAN		=	\e[36m
_WHITE		=	\e[37m
# Inverted, i.e. colored backgrounds
_IGREY		=	\e[40m
_IRED		=	\e[41m
_IGREEN		=	\e[42m
_IYELLOW	=	\e[43m
_IBLUE		=	\e[44m
_IPURPLE	=	\e[45m
_ICYAN		=	\e[46m
_IWHITE		=	\e[47m
# COMPILATION #
CC			=	clang++
CC_FLAGS	=	-Wall -Wextra -Werror
# COMMANDS #
RM			=	rm -rf
# DIRECTORIES #
DIR_HEADERS =	./incs/
DIR_SRCS	=	./srcs/
DIR_OBJS	=	./compiled_srcs/
DIR_CONFIGS =	./conf/
DIR_DIS		=	./srcs/Dispatcher
# FILES #
SRC			=	Config.cpp \
				Server.cpp \
				utils.cpp \
				main.cpp \
				ServerTest.cpp \
				Logger.cpp \
				Client.cpp \
				Parser.cpp
SRCS		=	$(SRC)
# COMPILED_SOURCES #
OBJS 		=	$(SRCS:%.cpp=$(DIR_OBJS)%.o)
OBJ			= 	*.o
NAME 		=	webserv
CONFIG		=	webserv.conf
CONFIG2		=	webserv2.conf
# **************************************************************************** #
## RULES ##
all:			$(NAME) $(DIR_CONFIGS)$(CONFIG) $(DIR_CONFIGS)$(CONFIG2)
# VARIABLES RULES #
$(NAME):		$(OBJS)
				make -C $(DIR_DIS)
				@printf "\033[2K\r$(_GREEN) All files compiled into '$(DIR_OBJS)'. $(_END)✅\n"
				$(CC) $(CC_FLAGS) $(DIR_OBJS)$(OBJ) -I $(DIR_HEADERS) -o $(NAME)
				@printf "$(_GREEN) Executable '$(NAME)' created. $(_END)✅\n"
# COMPILED_SOURCES RULES #
$(OBJS):		| $(DIR_OBJS)
$(DIR_OBJS)%.o: $(DIR_SRCS)%.cpp
				$(CC) $(CC_FLAGS) -I $(DIR_HEADERS) -c $< -o $@
$(DIR_OBJS):
				mkdir $(DIR_OBJS)
$(DIR_CONFIGS)$(CONFIG):
				@cat $(DIR_CONFIGS)webserv_model.conf | sed 's=PWD=$(PWD)=g' > $(DIR_CONFIGS)$(CONFIG)
				@printf "\033[2K\r$(_GREEN) Default config file '$(DIR_CONFIGS)$(CONFIG)' created. $(_END)✅\n"
$(DIR_CONFIGS)$(CONFIG2):
				@cat $(DIR_CONFIGS)webserv_model2.conf | sed 's=PWD=$(PWD)=g' > $(DIR_CONFIGS)$(CONFIG2)
				@printf "\033[2K\r$(_GREEN) Default config file '$(DIR_CONFIGS)$(CONFIG2)' created. $(_END)✅\n"
# OBLIGATORY PART #
clean:
				make clean -C $(DIR_DIS)
				@$(RM) $(DIR_OBJS)
				@printf "$(_RED) '"$(DIR_OBJS)"' has been deleted. $(_END)🗑️\n"
fclean:			clean
				make fclean -C $(DIR_DIS)
				@$(RM) $(NAME)
				@printf "$(_RED) '"$(NAME)"' has been deleted. $(_END)🗑️\n"
				@$(RM) $(DIR_CONFIGS)$(CONFIG)
				@printf "$(_RED) '"$(DIR_CONFIGS)$(CONFIG)"' has been deleted. $(_END)🗑️\n"
				@$(RM) $(DIR_CONFIGS)$(CONFIG2)
				@printf "$(_RED) '"$(DIR_CONFIGS)$(CONFIG2)"' has been deleted. $(_END)🗑️\n"
re:				fclean all
.PHONY:			all clean fclean re bonus re_bonus
