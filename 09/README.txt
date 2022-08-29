Il programma da lanciare è main.exe che genera i file:
- StopsCirc.txt -> stampa le posizioni dei punti disposti su un cerchio 
- Stops.txt -> stampa le posizioni dei punti disposti casualmente
- ChromosomesCirc.txt -> stampa la fitness e il contenuto del miglior percorso per ogni generazione (prima simulazione)
- Chromosomes.txt -> uguale al precedente per la seconda simulazione
- FitnessCirc.txt -> stampa la fitness del miglior percorso e la fitness media della generazione, per ogni generazione (prima simulazione)
- Fitness.txt -> come sopra ma per la seconda simulazione

Custom classes :
- Point contiene la coord x e la y. Usata per gli stop del percorso.
- Path descrive un percorso, composto da un vettore di interi. Ha i metodi per applicare mutazioni e crossing-over.
- Generation rappresenta una generazione dell'algoritmo, contiene un vettore di Point, che rappresenta gli stop da compiere per completare il pecorso, insieme
    ad un vettore di Path per rappresentare tutti i vari possibili percorsi che compongono una generazione. Contiene i metodi per passare da una generazione
    a quella seguente, e quelli per generare la prima.

Le classi e i metodi sono commentati nei rispettivi header files.