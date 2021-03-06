default : all;

run : all
	qemu-system-x86_64 -curses --serial mon:stdio -hdc kernel/kernel.img -hdd fat439/user.img

runC : all
	qemu-system-x86_64 --serial mon:stdio -hdc kernel/kernel.img -hdd fat439/user.img

runX: all
	qemu-system-x86_64 -hdc kernel/kernel.img -hdd fat439/user.img

report: REPORT.md
	pandoc REPORT.md --self-contained -t html5 > report.html
	xdg-open report.html

test : all
	make -C kernel kernel.img
	@echo "\nit will take 20-30 seconds for the following command to run\n"
	/usr/bin/time -p expect e0.tcl | tee t1.out
	@cat t1.out | grep -v sgabios > test.out
	@((diff -w -a test.ok test.out > test.diff 2>&1) && echo "\n======\npass\n======") || (echo "*** failed ***"; echo "\n========\nexpected\n========\n\n"; cat test.ok ; echo "\n\n========\nfound\n========\n\n" ; cat test.out ; echo "\n\n================\nfailed"; echo "expected results in test.ok" ; echo "your results in test.out"; echo "diff in test.diff"; echo "================")

	
% :
	(make -C kernel $@)
	(make -C user $@)
	(make -C fat439 $@)
