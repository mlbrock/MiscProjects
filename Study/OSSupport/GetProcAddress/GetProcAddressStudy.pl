#!/bin/perl

	$return_code = 0;
	$file_name   = "GetProcAddressStudy.ntdll.dll.20160409_A.txt";

	if (!open(IN_FH, "<$file_name")) {
		print STDERR "ERROR: Unable to open '", $file_name, "' for reading.\n";
		$return_code = -1;
	}
	else {
		@in_lines = <IN_FH>;
		close(IN_FH);
		chop @in_lines;
		for ($count_1 = 0; $count_1 <= $#in_lines; ++$count_1) {
			if ($in_lines[$count_1] =~ /^\s+ordinal\s+hint\s+RVA\s+name\s*$/) {
				++$count_1;
				while (($count_1 <= $#in_lines) &&
					($in_lines[$count_1] =~ /^\s*$/)) {
					++$count_1;
				}
				@export_lines = ();
				$export_hash  = ();
				while (($count_1 <= $#in_lines) &&
					($in_lines[$count_1] !~ /^\s*$/)) {
# ##### [       1994  7C1 000807D4 wcstoul
# ##### [0123456789012345678901234567890123456789
					$key                = substr($in_lines[$count_1], 17, 8);
					$export_hash{$key} .=
						((defined($export_hash{$key})) ? "\n" : "") .
						$in_lines[$count_1];
					++$count_1;
				}
			}
		}
		foreach $this_key (keys(%export_hash)) {
			$this_value = $export_hash{$this_key};
			if (index($this_value, "\n") > -1) {
				print "DUPLICATE ADDR ", $this_key, ":\n";
				print "   ", join("\n   ", split(/\n/, $this_value)), "\n";
			}
		}
	}

	exit($return_code);
