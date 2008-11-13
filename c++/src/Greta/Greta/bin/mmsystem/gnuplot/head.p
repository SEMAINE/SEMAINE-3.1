plot  [-5:60] [-1.5:1.5] "../../logs/head.csv" using 1:8 title 'REP' with linespoints lt 7 lw 2.5, "../../logs/head.csv" using 1:3 title 'OAC' with linespoints lt 1 lw 2.5, "../../logs/head.csv" using 1:4 title 'SPC' with linespoints lt 2 lw 2.5, "../../logs/head.csv" using 1:5 title 'TMP' with linespoints lt 3 lw 2.5, "../../logs/head.csv" using 1:6 title 'FLD' with linespoints lt 4 lw 2.5, "../../logs/head.csv" using 1:7 title 'PWR' with linespoints lt 5 lw 2.5
set terminal postscript enhanced color
set title 'Expressivity of the head modality - Baseline 3'
set output "head.ps"
replot
set terminal windows
set size 1,1