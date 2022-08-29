#ifndef __Generation__
#define __Generation__

#include "path.h"
#include "point.h"
#include <map>
#include <iostream>

class Generation {

    private:
    int m_NumGen; //numero della generazione

    Path* m_chromosomes; //pointer al vettore di path
    int m_NChromosomes; //numero di path 

    Point* m_stops; //stops del percorso
    int m_NStops; //numero di stops

    std::multimap<double,Path> m_order; //contiene i path ordinati per fitness
    bool m_ordered; // True se m_order è stato riempito, False altrimenti

    public:
    //constructor
    Generation(); 
    //destructor
    ~Generation();

    //methods

    void AppendStop(double,double); //aggiunge uno stop (x,y) a m_stops
    // ottengo le quantità private
    double GetNChromosomes () {return m_NChromosomes;}
    double GetNStops() {return m_NStops;}
    Point& GetStop(int i) {return m_stops[i-1];}
    Path& GetChromosome (int i) {return m_chromosomes[i-1];}
    Path& GetOrderedChromosome (int); // ottengo il path con la i-esima fitness più bassa (GetOrderedChromosome(1) dà il miglior path)
    bool IsOrdered () {return m_ordered;}
    int GetNumGen () {return m_NumGen;}
    double CalculateChromosomeFitness(int); //calcola la fitness del path in posizione i di m_chromosomes
    double CalculatePathFitness(Path,int); //calcola la fitness di un path esterno confrontandolo con m_stops

    //per creare e impostare correttamente le generazioni
    void GenerateCircularStops(Random&,int); //genera stops su un cerchio
    void GenerateRandomStops(Random&,int); // genera stops disposti casualmente sul piano
    void StartingChromosomes(Random&,int); // genera path casuali per la prima generazione, imposta m_NumGen = 1
    void OrderChromosomes(); // ordina i path per fitness riempiendo m_ordered
    void CopyGen(Generation&); // clona la generazione corrente
    void NewGeneration (Random&, Generation&); //crea una nuova generazione da quella vecchia, con mutazioni e crossing-over. Aumenta m_NumGen di 1
    void NewGeneration (Random&, Generation&, double); //come NewGeneration, ma usa un esponente alpha a scelta invece del default alpha=10
    void CrossGen (Random&,Path*,int,double); //mette casualmente alcuni percorsi di un vettore di path lungo int all'interno della generazione

    //per stampare e ritornare i risultati
    void PrintStops(); //stampa gli stops della generazione nello standard output
    void PrintOutStops(std::ofstream&); //stampa su file gli stops della generazione 
    void PrintChromosomes(); //stampa tutti i path della generazione nello standard output
    void PrintOrder(); //stampa tutti i path in ordine nello standard output
    void PrintOutOrder(std::ofstream&); //stampa su file tutti i path in ordine
    double BestFitness(); //ritorna la fitness più bassa
    double MeanBestFitnesses(); //ritorna la media delle migliori m_N/2 fitness
    double MeanBestFitnesses(int); //ritorna la media delle migliori i fitness
    void PrintBestChromosomes(); //stampa la gli m_N/2 cromosomi con fitness più bassa
    void PrintBestChromosomes(int); //stampa i migliori i cromosomi
    //come PrintBestChromosomes ma stampa su file 
    void PrintOutBestChromosomes(std::ofstream&);
    void PrintOutBestChromosomes(std::ofstream&,int);
    
};

#endif //__Generation__