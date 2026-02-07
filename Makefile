GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
YELLOW = \033[0;33m
NC = \033[0m

NAME        = Webserv

CPP         = c++
CPPFLAGS    = -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3

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


TOTAL_OBJS  = $(words $(SRCS))
PROGRESS_FILE = $(OBJ_DIR)/.progress
PROGBAR_LEN = 40

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
	@echo "$(GREEN)[Webserv] All objects compiled!$(NC)"
	@$(CPP) $(CPPFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME)
	@echo "$(BLUE)Linking $(NAME)...$(NC)"
	@echo "$(GREEN)all is DONE$(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CPP) $(CPPFLAGS) $(INCFLAGS) -c $< -o $@
	@count=$$(cat $(PROGRESS_FILE) 2>/dev/null || echo 0); \
	total=$(TOTAL_OBJS); [ $$total -le 0 ] && total=1; \
	[ $$count -ge $$total ] && count=0; \
	count=$$((count+1)); echo $$count > $(PROGRESS_FILE); \
	percent=$$(( (count*100)/total )); \
	bar_len=$(PROGBAR_LEN); filled=$$(( (percent*bar_len)/100 )); \
	bar=""; i=0; while [ $$i -lt $$filled ]; do bar="$$bar#"; i=$$((i+1)); done; \
	printf "\r[%-*s] %3d%% (%d/%d) Compiling: %s" $$bar_len "$$bar" $$percent $$count $$total "$<"; \
	[ $$count -eq $$total ] && printf "\n" || true

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
	@echo "$(RED)Cleaning object files...$(NC)"
	$(RM) $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean > /dev/null 2>&1
	@echo "\033[32m[webserv] Clean done!\033[0m"

fclean: clean
	@echo "$(RED)Cleaning executable and libraries...$(NC)"
	$(RM) $(NAME)  $(TEST_NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean > /dev/null 2>&1
	@echo "\033[32m[webserv] Full clean done!\033[0m"

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
	docker-compose up -d
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
