MAKE_CONFIG = make_config

# OPSYS should equal either "windows" or "linux".  If it does not, you will be to execute "make config".
# After which, this Makefile should change so that OPSYS equals "windows" or "linux".

# TODO -- Is there a better way of testing the operatin system?
OPSYS=linux

all:
	@ if [ -f $(MAKE_CONFIG) ]; then \
	make all -f Makefile.$(OPSYS); \
	else \
	echo "You must first execute 'make config'"; \
	fi 

desktop_parser:
	@ if [ -f $(MAKE_CONFIG) ]; then \
	make desktop_parser -f Makefile.$(OPSYS); \
	else \
	echo "You must first execute 'make config'"; \
	fi 

onenetlib:
	@ if [ -f $(MAKE_CONFIG) ]; then \
	make onenetlib -f Makefile.$(OPSYS); \
	else \
	echo "You must first execute 'make config'"; \
	fi 

config: make_config
	./make_config

make_config: make_configure.c
	gcc make_configure.c -o make_config

clean:
	rm -f make_config *.o *.a desktop_parser *~
