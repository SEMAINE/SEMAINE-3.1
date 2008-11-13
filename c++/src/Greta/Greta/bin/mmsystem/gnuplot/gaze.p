plot  [-5:60] [-1.5:1.5] "../../logs/gaze.csv" using 1:2 title 'preference' with linespoints, "../../logs/gaze.csv" using 1:3 title 'OAC' with linespoints, "../../logs/gaze.csv" using 1:4 title 'SPC' with linespoints, "../../logs/gaze.csv" using 1:5 title 'TMP' with linespoints, "../../logs/gaze.csv" using 1:6 title 'FLD' with linespoints, "../../logs/gaze.csv" using 1:7 title 'PWR' with linespoints, "../../logs/gaze.csv" using 1:8 title 'REP' with linespoints
set terminal latex size 16cm,8cm roman 10
set title 'Gaze modality'
set output "gaze.tex"
replot
set terminal windows
set size 1,1