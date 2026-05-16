CC = c++
CFLAGS = -I. -std=c++17 -Wall -Wextra -Wwrite-strings -Wno-parentheses -Wpedantic -Wdeprecated -Wsign-conversion -Wimplicit-fallthrough -Wshadow -Warray-bounds -Wmissing-braces -Wconversion -Wuseless-cast
DEPS = 
OBJ = cbmspr2pixmap.o
LIBS = 

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cbmspr2pixmap: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

