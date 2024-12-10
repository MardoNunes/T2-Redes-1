# Flags de compilação
CFLAGS = -Wall -g
LDFLAGS = 

# Objetos necessários
objs = t2.o librede.o

# Nome do executável
target = t2

# Regra padrão para construir o executável
all: $(target)

# Construção do executável a partir dos objetos
$(target): $(objs)
	gcc $(objs) -o $(target) $(LDFLAGS)

# Regra para compilar t2.c
t2.o: t2.c librede.h
	gcc $(CFLAGS) -c t2.c -o t2.o

# Regra para compilar librede.c
librede.o: librede.c librede.h
	gcc $(CFLAGS) -c librede.c -o librede.o

# Limpeza dos arquivos gerados
clean:
	rm -f $(target) $(objs) *~

# Limpeza total (inclui outros arquivos temporários)
purge: clean
	rm -f $(target)
