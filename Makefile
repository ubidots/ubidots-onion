CC := g++

SRCDIR := src
TARGET := bin/ubidots


CFLAGS := -fPIC -Wno-write-strings


default:
	mkdir -p bin
	$(CC) $(CFLAGS) src/ubidots.c -o $(TARGET)
	@echo Build completed.

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

