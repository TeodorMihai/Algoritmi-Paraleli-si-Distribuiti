#!/bin/bash


for i in `seq 1 1`;
do
	make clean
	make build
	rm mypic1.pgm mypic2.pgm mypic3.pgm mypic4.pgm
	make run
	diff mypic4.pgm pic4-blur.pgm
	diff mypic1.pgm pic1-sharpen.pgm
	diff mypic3.pgm pic3-m_removal.pgm
	diff mypic2.pgm pic2-smooth.pgm
	echo $i
done