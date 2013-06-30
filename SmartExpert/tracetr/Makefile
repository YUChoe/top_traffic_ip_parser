CC      = gcc
SRC     = tracetr.c
EXEC    = ../../bin/tracetrplus
LIB     = -lpthread -lpcap

tracetr : tracetr.c tracetr.h ../public/public.h
	$(CC) -o $(EXEC) $(SRC) $(LIB)

pfring : tracetr.c tracetr.h ../public/public.h
	$(CC) -o $(EXEC) $(SRC) $(LIB) -lpfring

debug :
	$(CC) -g -o $(EXEC) $(SRC) $(LIB)

mem :
	$(CC) -g $(SRC) ../public/memwatch.c -o $(EXEC) $(LIB) -DMEMWATCH -DMW_STDIO -DMEMWATCH_LOG_FILE="\"tracetr.log\""

efence : cta.c cta.h ../public/public.h ../public/inittcp.h ../public/conuds.h ../public/fifo.h ../public/epolladd.h ../public/sockopt.h
	$(CC) -g $(SRC) -o $(EXEC) $(LIB) -lefence

