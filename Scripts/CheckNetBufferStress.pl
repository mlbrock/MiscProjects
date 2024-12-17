#!/usr/bin/perl


	my @CNBS_LastData    = ();
	my @CNBS_LastString  = "";
	my $CNBS_LogFileName = "LOG_CheckNetBufferStress." .
		GetLogFileTimeString() . ".log";
	my $CNBS_SignalFlag  = 0;

	if (!open(LOG_FILE, ">$CNBS_LogFileName")) {
		print STDERR "\nERROR: Unable to open log file '",
			$CNBS_LogFileName, "' for writing.\n";
		exit(-1);
	}

	my $CNBS_LogFileHandle = "LOG_FILE";

	$SIG{"INT"}  = \&HandleSignal;
	$SIG{"TERM"} = \&HandleSignal;

	while (!$CNBS_SignalFlag) {
		my @tmp_last_data = `netstat -su | grep 'buffer errors'`;
		my $tmp_last_string = join("", @tmp_last_data);
		if ($tmp_last_string ne $CNBS_LastString) {
			my $tmp_time = GetGmTime() . " ";
			my $tmp_log  = $tmp_time . join($tmp_time, @tmp_last_data) . "\n";
			print LOG_FILE $tmp_log;
			print          $tmp_log;
			$CNBS_LastString = $tmp_last_string;
		}
		sleep(5);
	}

	exit(0);

# ###########################################################################
sub HandleSignal {
	print STDERR "\nSignal received.\n";

	$CNBS_SignalFlag = 1;
}
# ###########################################################################

# ###########################################################################
sub GetGmTime() {
	my(@time_data, $time_string);

	@time_data   = localtime(time());
	$time_string = sprintf("%4u-%02u-%02u %02u:%02u:%02u",
		$time_data[5] + 1900, $time_data[4] + 1, $time_data[3],
		$time_data[2], $time_data[1], $time_data[0]);

	return($time_string);
}
# ###########################################################################

# ###########################################################################
sub GetLogFileTimeString {
	my($src_time, $dst_time);

	$src_time = GetGmTime();

	$dst_time =
		substr($src_time,  0, 4) .
		substr($src_time,  5, 2) .
		substr($src_time,  8, 2) . "_" .
		substr($src_time, 11, 2) .
		substr($src_time, 14, 2) .
		substr($src_time, 17, 2);

	return($dst_time);
}
# ###########################################################################

