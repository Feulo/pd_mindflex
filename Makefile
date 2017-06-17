CFLAGS = -DPD -fPIC -Wall -O2 -std=c99 
cc= gcc

.PHONY all:
all: mindflex mind2sig mind_sin mind_fft mind_rec mind_play mind_filter mind_list mind_max mind_blink mind_attention mind_rec_att mind_energy mind_sequence
mindflex: mindflex.o ThinkGearStreamParser.o
	$(cc) -shared -o $@.pd_linux $^ -lbluetooth -lpthread
mindflex.o: mindflex.c m_pd.h
	$(cc) $(CFLAGS) -c mindflex.c
ThinkGearStreamParser.o: ThinkGearStreamParser.c ThinkGearStreamParser.h
	$(cc) $(CFLAGS) -c ThinkGearStreamParser.c

mind2sig: mind2sig.o 
	$(cc) -shared -o $@.pd_linux $^ -lsamplerate
mind2sig.o: mind2sig.c m_pd.h
	$(cc) $(CFLAGS) -c mind2sig.c

mind_sin: mind_sin.o 
	$(cc) -shared -o $@.pd_linux $^ -lpthread 
mind_sin.o: mind_sin.c m_pd.h
	$(cc) $(CFLAGS) -c mind_sin.c

mind_fft: mind_fft.o 
	$(cc) -shared -o $@.pd_linux $^ -lpthread
mind_fft.o: mind_fft.c m_pd.h
	$(cc) $(CFLAGS) -c mind_fft.c

mind_rec: mind_rec.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_rec.o: mind_rec.c m_pd.h
	$(cc) $(CFLAGS) -c mind_rec.c

mind_list: mind_list.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_list.o: mind_list.c m_pd.h
	$(cc) $(CFLAGS) -c mind_list.c

mind_play: mind_play.o 
	$(cc) -shared -o $@.pd_linux $^ -lpthread
mind_play.o: mind_play.c m_pd.h
	$(cc) $(CFLAGS) -c mind_play.c

mind_filter: mind_filter.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_filter.o: mind_filter.c m_pd.h filter.h
	$(cc) $(CFLAGS) -c mind_filter.c
mind_max: mind_max.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_max.o: mind_max.c m_pd.h 
	$(cc) $(CFLAGS) -c mind_max.c
mind_blink: mind_blink.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_blink.o: mind_blink.c m_pd.h 
	$(cc) $(CFLAGS) -c mind_blink.c
mind_attention: mind_attention.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_attention.o: mind_attention.c m_pd.h 
	$(cc) $(CFLAGS) -c mind_attention.c
mind_rec_att: mind_rec_att.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_rec_att.o: mind_rec_att.c m_pd.h 
	$(cc) $(CFLAGS) -c mind_rec_att.c
mind_energy: mind_energy.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_energy.o: mind_energy.c m_pd.h 
	$(cc) $(CFLAGS) -c mind_energy.c
make_folder: make_folder.o 
	$(cc) -shared -o $@.pd_linux $^ 
make_folder.o: make_folder.c m_pd.h 
	$(cc) $(CFLAGS) -c make_folder.c
mind_sequence: mind_sequence.o 
	$(cc) -shared -o $@.pd_linux $^ 
mind_sequence.o: mind_sequence.c m_pd.h 
	$(cc) $(CFLAGS) -c mind_sequence.c
.PHONY clean:
clean:
	$(RM) *.pd_linux *.o


