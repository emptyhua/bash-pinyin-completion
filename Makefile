LINUX_BASHCD=$(DESTDIR)/etc/bash_completion.d
LINUX_BIN=$(DESTDIR)/usr/bin
XCODE_SDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
PLATFORM=$(shell uname)

ifeq ($(PLATFORM),Darwin)
	CFLAGS=-isysroot $(XCODE_SDK)
else
	CFLAGS=
endif
all:pinyinmatch

build:pinyinmatch

pinyinmatch:pinyinmatch.o pinyin.o utf8vector.o linereader.o
	gcc -Wall $(CFLAGS) -std=c99 pinyinmatch.o pinyin.o utf8vector.o linereader.o -o pinyinmatch

%.o:%.c
	gcc -Wall $(CFLAGS) -std=c99 -c $< -o ./$@

install:	
	if [ "`uname`" = "Darwin" ];then \
		echo macos; \
		test ! -e "/usr/local/etc/bash_completion.d" && echo 'install bash-completion first!' && exit 1; \
		cp pinyinmatch /usr/local/bin ;\
		cp pinyin_completion //usr/local/etc/bash_completion.d/ ;\
	elif [ "`uname`" = "Linux" ];then \
		echo linux; \
		install -d $(LINUX_BASHCD) ;\
		install -d $(LINUX_BIN) ;\
		install ./pinyin_completion $(LINUX_BASHCD) ;\
		install ./pinyinmatch $(LINUX_BIN) ;\
	fi

uninstall:	
	if [ "`uname`" = "Darwin" ];then \
		echo macos; \
		rm /usr/local/bin/pinyinmatch  ;\
		rm /opt/local/etc/bash_completion.d/pinyin_completion ;\
	elif [ "`uname`" = "Linux" ];then \
		echo linux; \
		rm $(LINUX_BASHCD)/pinyin_completion  ;\
		rm $(LINUX_BIN)/pinyinmatch  ;\
	fi


clean:
	-rm *.o
	-rm pinyinmatch
