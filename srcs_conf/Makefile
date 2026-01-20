NAME = pars

PROG =	main\
		RouteConfig\
		ServerConfig\
		state\
		directive\
		print\
		StringUtils\

OBJ_DIR = obj

SRC = $(addsuffix .cpp, $(PROG))

OBJ = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(PROG)))

CC = g++

#CFLAGS =  -Wall -Wextra -Werror
CFLAGS =  -Wall -Wextra -Werror -std=c++98
#CFLAGS =  -Wall -Wextra -fsanitize=thread -g

all:			$(OBJ_DIR) $(NAME)

$(OBJ_DIR):
				@mkdir -p $(OBJ_DIR)

$(NAME):		$(OBJ)
				@$(CC) $(OBJ) $(CFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o:	%.cpp
				@$(CC) $(CFLAGS) -c $< -o $@

clean:
				@rm -f $(OBJ)

fclean:			clean
				@rm -f $(NAME)

re:				fclean all

a:				fclean all clean

.PHONY:			all clean fclean re a

# valgrind --leak-check=full --show-leak-kinds=all ./programme