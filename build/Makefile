 
CC      = g++
LIBPATH = /home/n00b/Code/Develop/Indoorino3/libraries/indoorino/
CFLAGS  = -Wall -DRAWRUN -lm -save-temps -I$(LIBPATH)

SOURCE  = rawrun.cpp $(LIBPATH)utils.cpp $(LIBPATH)packeTemplate.cpp $(LIBPATH)packetParse.cpp $(LIBPATH)packetEval.cpp

OBJ     = rawrun.cpp


# %.o: %.cpp $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

rawrun: $(OBJ)
	
	$(CC) -o $@ $(CFLAGS) $(SOURCE)

