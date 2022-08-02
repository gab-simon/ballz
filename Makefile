      PROGS = main

  ALLEGRO = -lallegro_image  -lallegro_primitives  -lallegro_dialog  -lallegro_ttf -lallegro_font  -lallegro

     CFLAGS = -I .
     LFLAGS = $(ALLEGRO)

       CC = gcc -std=c99 -Wall

%.o:  %.c
	$(CC) $(CFLAGS) -c $<

debug: CFLAGS += -g -D__DEBUG__

all debug: $(PROGS)

$(PROGS) : % : %.o grafico.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

clean:
	@echo "Limpando sujeira ..."
	@rm -f *% *.bak *~

purge: clean
	@echo "Limpando tudo ..."
	@rm -rf core *.o
	@rm -f $(PROGS)

