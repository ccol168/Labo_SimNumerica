Il programma mainmpi.exe è studiato per funzionare con 10 processi in contemporanea, con meno di quel numero non funzionarà correttamente, con più
di 10 non compierà comunicazioni con i processi in eccesso. Per farlo funzionare con meno processi è sufficiente cancellare le istanze della funzione
Communicate() in eccesso nel programma mainmpi.cpp. Devo chiamare Communicate un numero di volte uguale al numero di processi che si intendono
compiere in contemporanea.

Lanciando main.exe si lancia l'algoritmo per minimizzare il percorso tra le capitali americane con un solo processo, con mainmpi.exe 
si fa lo stesso con 10 processi.