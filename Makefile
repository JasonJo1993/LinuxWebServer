NAME=sws
SRC=net.c http.c
OBJ=$(SRC:.c=.o)
RM = rm -f
IPATH = .
CFLAGS += -I$(IPATH)
LDFLAGS += -Llib
LDLIBS += -lsws
all: $(NAME) fclean
$(NAME):$(OBJ)
	ar -rc ./libsws.a $(OBJ)
	ranlib ./libsws.a
	cc -L$(IPATH) -I$(IPATH) main.c -lsws -o sws
clean:
	$(RM) *~
	$(RM) *.o
	$(RM) *.core
	$(RM) *.swp
	$(RM) \#*
fclean: clean
	$(RM) ./libsws.a
re: fclean all
