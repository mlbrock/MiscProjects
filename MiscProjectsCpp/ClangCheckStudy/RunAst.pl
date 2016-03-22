#!/bin/perl

	foreach $this_file (@ARGV) {
		@ast_lines = `clang -cc1 -ast-dump $this_file`;
		if ($? == -1) {
			$tmp_error = "ERROR: Attempt to run clang for source file '".
				$this_file."' failed: " . $!;
			ReportFatalError($tmp_error);
		}
		chop @ast_lines;
		print "// "; print "/" x 76; print "\n";
		print "// "; print "/" x 76; print "\n";
		print "// File Name: ", $this_file, "\n";
		print "// "; print "/" x 76; print "\n";
		print join("\n", @ast_lines), "\n";
		print "// "; print "/" x 76; print "\n";
		print "\n";
	}

	exit(0);

# #############################################################################
sub ReportFatalError {
	local($error_text) = @_;

	print "ERROR: ", $error_text, "\n";

	exit(-1);
}
# #############################################################################
