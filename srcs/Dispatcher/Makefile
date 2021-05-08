SRCS        = 	Dispatcher.cpp \
				DispatcherCGI.cpp \
				DispatcherNegotiate.cpp \
				DispatcherStatusCode.cpp \
				DispatcherUtils.cpp
OBJS        =   $(SRCS:.cpp=.o)

DIR_HEADERS =   ../../incs
DIR_OBJS	=	../../compiled_srcs

CC          =   clang++
CC_FLAGS    =   -Wall -Wextra -Werror
RM          =   rm -rf
NAME        =   dispatcher



all			: $(NAME)

clean		:
	$(RM) $(OBJS)
fclean		: clean

re			: fclean all


$(NAME)		: $(DIR_OBJS)
			$(CC) -c $(CC_FLAGS) $(SRCS) -I $(DIR_HEADERS)
			mv $(OBJS) $(DIR_OBJS)
$(DIR_OBJS)	:
			mkdir $(DIR_OBJS)