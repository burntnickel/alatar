CXX = g++

CXXFLAGS = -std=c++20 -g
LDFLAGS =
LDLIBS =

TARGET = main_font_writer

SRCS = main_font_writer.cc

CXXFLAGS += `pkg-config --cflags          freetype2`
LDFLAGS  += `pkg-config --libs-only-L     freetype2`
LDLIBS   += `pkg-config --libs-only-other freetype2`
LDLIBS   += `pkg-config --libs-only-l     freetype2`

OBJS = $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) *.png


