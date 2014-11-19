include ../lib.rules

default:all

all:demo.cgi setroute.cgi flysetroute.cgi

demo.cgi: base.o utils.o base_page.o page.o demo.o
	$(CC) $^ -o $@ $(CFLAGS)

setroute.cgi: base.o utils.o base_page.o page.o setroute.o
	$(CC) $^ -o $@ $(CFLAGS)

flysetroute.cgi: base.o utils.o base_page.o page.o flysetroute.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY:clean
clean:
	-@$(ECHO) -e "\033[5;41;32m  Cleaning src ...   \033[0m" 
	-$(RM) *.o demo.cgi *.cgi route.cgi
install:
	@mkdir -p $(DESTDIR)
	@echo "### --> [$(ROOTFS_TOP)/webSvr/www/cgi-bin/demo.cgi]"
	@cp -a *.cgi $(DESTDIR)
