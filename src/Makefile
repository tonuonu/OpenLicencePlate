OBJECTS =contour
CFLAGS= -O3 -march=native -g  -Wall -I/usr/local/include/opencv2
CPPFLAGS= -O3 -march=native -g  -Wall -I/usr/local/include/opencv2
LDFLAGS=-L/usr/local/lib -lopencv_imgproc -lopencv_imgproc  -lopencv_highgui

all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)
love:
	@echo "Not today honey, I have an headache"

contours: contours.cpp
	$(CXX) $^ -o $@ `pkg-config --cflags --libs opencv` $(CFLAGS) 
