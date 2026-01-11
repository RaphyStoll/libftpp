NAME = miniserv

SRCS = main.cpp

LIBFTPP_DIR = ../../lib/LIBFTPP
LIBFTPP = $(LIBFTPP_DIR)/libftpp.a

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

all: 
	make re -C ../../lib/LIBFTPP && c++ -Wall -Wextra -Werror -std=c++98 main.cpp ../../lib/LIBFTPP/libftpp.a -o $(NAME)

fclean: clean
	rm -f $(NAME)

re: clean all

.PHONY: all clean fclean re
