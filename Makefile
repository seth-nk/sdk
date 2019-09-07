all :
	$(MAKE) -C libseth
	$(MAKE) -C desktop
clean :
	$(MAKE) -C libseth clean
	$(MAKE) -C desktop clean

install : all
	$(MAKE) -C libseth install
	$(MAKE) -C desktop install

rebuild : clean all
