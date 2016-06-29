CC := g++

SRCDIR := src
TARGET := bin/ubidots-onion


CFLAGS := -fPIC -Wno-write-strings


default:
	mkdir -p bin
	$(CC) $(CFLAGS) src/ubidots.cpp -o $(TARGET)
	@echo Build completed.

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

