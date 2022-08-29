Lo shell script RunSim.txt serve per lanciare il programma Monte_Carlo_ISING.exe più volte di seguito, in modo da ottenere in una sola volta i vari
parametri del sistema in funzione della temperatura.

Il programma in Python input_man.py serve allo shell script per manipolare correttamente il file input.dat e quindi lanciare la simulazione di Ising
con i parametri corretti. 

L'energia interna, la suscettività magnetica e la capacità termica in funzione della temperatura sono stampate in Simulation_results.txt, mentre la magnetizzazione
con h=0.02 è riportata a parte in Magnetization_results.txt. 
I file di output predefiniti non sono stati utilizzati per visualizzare i risultati della simulazione.

I risultati usati nel notebook si trovano nelle cartelle Gibbs_res e Metropolis_res.
