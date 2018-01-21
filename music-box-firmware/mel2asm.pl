# perl mel2asm.pl < mel.txt > melody.asm


	foreach (<STDIN>) {
		chop;
		s/;.*$//;
		@lst = split(/ /);
		if(@lst < 2) { next; }
		$n = $lst[0] & 255; &putb;
		$n = $lst[0] >> 8; &putb;
		$p = 1;
		while($lst[$p] ne '') {
			$n = $lst[$p++];
			if($lst[$p] eq '') { $n .= "|en"; }
			&putb;
		}
	}

	print "\n;$cnt\n";

	exit;


sub putb
{
	$cnt++;
	if($c == 0) {
		print "\n\t.db $n";
	} else {
		print ", $n";
	}
	$c = ($c + 1) & 15;
}
