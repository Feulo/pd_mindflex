CLIBS = -lbluetooth
CFLAGS = -Wall -O2 -std=c99 
cc= gcc


.PHONY all:
all: mindflex mind2sig mind_sin
mindflex: mindflex.o ThinkGearStreamParser.o
	$(cc) -shared -lc -lm -lbluetooth -pthread -o $@.pd_linux $^ $(CFLAGS)
mindflex.o: mindflex.c m_pd.h
	$(cc) -DPD -fPIC $(CFLAGS) -lbluetooth -c mindflex.c
ThinkGearStreamParser.o: ThinkGearStreamParser.c ThinkGearStreamParser.h
	$(cc) -DPD -fPIC $(CFLAGS) -c ThinkGearStreamParser.c

#mindflex_filter: mindflex_filter.o 
#	$(cc) -shared -o $@.pd_linux $^ $(CFLAGS)
#mindflex_filter.o: mindflex_filter.c m_pd.h
#	$(cc) -DPD -fPIC $(CFLAGS) -c mindflex_filter.c

mind2sig: mind2sig.o 
	$(cc) -shared -o $@.pd_linux $^ $(CFLAGS)
mind2sig.o: mind2sig.c m_pd.h
	$(cc) -DPD -fPIC $(CFLAGS) -c mind2sig.c

mind_sin: mind_sin.o 
	$(cc) -shared -pthread -o $@.pd_linux $^ $(CFLAGS)
mind_sin.o: mind_sin.c m_pd.h
	$(cc) -DPD -fPIC $(CFLAGS) -c mind_sin.c
.PHONY clean:
clean:
	$(RM) *.pd_linux *.o


