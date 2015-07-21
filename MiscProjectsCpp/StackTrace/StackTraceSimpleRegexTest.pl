#!/usr/bin/perl

	@test_list = (
		"./test_StackTraceSimple() [0x401399]",
		"./test_StackTraceSimple() [0x401952]",
		"./test_StackTraceSimple() [0x401947]",
		"./test_StackTraceSimple() [0x40157c]",
		"./test_StackTraceSimple() [0x401591]",
		"./test_StackTraceSimple() [0x40159c]",
		"./test_StackTraceSimple() [0x40198a]",
		"./test_StackTraceSimple() [0x40197f]",
		"./test_StackTraceSimple() [0x40196c]",
		"./test_StackTraceSimple() [0x4015b7]",
		"./test_StackTraceSimple() [0x40165c]",
		"/lib64/libc.so.6(__libc_start_main+0xf5) [0x7f6a54ceed65]",
		"./test_StackTraceSimple() [0x4012a9]"
	);

	foreach $this_item (@test_list) {
		print $this_item, "\n";
		$tmp_item =  $this_item;
		if ($this_item =~
			/^(.+)\(((.+)\+0x([\dA-Fa-f]+))*\)\s+\[0x([\dA-Fa-f]+)\]$/) {
			print " ---> [$1][$3][$4][$5]\n";
		}
		else {
			print " ---> NULL\n";
		}
	}

