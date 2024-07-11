NAME = libft.a
FLAGS = -Wall -Wextra -Werror
SOURCES = *.c #arquivos em formato .c
OBJECT = *.o #arquivos em formato .o que vieram dos .c
COMPILATION = cc

all: ${NAME}

${NAME}: ${OBJECT}
	ar rc ${NAME} ${OBJECT}

${OBJECT}: ${SOURCES}
	cc ${FLAGS} -c ${SOURCES}
	
clean:
	rm -f ${OBJECT}

fclean:
	rm -f ${OBJECT} ${NAME}

re: ${fclean}
	${fclean} ${all}