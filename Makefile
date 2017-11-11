
SRCS   	= $(wildcard src/*.c)
OUT     = build

CFLAGS 	= -Wall -lX11 -std=c99

### Compile, link and create load file
$(OUT)/lights : $(SRCS)
	cc $(CFLAGS) $(SRCS) -o $(OUT)/lights


### Cleanup
.PHONY  : clean
clean   :
	-rm $(OUT)/lights

