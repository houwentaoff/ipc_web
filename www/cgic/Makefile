SUBDIRS=sample src

include lib.rules

#AR=ar
#RANLIB=ranlib

LIBS=-L./ -lcgic

all: libcgic.a cgictest.cgi capture ./sample/login.cgi
		for d in $(SUBDIRS); do [ -d $$d ] && $(MAKE) -C $$d; done

#install: libcgic.a
#	cp libcgic.a /usr/local/lib
#	cp cgic.h /usr/local/include
#	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

install: cgictest.cgi
	cp cgictest.cgi $(DESTDIR)
libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

cgictest.cgi: cgictest.o libcgic.a
	$(CC) cgictest.o -o cgictest.cgi ${LIBS}

capture: capture.o libcgic.a
	$(CC) capture.o -o capture ${LIBS}

sample/login.cgi:libcgic.a sample/login.o
	$(MAKE) -C sample 

clean:
	for d in $(SUBDIRS); do [ -d $$d ] && $(MAKE) -C $$d clean; done
	rm -f *.o *.a cgictest.cgi capture


