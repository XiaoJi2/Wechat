PROJDIR = .
OBJSDIR = objs
INCSDIR = $(PROJDIR)/include
SRCDIR  = src

SRCS += $(wildcard $(SRCDIR)/*.cpp)
OBJS += $(patsubst %.cpp,$(OBJSDIR)/%.o,$(SRCS))

CPP     = g++
CFLAGS += -g -Wall
CFLAGS += -I$(INCSDIR) -I/usr/local/include/htmlcxx/html -I/usr/include/curl
LIBS   += -lrt -lhtmlcxx -lcurl -lm -lpthread -lsqlite3


EXENAME = WEIXIN

all: $(EXENAME) $(DEPFILE)

$(EXENAME): $(OBJS)
	@echo '=====Make File====='
	$(CPP) $(CFLAGS) $(OBJS) -o $@ $(LIBS) $(CFLAGS)
	rm -rf log
	rm -rf wx.cookie

$(OBJSDIR)/%.o: %.cpp
	$(CPP) -c -o $@ $< $(CFLAGS) $(LIBS)

build:
	mkdir -p $(PROJDIR)/$(OBJSDIR)/$(SRCDIR)

.PHONY:clean
clean:
	rm $(EXENAME)
	rm $(PROJDIR)/$(OBJSDIR)/$(SRCDIR)/*.o 
	rm QRcode.jpg
	rm -rf log
	rm -rf wx.cookie

