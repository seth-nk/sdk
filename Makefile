all : libseth desktop

install : libseth-install desktop-install

libseth :
	$(MAKE) -C libseth

desktop : libseth libseth-install-sudo
	$(MAKE) -C desktop

libseth-install-sudo:
	sudo $(MAKE) -C libseth install

libseth-install :
	$(MAKE) -C libseth install

desktop-install :
	$(MAKE) -C desktop install

clean :
	$(MAKE) -C libseth clean
	$(MAKE) -C desktop clean
	$(MAKE) -C desktop OS=Windows_NT clean

uninstall :
	$(MAKE) -C libseth uninstall
	$(MAKE) -C desktop uninstall

rebuild : clean all

.PHONY: all clean rebuild install uninstall libseth desktop libseth-install-sudo libseth-install desktop-install
