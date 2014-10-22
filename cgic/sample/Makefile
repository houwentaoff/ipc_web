include ../lib.rules

login.cgi:login.o
	$(CC) $? -o $@ $(CFLAGS)

.PHONY:clean
clean:
	-@$(ECHO) -e "\033[5;41;32m  Cleaning sample ...   \033[0m" 
	-$(RM) *.o login.cgi *.cgi
install:
	cp login.cgi $(DESTDIR)

