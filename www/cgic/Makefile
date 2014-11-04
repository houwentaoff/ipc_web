SUBDIRS=sample src

include lib.rules

LIBS=-L./lib -lcgic

all:lib/libcgic.a 
		for d in $(SUBDIRS); do [ -d $$d ] && $(MAKE) -C $$d; done

#install: libcgic.a
#	cp libcgic.a /usr/local/lib
#	cp cgic.h /usr/local/include
#	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

install:
	make -C src install
lib/libcgic.a: src/cgic.o
	rm -f lib/libcgic.a
	$(AR) rc src/libcgic.a src/cgic.o
	$(RANLIB) src/libcgic.a
	@mv src/libcgic.a lib/

#cgictest.cgi: cgictest.o libcgic.a
#	$(CC) cgictest.o -o cgictest.cgi ${LIBS}

#capture: capture.o libcgic.a
#	$(CC) capture.o -o capture ${LIBS}

#sample/login.cgi:libcgic.a sample/login.o
#	$(MAKE) -C sample 

clean:
	for d in $(SUBDIRS); do [ -d $$d ] && $(MAKE) -C $$d clean; done
	rm -f *.o *.a cgictest.cgi capture


