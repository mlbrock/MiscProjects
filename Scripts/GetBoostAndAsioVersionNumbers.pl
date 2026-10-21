#    !/bin/bash
#    !/bin/bash
# #############################################################################
# #############################################################################
# Miscellaneous Utility Script
# #############################################################################
#
# File Name       : GetBoostAndAsioVersionNumbers.pl
#
# File Description: Link Boost versions with corresponding ASIO versions.
#
# Revision History: 2026-10-21 --- Creation.
#                       Michael L. Brock
#
#       Copyright Michael L. Brock 2026 - 2026.
#
#       Distributed under the Boost Software License, Version 1.0.
#       (See accompanying file LICENSE_1_0.txt or copy at
#       http://www.boost.org/LICENSE_1_0.txt)
#
# #############################################################################

# #############################################################################

	$return_code        = 0;
	$error_text         = "";

	if (grep(/^\-H$|^\-HELP$/i, @ARGV)) {
		print "USAGE:\n";
		print "   $0 ",
			"[ <src-dir> [ <src-dir> ... ] ]\n\n",
			"   ",
			"<src-dir> If not specified, defaults to the current directory.\n";
		print "\n";
		exit($return_code);
	}

	@GBAAVN_DirList = @ARGV;

	if ($#GBAAVN_DirList == -1) {
		@GBAAVN_DirList = ($ENV{"PWD"});
	}

	%GBAAVN_BoostVersionHash = ();
	%GBAAVN_FileVersionHash  = ();

	foreach $this_dir (@GBAAVN_DirList) {
		if (!(-d $this_dir)) {
			print STDERR "ERROR: Expected '", $this_dir, "' to be a directory ",
				"... skipping.\n";
			next;
		}
		$tmp_dir         =  `readlink -f $this_dir`;
		ExitIfSystemError("Unable to resolve directory '" . $this_dir .
			"' to a canonical directory name.");
		chop($tmp_dir);
		$tmp_dir_length  =  length($tmp_dir);
		$tmp_dir         =~ s/\\/\//g;
		$tmp_dir         =~ s/\/\//\//g;
		$tmp_dir         =~ s/\/$//g;
		$tmp_dir        .=  "\/";
		$dir_pattern     =  $tmp_dir . "boost*.zip";
#print "[", this_dir, "] -> [", $tmp_dir, "] -> [", $dir_pattern, "]\n";
		@in_file_list    =  `find $tmp_dir -maxdepth 1 -type f|grep -P '\\/boost(_|-)\\d(_|\\.)\\d+((_|\\.)\\d)?\\.zip\$'`;
		ExitIfSystemError("Attempt to perform a find operation on directory '" .
			$this_dir . "' failed.");
#print @in_file_list, "\n"; exit(0);
#print "[", join("][", @in_file_list), "]\n";
		chop @in_file_list;
		$in_file_count_1 =  $#in_file_list + 1;
#		@in_file_list    =  sort(@in_file_list);
		foreach $this_file (@in_file_list) {
			$tmp_file = $this_file;
			$tmp_file =~ s/.+\/boost(_|-|\.)(\d+.+)\.zip$/$2/;
			@version_info =  split(/_|-|\./, $tmp_file);
			$version_name = join("_", @version_info);
			if ($#version_info == 1) {
				push(@version_info, "0")
			}
			$version_number = join("_", @version_info);
#print "[", $this_file, "] ---> [", $tmp_file, "][", $version_number, "]\n";
			if (!defined($GBAAVN_BoostVersionHash{$version_number})) {
				$GBAAVN_BoostVersionHash{$version_number} = $this_file;
				$GBAAVN_FileVersionHash{$version_number}  = $version_name;
			}
		}
	}

	for $this_version (sort(keys(%GBAAVN_BoostVersionHash))) {
		# ASIO was first introduced into Boost version 1.42.0.
		if ($this_version lt "1_42_0") {
			next;
		}
		$version_name  =  $GBAAVN_FileVersionHash{$this_version};
		$boost_version =  $this_version;
		$boost_version =~ s/_/\./g;
#		$this_cmd = "unzip -c " . $GBAAVN_BoostVersionHash{$this_version} .
#			" boost_" . $this_version . "/boost/asio/version.hpp|" .
#			"grep '#define BOOST_ASIO_VERSION ' > Boost_" . $this_version .
#			"_AsioVersion.txt";
		$this_path   = "boost_" . $version_name . "/boost/asio/version.hpp";
		$this_cmd    = "unzip -c " . $GBAAVN_BoostVersionHash{$this_version} .
			" " . $this_path . "|grep '#define BOOST_ASIO_VERSION '";
#print $this_cmd, "\n";
		@cmd_results = `$this_cmd`;
		ExitIfSystemError("Unable to unzip file '" . $this_path . "' from ZIP " .
			"file '" . $GBAAVN_BoostVersionHash{$this_version} . "'.");
		if ($#cmd_results != 0) {
			print STDERR "ERROR: Processing of ZIP file '",
				$GBAAVN_BoostVersionHash{$this_version}, "' failed: ",
				"expected 1 line of command results, received ",
				($#cmd_results + 1), ".\n";
			next;
		}
		chop(@cmd_results);
		@tmp_results = split(/\s+/, $cmd_results[0]);
		if ($#tmp_results != 4) {
			print STDERR "ERROR: Processing of ZIP file '",
				$GBAAVN_BoostVersionHash{$this_version}, "' failed: ",
				"expected 5 white-space separated tokens in the ASIO ",
				"'version.hpp' file, found ", ($#tmp_results + 1), ".\n";
			next;
		}
		if ($tmp_results[2] !~ /^\d+$/) {
			print STDERR "ERROR: Processing of ZIP file '",
				$GBAAVN_BoostVersionHash{$this_version}, "' failed: ",
				"expected an integer representation of the ASIO version number ",
				"in the 'version.hpp' file as token number 3, found '",
				$tmp_results[2], "' instead.\n";
			next;
		}
		if ($tmp_results[4] !~ /^\d+\.\d+\.\d+$/) {
			print STDERR "ERROR: Processing of ZIP file '",
				$GBAAVN_BoostVersionHash{$this_version}, "' failed: ",
				"expected a dotted ASIO version number ",
				"in the 'version.hpp' file as token number 5, found '",
				$tmp_results[4], "' instead.\n";
			next;
		}
		$version_file = "Boost_Version_" . $boost_version .
			"___CONTAINS___ASIO_Version_" . $tmp_results[4] . ".txt";
#print "[Boost=", $boost_version, "] ---> [ASIO=", $cmd_results[0], "]\n";
#print $version_file, "\n";
		if (!open(OUT_VERSION_FILE, ">" . $version_file)) {
			print STDERR "Unable to open version file '", $version_file,
				"' for writing.\n";
			next;
		}
		print OUT_VERSION_FILE $cmd_results[0], "\n";
		close(OUT_VERSION_FILE);
		print STDERR $version_file, " done.\n";
	}

	exit($return_code);

# #############################################################################

# #############################################################################
sub ReportFatalError {
	my($error_text) = @_;

	print "ERROR: ", $error_text, "\n";

	exit(-1);
}
# #############################################################################

# #############################################################################
sub ExitIfSystemError {
	my($error_text) = @_;

	if ($? == -1) {
		chop($!);
		my $sys_error = "";
		if ($error_text ne "") {
			$sys_error = $error_text;
			if ($! ne "") {
				$sys_error .= ": (error '" . $! . "')";
			}
		}
		else {
			ExitIfSystemError("Error encountered on system shell invocation");
		}
		ReportFatalError($sys_error);
	}
}
# #############################################################################


