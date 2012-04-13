SHELL = /bin/bash

# UEberschreiben der impliziten Regel 
# wie .o Dateien generiert werden, 
# damit die .o Dateien im objs Verzeichnis landen

%.o : %.cpp  
	$(CXX) $(CXXFLAGS)  -c $< -o ../objs/$@

# Verzeichnisse die Quelltexte enthalten
# und Verzeichnisse, die Dateien enthalten,
# die für die Auflösung von Abhängigkeiten
# eine Rolle spielen, also bin und 
# executables
# 
VPATH=				\
	../bin			\
	../objs			\
	./				\
	FirstSteps		\
	CAN_Objects		\
	CAN_EDS_Profiles	\
	../../../sfz-tools/Threads/src/Thread		\
	

	
CXX = g++


# Die im Projekt vorkommende Objektdateien
# werden durch make depend ermittelt und 
# in eine Datei geschrieben, die wird hier eingebunden

-include objects_and_includes



# Compilerflags
CXXFLAGS = -O2 -g -Wall -fmessage-length=0 	\
	-lboost_thread			\
	`pkg-config --cflags --libs gtkmm-2.4`	\
	 $(INCLUDEDIRS) 



		
#Targets
# Ausführbare Dateien enden auf .exe, damit sie 
# mit make clean leicht geloescht werden koennen

all: 					\
		Recive_Can_Frame.exe		\
		SendCanFrame.exe			\
		tst-bcm-rx-sendto			\
		Can_Node_Main			\
		EPOS_Tests_Main			\
		EDS_File_Parser			\



# $< der Name der Ersten Abhängigkeit  
# $@ der Name des Targets 

	
	
Recive_Can_Frame.exe: Recive_Can_Frame.cpp $(OBJS)
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@

SendCanFrame.exe: SendCanFrame.cpp $(OBJS)
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@

tst-bcm-rx-sendto: tst-bcm-rx-sendto.cpp $(OBJS)
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@

Can_Node_Main: Can_Node_Main.cpp $(OBJS)
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@

EPOS_Tests_Main: EPOS_Tests_Main.cpp $(OBJS)
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@

EDS_File_Parser: EDS_File_Parser.cpp $(OBJS)
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@



.PHONY: clean

clean: 
	-rm ../bin/*
	-rm ../objs/*.o
	
.PHONY: cleanall
	
cleanall: clean
	-rm -r ../doc/html
	-rm -r ../doc/latex
	
	

# 1)automatische Erstellung von Abhängigkeiten
# die in der Datei dependencies gespeichert werden
# 
# 2) automatische Erstellung der include Dirs
#
# 3) automatische Erstellung der OBJS
#
# 4)OBJS_AND_PATH enthält die .o Dateien mit dem Präfix ../objs
#
# Strategie: Für .cpp Dateien für die es auch eine .h Datei gibt wird ein 
# .o Eintrag bzw. ein ../objs/*.o Eintrag generiert
#

.PHONY: depend

depend: 
	if [ ! -d ../objs ]; then mkdir ../objs ; fi ;   
	if [ ! -d ../bin ]; then mkdir ../bin ; fi ;   
	if [ ! -d ../doc ]; then mkdir ../doc ; fi ;   
	
	-rm objects_and_includes
	echo 'INCLUDEDIRS =  \' >>  objects_and_includes; \
	for i in $(VPATH) ; \
	do \
		echo "-I $$i   \\" >> objects_and_includes; \
	done; 

	echo "" >> objects_and_includes;  \
	echo 'OBJS =  \'  >> objects_and_includes; \
	for i in $(VPATH) ; \
	do \
		for j in $$(ls -A $$i | grep .cpp); \
		do	\
			  tmp=$$i/$${j/.cpp}.h;		\
			  if [ -f $$tmp ] ;				\
			  then						\
			  	echo  "$${j/.cpp}.o  \\" >> objects_and_includes; \
			  fi;	\
		done; \
	done;

	echo "" >> objects_and_includes;  \
	echo 'OBJS_AND_PATH =  \'  >> objects_and_includes; \
	for i in $(VPATH) ; \
	do \
		for j in $$(ls -A $$i | grep .cpp); \
		do	\
			  tmp=$$i/$${j/.cpp}.h;		\
			  if [ -f $$tmp ] ;				\
			  then						\
			  	echo  "../objs/$${j/.cpp}.o  \\" >> objects_and_includes; \
			  fi;	\
		done; \
	done;

	-rm dependencies
	for i in $(VPATH) ; \
	do \
		for j in $$(ls -A $$i | grep .cpp); \
		do	\
			  tmp=$$i/$${j/.cpp}.h;		\
			  if [ -f $$tmp ] ;				\
			  then						\
				$(CXX) $(INCLUDEDIRS) -MM  $$i/$$j >> dependencies; \
			  fi;	\
		done; \
	done; 


	cat dependencies
	cat objects_and_includes
	
	

.PHONY: doc
doc:
	doxygen  Doxyfile

# die Abhängigkeiten im File dependencies einbinden

-include   dependencies