CLIBS = -lbluetooth
CFLAGS = -Wall -O2 -std=c99 
cc= gcc



mindflex: mindflex.o ThinkGearStreamParser.o
	$(cc) -shared -lc -lm -lbluetooth -pthread -o $@.pd_linux $^ $(CFLAGS)
mindflex.o: mindflex.c m_pd.h
	$(cc) -DPD -fPIC $(CFLAGS) -lbluetooth -c mindflex.c
ThinkGearStreamParser.o: ThinkGearStreamParser.c ThinkGearStreamParser.h
	$(cc) -DPD -fPIC $(CFLAGS) -c ThinkGearStreamParser.c
.PHONY clean:
clean:
	$(RM) mindflex.pd_linux *.o


