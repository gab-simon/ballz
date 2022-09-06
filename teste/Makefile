
CFLAGS = -I . $(INCDIR) -std=c99 -Wall
EXEC_NAME =ballz
OBJS =ballz.o grafico.o game_helpers.o
ALLEGRO =-lallegro_image -lallegro_primitives -lallegro_dialog -lallegro_ttf -lallegro_font -lallegro -lallegro_main
LIBS =-lm $(ALLEGRO)
CC =gcc

all: $(EXEC_NAME)

all debug: $(EXEC_NAME)
debug: CFLAGS += -g -D__DEBUG__

$(EXEC_NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $(EXEC_NAME)

ballz.o: ballz.c
grafico.o: grafico.c grafico.h
game_helpers.o: game_helpers.c game_helpers.h

clean:
	rm -f *.o *.bak *~

purge: clean
	rm -f $(EXEC_NAME)