#!/bin/bash
# Clears all ports needed for snapshot

for i in 15457 15458 15459 15460 15461 15462 15462
do
	fuser $i/udp -k
done
