set terminal pngcairo size 800,600 background rgb 'white'
set output 'plots/portfolio_bar_chart.png'
set boxwidth 0.5
set style fill solid
set xlabel 'Company'
set ylabel 'Profit (in USD)'
set title 'portfolio of JohnJackson -Total Profit= -25605.8$'
set grid ytics
set xtics format ''
set xtics rotate by -45
set xtics ("TSLA" 0, "AMZN" 1, "MSFT" 2, "GOOGL" 3, "AAPL" 4)
set arrow from 0,0 to 5,0 nohead lt 1 lw 2 linecolor rgb 'black'
plot 'trash/portfolio_data.txt' using ($1==0 ? $2 : 1/0):xtic(3) with boxes lc rgb '#E31937' notitle, 'trash/portfolio_data.txt' using ($1==1 ? $2 : 1/0):xtic(3) with boxes lc rgb '#FF9900' notitle, 'trash/portfolio_data.txt' using ($1==2 ? $2 : 1/0):xtic(3) with boxes lc rgb '#0078D4' notitle, 'trash/portfolio_data.txt' using ($1==3 ? $2 : 1/0):xtic(3) with boxes lc rgb '#34A853' notitle, 'trash/portfolio_data.txt' using ($1==4 ? $2 : 1/0):xtic(3) with boxes lc rgb '#A2AAAD' notitle
