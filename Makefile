NAME        = Webserv

CPP         = c++
CPPFLAGS    = -Wall -Wextra -Werror -std=c++98 -MMD -MP

LIBFT_DIR   = lib/LIBFTPP
LIBFT_A     = $(LIBFT_DIR)/libftpp.a
INCFLAGS    = -I include -I $(LIBFT_DIR)/include

SRC_DIR     = src
OBJ_DIR     = obj

SRCS        = $(wildcard $(SRC_DIR)/*.cpp)
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS        = $(OBJS:.o=.d)

RM          = rm -rf

# Docker Compose command ("docker compose" on recent setups, "docker-compose" on older ones)
COMPOSE     := $(shell \
	if docker compose version > /dev/null 2>&1; then \
		echo 'docker compose'; \
	elif command -v docker-compose > /dev/null 2>&1; then \
		echo 'docker-compose'; \
	else \
		echo ''; \
	fi)

all: $(NAME)


$(NAME): $(LIBFT_A) $(OBJS)
	@if [ -z "$(strip $(OBJS))" ]; then \
		echo "Error: no sources found in $(SRC_DIR)/ (*.cpp)."; \
		echo "Hint: add at least $(SRC_DIR)/main.cpp with an int main()."; \
		exit 1; \
	fi
	$(CPP) $(CPPFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CPP) $(CPPFLAGS) $(INCFLAGS) -c $< -o $@

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR) static

clean:
	$(RM) $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

-include $(DEPS)

colima-start:
	@if command -v colima > /dev/null; then \
		colima status > /dev/null 2>&1 || colima start; \
	fi

up: colima-start
	@if [ -z "$(COMPOSE)" ]; then echo "Error: neither 'docker compose' nor 'docker-compose' found."; exit 1; fi
	$(COMPOSE) up -d --build

down:
	@if [ -z "$(COMPOSE)" ]; then echo "Error: neither 'docker compose' nor 'docker-compose' found."; exit 1; fi
	$(COMPOSE) down

join:
	docker exec -it webserv_tester /bin/zsh

logs:
	@if [ -z "$(COMPOSE)" ]; then echo "Error: neither 'docker compose' nor 'docker-compose' found."; exit 1; fi
	$(COMPOSE) logs -f

re-docker: down up

.PHONY: all clean fclean re colima-start up down join logs re-docker