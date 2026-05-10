CC = c++
CFLAGS = -I. -std=c++17 -Wall -Wextra -Wwrite-strings -Wno-parentheses -Wpedantic -Wdeprecated -Wsign-conversion -Wimplicit-fallthrough -Wshadow -Warray-bounds -Wmissing-braces -Wconversion -Wuseless-cast
DEPS = 
OBJ = cbmchr2pixmap.o
LIBS = 

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cbmchr2pixmap: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

