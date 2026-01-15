NAME = pars

PROG =	main\
		init\
		StringUtils\


SRC = $(addsuffix .cpp, $(PROG))

OBJ = $(addsuffix .o, $(PROG))

CC = c++

CFLAGS = -Wall -Wextra -Werror

all :		$(NAME)

$(NAME) :	$(OBJ)
			$(CC) $(OBJ) -o $(NAME)

%.o :		%.cpp
			$(CC) -c $< $(CFLAGS) -o $@

clean :
			rm -f $(OBJ)

fclean : 	clean
			rm -f $(NAME)

re :		fclean all

a :			all clean

valgrind :	$(NAME)
			valgrind --leaks-check=full --show-leaks-kinds=all --track-origins=yes ./$(NAME)

.PHONY :	all clean fclean re a valgrind
