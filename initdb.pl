#!/usr/bin/perl
#!/usr/bin/perl

use DBI;
use strict;

my $driver   = "SQLite";
my $database = "eventlog.db";
my $dsn = "DBI:$driver:dbname=$database";
my $userid = "";
my $password = "";


sub createLocationTable
{
  my $dbh = shift;
   
  my $stmt = qq( CREATE TABLE locations( location_id INTEGER PRIMARY KEY NOT NULL, location_name TEXT NOT NULL ); );
  my $rv = $dbh->do($stmt);
  if($rv < 0){
     print $DBI::errstr;
  } else {
     print "locations Table created successfully\n";
  }
   


}




my $dbh = DBI->connect($dsn, $userid, $password, { RaiseError => 1 })
                      or die $DBI::errstr;
print "Opened database successfully\n";

createLocationTable($dbh);



$dbh->disconnect();
