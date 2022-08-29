La struttura di questa cartella è identica alla 09, se non per il fatto che ci sono due programmi da lanciare.
Chiamando make si compila solo mainmpi.exe, main.exe si compila chiamando esplicitamente "make main.exe"


Il programma mainmpi.exe è studiato per funzionare con 10 processi in contemporanea, con meno di quel numero non funzionerà correttamente, con più
di 10 non compierà comunicazioni con i processi in eccesso. Per farlo funzionare con meno processi è sufficiente cancellare le istanze della funzione
Communicate() in eccesso nel programma mainmpi.cpp. Bisogna chiamare Communicate un numero di volte uguale al numero di processi che si intendono
compiere in contemporanea.

Lanciando main.exe si lancia l'algoritmo per minimizzare il percorso tra le capitali americane con un solo processo, con mainmpi.exe 
si fa lo stesso con 10 processi.

Ogni processo dà degli output files segnati con il proprio numero progressivo. 

