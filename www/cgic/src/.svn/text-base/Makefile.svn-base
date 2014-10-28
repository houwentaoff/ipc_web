include ../lib.rules

demo.cgi: base.o page.o demo.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY:clean
clean:
	-@$(ECHO) -e "\033[5;41;32m  Cleaning src ...   \033[0m" 
	-$(RM) *.o demo.cgi *.cgi
install:
	cp demo.cgi $(DESTDIR)
