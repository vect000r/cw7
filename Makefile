FC = gcc
LINKER = -Wall -std=c99 -pedantic

NAME1 = cw7
NAME2 = producer
NAME3 = consumer

EXEC1 = $(NAME1).x
OBJS1 = $(NAME1).o

EXEC2 = $(NAME2).x
OBJS2 = $(NAME2).o

EXEC3 = $(NAME3).x
OBJS3 = $(NAME3).o

%.o: %.c
	$(FC) -c $<

%.x: %.o
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

$(EXEC1): $(OBJS1) 
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

$(EXEC2): $(OBJS2) 
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

$(EXEC3): $(OBJS3) 
	$(FC) $(LINKER) -o $@ $^
	chmod +x $@

run-cw7: $(EXEC1) 
	./$(EXEC1)

.PHONY: all
all: $(EXEC1) $(EXEC2) $(EXEC3)

help:
	@echo "make run-cw7		Kompiluje i uruchamia programy powielacz.c i wykluczanie.c"
	@echo "make all			Kompiluje programy powielacz.c i wykluczanie.c"
	@echo "make clean  		Usuwa pliki .x, .o"

.PHONY: clean
clean:
	@-rm *.x *.o  