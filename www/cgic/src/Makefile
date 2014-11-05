include ../lib.rules

default:all

all:demo.cgi setroute.cgi

demo.cgi: base.o utils.o page.o demo.o
	$(CC) $^ -o $@ $(CFLAGS)

setroute.cgi: base.o utils.o base_page.o page.o setroute.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY:clean
clean:
	-@$(ECHO) -e "\033[5;41;32m  Cleaning src ...   \033[0m" 
	-$(RM) *.o demo.cgi *.cgi route.cgi
install:
	@mkdir -p $(DESTDIR)
	cp -a *.cgi $(DESTDIR)
