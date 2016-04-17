#!/bin/bash

result=

factorial() {
	n=$1
	result=1
	for i in $(seq 1 $n); do
		result=$((result*$i))
	done
}

if [ $# -lt 1 ]; then
	echo "Please specify the number which need calculate its factorial"
	exit 1
fi 

loops=1
if [ $# -ge 2 ]; then
	loops=$2
fi

for i in $(seq 1 $loops); do
	factorial $1
done

echo $result
