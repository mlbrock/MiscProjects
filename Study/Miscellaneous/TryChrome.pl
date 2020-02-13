#!/usr/bin/perl

#https://metacpan.org/pod/WWW::Mechanize

#use Log::Log4perl qw(:easy);
#use WWW::Mechanize::Chrome;
use WWW::Mechanize();

#my $mech = WWW::Mechanize::Chrome->new();
my $mech = WWW::Mechanize->new();
$mech->get('https://google.com');
 
#$mech->eval_in_page('alert("Hello Chrome")');
#my $png = $mech->content_as_png();

print "=" x 79; print "\n";
$mech->dump_headers();
print "-" x 79; print "\n";
$mech->dump_links();
print "-" x 79; print "\n";
$mech->dump_images();
print "-" x 79; print "\n";
$mech->dump_forms();
print "-" x 79; print "\n";
$mech->dump_text();
print "=" x 79; print "\n";


