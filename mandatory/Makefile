NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -I ../libft/include \
         -I ~/.brew/opt/readline/include -I ~/.brew/opt/ncurses/include

LFLAGS = -L ../libft -L ~/.brew/opt/readline/lib -L ~/.brew/opt/ncurses/lib \
         -l ft -l readline -l ncurses


LIBFT = ../libft/libft.a 

SRC = shell.c wrappers.c errors.c expand.c tokenize.c utils/cmdline_utils.c \
utils/tokenize_utils.c classed_list.c create_ast.c

all: $(NAME)

$(NAME): shell.h $(LIBFT) $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LFLAGS) -o $(NAME)

$(LIBFT):
	make -C ../libft

clean:
	make -C ../libft clean

fclean: clean
	rm -f $(NAME)
	make -C ../libft fclean

re: fclean all

.PHONY: all clean fclean re bonus