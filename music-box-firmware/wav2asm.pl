# waveform to ASM converter

	if(!open(RHA, "p1.wav")) # source file
		{ die "File could not opend." }
	binmode RHA;

	read(RHA, $sign, 4);
	if($sign ne "RIFF") { die "Not wav file"; }
	read(RHA, $dmy, 4);
	read(RHA, $sign, 4);
	if($sign ne "WAVE") { die "Not wav file"; }

	read(RHA, $sign, 4);
	if($sign ne "fmt ") { die "Not fmt"; }
	read(RHA, $siz, 4);
	$lfmt = unpack('L4', $siz);
	if($lfmt > 100) { die "Invalid format"; }
	read(RHA, $cfmt, $lfmt);
	$id = unpack('S2', substr($cfmt, 0, 2));
	$nchan = unpack('S2', substr($cfmt, 2, 2));
	$nsamp = unpack('L4', substr($cfmt, 4, 4));
	$nrate = unpack('L4', substr($cfmt, 8, 4));
	$nblk = unpack('S2', substr($cfmt, 12, 2));
	$nbit = unpack('S2', substr($cfmt, 14, 2));
	print "id = $id\n";
	print "channels = $nchan ch\n";
	print "sampling freq = $nsamp Hz\n";
	print "data rate = $nrate bytes/sec\n";
	print "block size = $nblk bytes\n";
	print "bit/sample = $nbit bit\n";

	if($id != 1) { die "Invalid format (not LPCM)"; }		# linear?
	if($nchan != 1) { die "Invalid format (not Mono)"; }	# mono?
	if($nbit != 16) { die "Invalid format (not 16bit)"; }	# 16bit?

	read(RHA, $sign, 4);
	if($sign ne "data") { die "No data"; }
	read(RHA, $siz, 4);
	$ldata = unpack('L4', $siz) / 2;
	print "samples = $ldata\n";

	$c = 0;
	while ($ldata--) {
		read(RHA, $bdat, 2);
		$val = int(unpack('s2', $bdat) / 256);
		if($c == 0) {
			print "\n\t.db $val";
		} else {
			print ", $val";
		}
		$c = ($c+1) & 15;
	}
	print "\n";

	close RHA;

	exit;

