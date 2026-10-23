#!/usr/bin/perl
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
		$prog_name = $0;
		$prog_name =~ s/^(.+(\/|\\))?(.+)$/$3/;
		print "USAGE:\n";
		print "   $prog_name ",
			"[ -extract-include-files ] ",
			"[ <src-dir> [ <src-dir> ... ] ]\n\n",
			"   -extract-include-files\n",
			"   ",
			"   If specified, the contents of boost/asio will be extracted.\n",
			"\n",
			"   <src-dir>\n",
			"      If not specified, defaults to the current directory.\n";
		print "\n";
		exit($return_code);
	}

	$GBAAVN_ExtractFlag      = 0;
	%GBAAVN_ExtractNamesHash = ();
	@GBAAVN_ExtractNamesList = ();
	%GBAAVN_BoostVersionHash = ();
	%GBAAVN_FileVersionHash  = ();

	if (grep(/^\-EXTRACT(_|\-)?INCLUDE((_|\-)?FILE)?S?=?/i,
		@ARGV)) {
		my %tmp_name_hash = ();
		my @tmp_argv      = ();
		my $this_argv;
		for $this_argv (@ARGV) {
			if ($this_argv =~ /^\-EXTRACT(_|\-)?INCLUDE((_|\-)?FILE)?S?=?/i) {
				my $sep_index = index($this_argv, "=");
				if ($sep_index > -1) {
					my $argv_name     =  substr($this_argv, $sep_index + 1);
					my $tmp_name      =  $argv_name;
					$tmp_name         =~ s/^\s+|\s+$//g;
					$tmp_name         =~ s/^,+|,+$//g;
					if ($tmp_name =~ /\s/) {
						ReportFatalError("Parameter '" . $this_argv .
							"' specifies a file name for extraction which contains " .
							"whitespace ('" . $argv_name . "').");
					}
					my @tmp_name_list =  split(/,+/, $tmp_name);
					for $tmp_name (@tmp_name_list) {
						if ($tmp_name !~ /^\s*$/) {
							$fixed_name                           =  $tmp_name;
							$fixed_name                           =~ s/\\/\//g;
							$fixed_name                           =~ s/\/\//\//g;
							if (index($fixed_name, "/boost/") == 0) {
								if (length($fixed_name) == 7) {
									next;
								}
								$fixed_name = substr($fixed_name, 7);
							}
							$formal_name                          =  $fixed_name;
							$formal_name                          =~ s/\//_/g;
							$GBAAVN_ExtractNamesHash{$fixed_name} =  $formal_name;
						}
					}
				}
			}
			else {
				push(@tmp_argv, $this_argv);
			}
		}
		@GBAAVN_ExtractNamesList = sort(keys(%GBAAVN_ExtractNamesHash));
		@ARGV = @tmp_argv;
		$GBAAVN_ExtractFlag = 1;
print "EXTRACT=[", join("]\n        [", @GBAAVN_ExtractNamesList), "]\n";
#exit(0);
	}

	@GBAAVN_DirList = @ARGV;

	if ($#GBAAVN_DirList == -1) {
		@GBAAVN_DirList = ($ENV{"PWD"});
	}


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
		# ASIO was first introduced into Boost version 1.36.0.
		if ($this_version lt "1_36_0") {
			next;
		}
		$version_name  =  $GBAAVN_FileVersionHash{$this_version};
		$boost_version =  $this_version;
		$boost_version =~ s/_/\./g;
#		$this_cmd = "unzip -c " . $GBAAVN_BoostVersionHash{$this_version} .
#			" boost_" . $this_version . "/boost/asio/version.hpp|" .
#			"grep '#define BOOST_ASIO_VERSION ' > Boost_" . $this_version .
#			"_AsioVersion.txt";
		my $this_path;
		my $this_file;
		my $this_cmd;
		if ($GBAAVN_ExtractFlag) {
			if ($#GBAAVN_ExtractNamesList == -1) {
				$this_path = "boost_" . $version_name . "/boost/";
				$this_cmd  = "unzip " . $GBAAVN_BoostVersionHash{$this_version} .
					" " . $this_path . "*";
				#print $this_cmd, "\n";
				`$this_cmd`;
				ExitIfSystemError("Unable to unzip contents of path '" .
					$this_path . "' from ZIP file '" .
					$GBAAVN_BoostVersionHash{$this_version} . "'.");
				print STDERR "Extracted Boost version ", $boost_version,
					" include files from /boost/.\n";
				next;
			}
			my $extract_count = 0;
			my $this_extract;
			for $this_extract (@GBAAVN_ExtractNamesList) {
				$this_path   = "boost_" . $version_name . "/boost/";
				$this_file   = $this_path . $this_extract;
				$dst_file    = "Boost_" . $boost_version . "_boost_" .
					$GBAAVN_ExtractNamesHash{$this_extract};
				$this_cmd    = "unzip -c -q " .
					$GBAAVN_BoostVersionHash{$this_version} . " " . $this_file .
					" > " . $dst_file;
				@cmd_results = `$this_cmd`;
				if ($? == -1) {
					print STDERR "Unable to unzip file '" , $this_file ,
						"' from ZIP file '" ,
						$GBAAVN_BoostVersionHash{$this_version} , "'.";
					next;
				}
				$extract_count++;
			}
			print STDERR "Extracted ", $extract_count, " of ",
				($#GBAAVN_ExtractNamesList + 1), " include files of Boost ",
				"version ", $boost_version, " from /boost.\n";
			next;
		}
		$this_path   = "boost_" . $version_name . "/boost/asio/";
		$this_file   = $this_path . "version.hpp";
		$this_cmd    = "unzip -c " . $GBAAVN_BoostVersionHash{$this_version} .
			" " . $this_file . "|grep '#define BOOST_ASIO_VERSION '";
#print $this_cmd, "\n";
		@cmd_results = `$this_cmd`;
		ExitIfSystemError("Unable to unzip file '" . $this_file . "' from ZIP " .
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
#		if ($GBAAVN_ExtractFlag) {
#			$this_cmd    = "unzip " . $GBAAVN_BoostVersionHash{$this_version} .
#				" " . $this_path . "*";
#			#print $this_cmd, "\n";
#			`$this_cmd`;
#			ExitIfSystemError("Unable to unzip contents of path '" . $this_path .
#				"' from ZIP file '" . $GBAAVN_BoostVersionHash{$this_version} .
#				"'.");
#		}
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


