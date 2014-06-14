#!/bin/bash

#
# plot a cactus plot with the first $cols columns of a file (the first one is not counted)
#

infile=${1}
cols=${2}
outfile=${3}

sep="$"
# split file into its rows

file=/tmp/tmpCactus.dat

HEADLINE=$( head $infile -n 1 )

n=`wc -l $infile|cut -d' ' -f1`
let n=$n-1
tail -n $n $infile > $file

for col in `seq 2 $(($cols+1))`
do
	cat "$file" | awk "{ if( $sep$col+0 >= 0 ) print $sep$col}" | sort -g > tmp$col.dat
done


# create plt file

echo "set terminal postscript eps color enhanced" > tmp.plt
#echo "set terminal postscript eps" > tmp.plt
echo "set key left top" >> tmp.plt
echo "set output \"$outfile.eps\"" >> tmp.plt
echo "set xlabel \"solved instances\"" >> tmp.plt
echo "set ylabel \"time in seconds\"" >> tmp.plt
#echo "set xrange [90:]" >> tmp.plt
#echo "set logscale x" >> tmp.plt
#echo "set logscale y" >> tmp.plt
echo "set xtic 2" >> tmp.plt
echo "set size 0.7,0.7" >> tmp.plt
echo "set style line 1 lt rgb \"#FF0000\"    lw 3.5 pt 1" >> tmp.plt
echo "set style line 2 lt rgb \"#000000\"  lw 3.5     " >> tmp.plt
echo "set style line 3 lt rgb \"#0000FF\"   lw 3.5 pt 2" >> tmp.plt
echo "set style line 4 lt rgb \"#999999\"   lw 4 pt 1" >> tmp.plt
echo "set style line 5 lt rgb \"#000000\"  lw 3.5 pt 4" >> tmp.plt
echo "set style line 6 lt rgb \"#006400\" lw 2.5 pt 6" >> tmp.plt
echo "set style line 7 lt rgb \"#FFA500\" lw 2.5 pt 7" >> tmp.plt
echo "set style line 8 lt rgb \"#0000FF\"   lw 2.5 pt 8" >> tmp.plt
echo "set style line 9 lt rgb \"#999999\"   lw 3 pt 9" >> tmp.plt
echo "set style line 10 lt rgb \"#000000\" lw 2.5 pt 0" >> tmp.plt
echo "set style line 11 lt rgb \"#FF0000\"   lw 2" >> tmp.plt
echo "set style line 12 lt rgb \"#8A2BE2\" lw 2 pt 11" >> tmp.plt
echo "set style line 13 lt rgb \"#0000FF\"  lw 2 pt 12" >> tmp.plt
echo "set style line 14 lt rgb \"#999999\"  lw 2.5 pt 13" >> tmp.plt
echo "set style line 15 lt rgb \"#000000\" lw 2 pt 8" >> tmp.plt
echo "set style line 16 lt rgb \"#006400\" lw 1 pt 8" >> tmp.plt
echo "set style line 17 lt rgb \"#FFA500\" lw 1 pt 9" >> tmp.plt
echo "set style line 18 lt rgb \"#0000FF\"  lw 1 pt 9" >> tmp.plt
echo "set style line 19 lt rgb \"#999999\"  lw 2" >> tmp.plt
echo "set style line 20 lt rgb \"#000000\" lw 1" >> tmp.plt
#echo "set size ratio 1.5" >> tmp.plt
echo "set title \"\"" >> tmp.plt
#echo "f(x)=900" >> tmp.plt
#echo "plot f(x) title \"SAT Race\" with lines linestyle 2, \\"  >> tmp.plt
#echo "\"Beleg.csv\" using 1 title \"Baseline\" with linespoints linestyle 1, \\" >> tmp.plt

echo "set title \"\"" >> tmp.plt
echo "plot \\"  >> tmp.plt

for col in `seq 2 $cols`
do
	des=$(($col-1))
	lines=$(($col+1))
	cHead=`echo "$HEADLINE" | awk -v x=$col '{print $x}'`
	echo "    \"tmp$col.dat\" using 1 title \"$cHead\" with linespoints linestyle $lines, \\" >> tmp.plt
done

des=$(($cols+1))
lines=$(($cols+3))


cHead=`echo "$HEADLINE" | awk -v x=$des '{print $x}'`

#echo "tmp$des.dat:"
#tail -n 5 tmp$des.dat

echo "    \"tmp$des.dat\" using 1 title \"$cHead\" with linespoints linestyle $lines" >> tmp.plt


# plot
cat tmp.plt | gnuplot

# convert to pdf
epstopdf $outfile.eps

#clean up
for col in `seq 2 $(($cols+1))`
do
	rm -f tmp$col.dat
done
rm -f tmp.plt
