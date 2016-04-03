#!/bin/bash
id=`id -g`
for i in  include/cryptopp bin lib share/cryptopp; do
	echo "doing $i"
	sudo mkdir -p /usr/local/$i
	sudo chown .${id} /usr/local/$i 
	sudo chmod g+ws /usr/local/$i
done

cmake .
make 

