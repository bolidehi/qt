Auther: Wang Bin (aka nukin in ccomve & novesky in motorolafans)
Shanghai university, China
2010-09-09
wbsecg1@gmail.com
Other links:	http://sourceforge.net/projects/qop/files
				http://qt-apps.org/content/show.php/qop?content=132430

Qt Output Parser for tar, zip, unzip, unrar, 7z with a compression/extraction progress indicator.
Support platforms: windows(Qt4), Linux(Qt4, tested on ubuntu 10.04), motorola ezx(Qt2, tested on ROKR E6) and Maemo5 etc.

bug: can't kill 7z in windows.

Usage: qop [-interval=Nunit] [--all] [-t parserFor] [-n|s] [-chm] [-x archieve|-T totalSteps] [files...]

-a, --all: update all changes. default is update on timer event
-F, --time-format=fmt: setup time format. utc(iso8601) or normal
-m, --multi-thread: create a new thread to calculate progress bar's total steps.
-i, --interval=Nunit: update the progress every N seconds/mseconds. unit can be s, sec[s],seconds(N can be float) or msec[s](N is int)
-h, --help: help
-n, --number: set number of files as total steps.
-s, --size: set size of files as total steps. -s is default
-T, --steps=STEPS: specify total steps.
-t,  --parser[=TYPE]: usually is tool's name, such as tar, zip, unzip, unrar. If using xz, lzop etc with tar, parser is tar. -t tar is default.
If you want to extract a .tar or .tar.xxx file and set size as total steps, use -tuntar is better.
-x, --exteact=ARCHIVE: extracting mode. Omit -T argument. Analyze parser and total steps automaticly.
-o, --outdir=dir?? set the output dir when using internal extract method(qop -d -x test.tar -o outdir)
-c, --auto-close: auto close when finished
-C, --cmd=command: execute command
-d, --diy[=TARFILE]: using built-in method to extract an archive

examples:
	qop -C zip -ryv -9 test.zip test
	qop -C zip -ry -9 test.zip test
	qop -C unzip -o test.zip -d exdir
	qop -C unrar x -o+ -p- -y rar.rar destdir
	qop -C tar cvvf test.tar test
	qop -C tar zcvf test.tgz test
	qop -C tar zxvvf test.tgz -C /tmp   ##only 1 v will not show the right totalsteps and progress
	qop tar cvvf test.tar test
	tar zcvvf test.tgz test |qop test -m
	tar zxvvf test.tgz |qop -T `gzip -l test.tgz |sed -n '$s/\(.*\) \(.*\)  .*/\2/p'` -tuntar -c
	tar zxvf test.tgz |qop -T `tar -zt <test.tgz |wc -l` -n  ##slower than former

	tar --use=xz -cvvf test.txz test |qop test -m -t tar
	tar --use=xz test.tar.xz |qop -T `tar --use=xz -t<test.tar.xz |wc -l` -n

	zip -ryv -9 -FS test.zip test |qop test -m -t zip
	unzip -o test.zip -d . |qop -T `unzip -Z -t test.zip |sed 's/\(.*\) files.*/\1/'` -t unzip &
	unrar x -o+ -y test.rar . |qop -t unrar
	7z x -y test.7z |qop -t7z -T $((`7z l test.7z |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l test.7z |sed -n '$s/\(.*\) \(.*\)files.*/\2/p'`))

for Version >= 0.2.2, you can omit -n and -s
	tar zcvvf test.tgz test |qop test [-m]
	tar zcvf test.tgz test |qop test  ##no -m

extracting a tar.gz file for new version(>=0.1.0):
	tar zxvvf test.tar.gz |qop -x test.tar.gz


How To Compile:
1.Desktop and Maemo: Just use QtCreator.
2.EZX: Use tmake. Or use my configure script. Type
./configure
make
make pkg
More information about configue script can be got by ./configure --help
