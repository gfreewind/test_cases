#!/usr/bin/php
<?php

function factorial($n)
{
	if ($n == 0) {
		return 0;
	}

	$ret = 1;
	for ($i = 1; $i <= $n; ++$i) {
		$ret = $ret * $i;
	}

	return $ret;
}


if ($argc < 2) {
	echo "Please specify the number which need calculate its factorial\n";
	return 1;
}

$num = $argv[1];
$loops = 1;

if ($argc == 3) {
	$loops = $argv[2];
}

for ($i = 0; $i < $loops; ++$i) {
	$ret = factorial($num);
}

echo "$ret\n";
?>
