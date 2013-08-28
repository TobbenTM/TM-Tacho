#!/usr/bin/perl
use strict;
use IO::Socket::INET;

my $listenNIC = "10.13.37.1";
my $listenPort = "2002";
my $monitorNIC = "eth3";
my ($sock, $clientSocket);
my ($rx1, $rx2, $rx_bps, $rx_kbps, $tx1, $tx2, $tx_bps, $tx_kbps);

my $rxPath = '/sys/class/net/' . $monitorNIC . '/statistics/rx_bytes';
my $txPath = '/sys/class/net/' . $monitorNIC . '/statistics/tx_bytes';

$sock = IO::Socket::INET->new(
	LocalAddr => $listenNIC,
	LocalPort => $listenPort,
	Proto => "tcp",
	Listen => 5,
	Reuse => 1
) or die "ERROR: Could not create socket\n";

sub readFile {
	my $path = @_[0];
	open(FILE, "<", $path) or die "ERROR: Could not open file\n";
	my $line = <FILE>;
	close(FILE) or die "ERROR: Could not close file\n";
	return $line;
}

while(1){
	$clientSocket = $sock->accept();
	while(1){
		$rx1 = readFile($rxPath);
		$tx1 = readFile($txPath);
		sleep(1);
		$rx2 = readFile($rxPath);
		$tx2 = readFile($txPath);

		$rx_bps = $rx2 - $rx1;
		$rx_kbps = $rx_bps / 1024;
		$tx_bps = $tx2 - $tx1;
		$tx_kbps = $tx_bps / 1024;

		$clientSocket->send(int($rx_kbps) . '|' . int($tx_kbps) . "\n");
	}
}

$sock->close();
