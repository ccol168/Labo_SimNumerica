#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include "functions.h"
#include "path.h"
#include "generation.h"


using namespace std;

//esegue una simulazione
void PrintGeneration(Random& rnd,ofstream& outstops, ofstream& outfitness, ofstream& outchromosomes, int NGen, bool CircStops) {
   if (NGen>20000){ // check per evitare di fare troppe generazioni. Usato in fase di scrittura (ma lo tengo per sicurezza)
      cout<<"Save your computer: too many generations"<<endl;
      exit(1);
   }
   //creo solo tre oggetti, ciclo tra questi tre
   Generation StartGen;
   Generation NewGen;
   Generation OldGen;

   // genero gli stop del percorso
   if (CircStops) StartGen.GenerateCircularStops(rnd,34);
   else StartGen.GenerateRandomStops(rnd,34);

   // genero i 100 path di partenza
   StartGen.StartingChromosomes(rnd,100);

   StartGen.PrintOutStops(outstops); //stampo gli stop generati
   OldGen.CopyGen(StartGen); //pongo OldGen = StartGen

   for (int i=0;i<NGen;i++) {
      NewGen.NewGeneration(rnd,OldGen,4); // creo una nuova generazione NewGen a partire da quella vecchia OldGen, applicando mutazioni e crossing over
      outfitness<<NewGen.GetNumGen()<<"   "<<NewGen.BestFitness()<<"  "<<NewGen.MeanBestFitnesses()<<endl; // stampo le fitness di NewGen
      NewGen.PrintOutBestChromosomes(outchromosomes,1); // stampo il miglior percorso di NewGen
      OldGen.CopyGen(NewGen); // OldGen = NewGen
   }
   return;
}
 
int main (int argc, char *argv[]){

   Random rnd = InitializeRandom();

   // risolvo il primo problema, con punti lungo un cerchio
   ofstream outstops("StopsCirc.txt");
   ofstream outfitness("FitnessCirc.txt");
   ofstream outchromosomes("ChromosomesCirc.txt");

   int NGenCirc = 5000;

   PrintGeneration(rnd,outstops,outfitness,outchromosomes,NGenCirc,true);
   
   outstops.close();
   outfitness.close();
   outchromosomes.close();

   // risolvo il secondo problema, con punti casuali sul piano
   ofstream outstops_rnd("Stops.txt");
   ofstream outfitness_rnd("Fitness.txt");
   ofstream outchromosomes_rnd("Chromosomes.txt");

   int NGenRnd=5000;

   PrintGeneration(rnd,outstops_rnd,outfitness_rnd,outchromosomes_rnd,NGenRnd,false);

   outstops_rnd.close();
   outfitness_rnd.close();
   outchromosomes_rnd.close();

   rnd.SaveSeed();
   return 0;
}

