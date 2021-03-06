
ifeq ($(shell [ ! -r ../Make.Rules ] && echo 1),)
    include ../Make.Rules
endif

INCLUDEDIR := -I../include
CIN      := $(wildcard *.c)

OBJ      := $(CIN:%.c=%.o)
OUT_SLIB := $(addsuffix .a,$(PACKAGE_NAME))
OUT_DLIB := $(addsuffix .so,$(PACKAGE_NAME))

LDFLAGS  := -Wl,--warn-common


LIBNAME=$(PACKAGE_NAME)
VERSION=.$(MAJOR_REL)
MODIFICATION=.$(MINOR_REL)

CFLAGS += -fPIC

# dynamic library names
LIBHASH = $(LIBNAME).so
LIBHASH = $(addsuffix .so,$(LIBNAME))
LIBHASHNAME = $(LIBHASH)$(VERSION)
LIBHASHFULL = $(LIBHASHNAME)$(MODIFICATION)

# static library name
LIBHASHSTATIC = $(LIBNAME).a

# pkgconfig name
LIBHASHPC = $(LIBNAME).pc

%.o: %.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) $< -c -o $@

all: $(LIBHASH) $(LIBHASHSTATIC) $(CIN)

$(LIBHASH): $(OBJ) ../Make.Rules
	@echo "### build dynamic library (libhashish.so)"
	$(CC) -shared -fPIC $(LDFLAGS) -Wl,-soname=$(LIBHASHNAME) -lm -lpthread -o $@ $(OBJ)

$(LIBHASHSTATIC): $(OBJ) ../Make.Rules
	@echo "### build static archiv (libhashish.a)"
	$(AR) cr $@ $(OBJ) 
	$(RANLIB) $@
# This is a workaround that the test suite can be linked
# against the static archive instead of the dynamic library.
# The linker will normaly link against the dynamic library,
# in the development phase it is normaly not the case that
# this library is installed. Through this workaround we can link
# against the static archive without forcing the whole project
# is linked against all invoked static libraries (through -static)
	ln -sf $@ $(LIBNAME)_s.a

clean:
	@echo "### clean-up library directory"; \
	$(RM) -f $(OBJ) $(OUT_SLIB) $(OUT_DLIB) $(LIBNAME)_s.a
 
distclean:
	@echo "### distclean library directory"
	@true
 
install:
	$(MKDIR) -p $(DESTDIR)$(libdir)/
	$(MKDIR) -p $(DESTDIR)$(libdir)/pkgconfig
	$(INSTALL) -m 0644 $(OUT_SLIB) $(DESTDIR)$(libdir)
	$(INSTALL) -m 0644 $(OUT_DLIB) $(DESTDIR)$(libdir)/$(OUT_DLIB).$(MAJOR_REL)
	$(INSTALL) -m 0644 $(LIBHASHPC) $(DESTDIR)$(libdir)/pkgconfig
	$(LDCONFIG)
	( cd $(LIBDIR) ; ln -sf $(LIBHASHNAME) $(DESTDIR)$(libdir)/$(LIBHASH) )

 export
 
.PHONY: all clean install

