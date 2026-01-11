# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: libftpp                                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/23                              #+#    #+#              #
#    Updated: 2025/10/23                             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Nom de la bibliothèque
NAME = libftpp

# Extensions selon l'OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    SHARED_EXT = .dylib
    SHARED_FLAGS = -dynamiclib -install_name @rpath/$(NAME)$(SHARED_EXT)
else
    SHARED_EXT = .so
    SHARED_FLAGS = -shared
endif

# Noms des bibliothèques
STATIC_LIB = $(NAME).a
SHARED_LIB = $(NAME)$(SHARED_EXT)

# Compilateur et flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXXFLAGS_SHARED = $(CXXFLAGS) -fPIC

# Répertoires
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
OBJ_DIR_STATIC = $(OBJ_DIR)/static
OBJ_DIR_SHARED = $(OBJ_DIR)/shared

# Ajouter le chemin des includes s'il existe
ifneq ($(wildcard $(INC_DIR)),)
    CXXFLAGS += -I$(INC_DIR)
    CXXFLAGS_SHARED = $(CXXFLAGS) -fPIC
endif

# Trouver tous les fichiers .cpp récursivement
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Si pas de src/ trouvé, utiliser wildcard à la racine
ifeq ($(SRCS),)
    SRCS = $(wildcard *.cpp)
endif

# Objets pour bibliothèque statique
OBJS_STATIC = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR_STATIC)/%.o,$(SRCS))
ifeq ($(SRCS),$(wildcard *.cpp))
    OBJS_STATIC = $(patsubst %.cpp,$(OBJ_DIR_STATIC)/%.o,$(SRCS))
endif

# Objets pour bibliothèque dynamique
OBJS_SHARED = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR_SHARED)/%.o,$(SRCS))
ifeq ($(SRCS),$(wildcard *.cpp))
    OBJS_SHARED = $(patsubst %.cpp,$(OBJ_DIR_SHARED)/%.o,$(SRCS))
endif

# Couleurs
GREEN = \033[0;32m
BLUE = \033[0;34m
RED = \033[0;31m
YELLOW = \033[0;33m
NC = \033[0m

# Règles
.PHONY: all static dev clean fclean re help

# Par défaut: compile les deux
all: static dev

# Compilation bibliothèque statique
static: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS_STATIC)
	@echo "$(BLUE)Creating static library $(STATIC_LIB)...$(NC)"
	@ar rcs $(STATIC_LIB) $(OBJS_STATIC)
	@echo "$(GREEN)✓ Static library $(STATIC_LIB) created successfully!$(NC)"

# Compilation bibliothèque dynamique
dev: $(SHARED_LIB)

$(SHARED_LIB): $(OBJS_SHARED)
	@echo "$(BLUE)Creating shared library $(SHARED_LIB)...$(NC)"
	@$(CXX) $(SHARED_FLAGS) -o $(SHARED_LIB) $(OBJS_SHARED)
	@echo "$(GREEN)✓ Shared library $(SHARED_LIB) created successfully!$(NC)"

# Compilation des objets statiques
$(OBJ_DIR_STATIC)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling (static): $<$(NC)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Cas où les sources sont à la racine
$(OBJ_DIR_STATIC)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling (static): $<$(NC)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilation des objets dynamiques
$(OBJ_DIR_SHARED)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling (shared): $<$(NC)"
	@$(CXX) $(CXXFLAGS_SHARED) -c $< -o $@

# Cas où les sources sont à la racine
$(OBJ_DIR_SHARED)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling (shared): $<$(NC)"
	@$(CXX) $(CXXFLAGS_SHARED) -c $< -o $@

# Nettoyage des objets
clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✓ Object files cleaned!$(NC)"

# Nettoyage complet
fclean: clean
	@echo "$(RED)Cleaning libraries...$(NC)"
	@rm -f $(STATIC_LIB) $(SHARED_LIB)
	@echo "$(GREEN)✓ Libraries cleaned!$(NC)"

# Recompilation
re: fclean all

# Aide
help:
	@echo "$(BLUE)Available targets:$(NC)"
	@echo "  $(GREEN)all$(NC)      - Compile both static and shared libraries (default)"
	@echo "  $(GREEN)static$(NC)   - Compile static library (.a)"
	@echo "  $(GREEN)dev$(NC)      - Compile shared library (.so/.dylib)"
	@echo "  $(GREEN)clean$(NC)    - Remove object files"
	@echo "  $(GREEN)fclean$(NC)   - Remove object files and libraries"
	@echo "  $(GREEN)re$(NC)       - Recompile everything"
	@echo "  $(GREEN)help$(NC)     - Show this help message"
	@echo ""
	@echo "$(YELLOW)OS detected: $(UNAME_S)$(NC)"
	@echo "$(YELLOW)Shared library extension: $(SHARED_EXT)$(NC)"