SHELL = /bin/bash


# Verzeichnisse die Quelltexte enthalten
# und Verzeichnisse, die Dateien enthalten,
# die für die Auflösung von Abhängigkeiten
# eine Rolle spielen, also bin und 
# executables
# 
VPATH=				\
	../bin			\
	../objs			\
	../libs			\
	./				\
		


# For Player v.3.0.1 on x86_64 Plattforms 
# you have to export PKG_CONFIG_PATH	
export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig/
	
CXX = g++


# Die im Projekt vorkommende Objektdateien
# werden durch make depend ermittelt und 
# in eine Datei geschrieben, die wird hier eingebunden

-include objects_and_includes



# Compilerflags
#CXXFLAGS = -O2 -g -Wall -fmessage-length=0 \

CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -fPIC\
	 -lusb-1.0										\
	 `pkg-config --libs playerc++` -lrt				\
     `pkg-config --cflags playerc++`				\
	 $(INCLUDEDIRS) 



# UEberschreiben der impliziten Regel 
# wie .o Dateien generiert werden, 
# damit die .o Dateien im objs Verzeichnis landen

%.o : %.cpp  
	$(CXX) $(CXXFLAGS)  -c $< -o ../objs/$@


		
#Targets
# Ausführbare Dateien enden auf .exe, damit sie 
# mit make clean leicht geloescht werden koennen

all: 					\
	libusb_test			\
	libsicktim3xx.so	\
	TiM_client_test

# $< der Name der Ersten Abhängigkeit  
# $@ der Name des Targets 


libusb_test: libusb_test.cpp $(OBJS) 
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@
	
TiM_client_test: TiM_client_test.cpp $(OBJS) 
	$(CXX)  $(CXXFLAGS) $< $(OBJS_AND_PATH) -o ../bin/$@



libsicktim3xx.so: $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS_AND_PATH) ../objs/SickTiM3xx.o  -shared -o ../libs/$@

	



.PHONY: clean

clean: 
	-rm ../bin/*
	-rm ../objs/*.o
	-rm ../libs/*.so
	
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
	if [ ! -d ../libs ]; then mkdir ../libs ; fi ;   
	
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
			  tmp=$$i/$${j/.cpp}.hpp;		\
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
			  tmp=$$i/$${j/.cpp}.hpp;		\
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