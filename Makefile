NAME        = Webserv

CPP         = c++
CPPFLAGS    = -Wall -Wextra -Werror -std=c++98 -MMD -MP

LIBFT_DIR   = lib/LIBFTPP
LIBFT_A     = $(LIBFT_DIR)/libftpp.a
INCFLAGS    = -I include -I $(LIBFT_DIR)/include

SRC_DIR     = src
OBJ_DIR     = obj
TEST_DIR	= test

SRCS        = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS        = $(OBJS:.o=.d)

# pour les test du parsing
TEST_NAME = test_request
TEST_SRC    = $(TEST_DIR)/testRequest.cpp
TEST_OBJ    = $(OBJ_DIR)/test/testRequest.o
OBJS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

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
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) $(INCFLAGS) -c $< -o $@

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR) static

	# ==================== TEST TARGETS ====================
test: $(LIBFT_A) $(OBJS_NO_MAIN) $(TEST_OBJ)
		$(CPP) $(CPPFLAGS) $(OBJS_NO_MAIN) $(TEST_OBJ) $(LIBFT_A) -o $(TEST_NAME)
		@echo "\n Test binary built: ./$(TEST_NAME)\n"

$(OBJ_DIR)/test/%.o: $(TEST_DIR)/%.cpp
		@mkdir -p $(dir $@)
		$(CPP) $(CPPFLAGS) $(INCFLAGS) -c $< -o $@

test-run: test
		@echo "\n========== Running Tests ==========\n"
		./$(TEST_NAME)

test-clean:
		$(RM) $(TEST_NAME) $(OBJ_DIR)/test
# ======================================================

clean:
	$(RM) $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME) $(TEST_NAME)
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

pull-libftpp:
	git fetch libftpp
	git subtree pull --prefix=lib/LIBFTPP libftpp main --squash

push-libftpp:
	git subtree push --prefix=lib/LIBFTPP libftpp main

.PHONY: all clean fclean re colima-start up down join logs re-docker test-run test-clean
