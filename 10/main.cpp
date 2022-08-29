#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include <sstream>
#include "functions.h"
#include "path.h"
#include "generation.h"


using namespace std;

bool IsDouble(string str) { //ritorna se l'oggetto in una stringa è un double o meno
    char* endptr = 0;
    const char * c = str.c_str();
    strtod(c, &endptr);
    if(*endptr != '\0' || endptr == c)
        return false;
    return true;
}


Generation SetStops(ifstream& instops) { //riempio la generazione con gli stop del file american captals.txt
   Generation Gen;
   string jump;
   double a,b;
   a = 0.;
   b = 0.;
   bool flag_a = false;
   bool flag_b = false;
   
   while (instops.peek()!=EOF ) {
      instops>>jump;
      if (IsDouble(jump) == true && flag_a == false) {
         a=stod(jump);
         flag_a = true;
      } else if (IsDouble(jump) == true && flag_b == false && flag_a==true) {
         b=stod(jump); 
         flag_b = true;
      } 
      if (flag_a == true && flag_b == true) {
         Gen.AppendStop(a,b);
         flag_a = false;
         flag_b = false;
      }
   }
   return Gen;
}

//fa la simulazione
void PrintGeneration(Random& rnd,Generation& StartGen,ofstream& outstops, ofstream& outfitness, ofstream& outchromosomes, int NGen, bool CircStops) {
   /*if (NGen>20000){
      cout<<"Save your computer: too many generations"<<endl;
      exit(1);
   }*/
   
   Generation NewGen;
   Generation OldGen;

   StartGen.StartingChromosomes(rnd,100);
   
   StartGen.PrintOutStops(outstops);
   
   OldGen.CopyGen(StartGen);
   
   for (int i=0;i<NGen;i++) {
      NewGen.NewGeneration(rnd,OldGen,4);
      outfitness<<NewGen.GetNumGen()<<"   "<<NewGen.BestFitness()<<"  "<<NewGen.MeanBestFitnesses()<<endl;
      NewGen.PrintOutBestChromosomes(outchromosomes,1);
      if (i%1000 == 0) cout<<"Made "<<i<<"th generation"<<endl;
      OldGen.CopyGen(NewGen);
      
   }
   return;
}
 
int main (int argc, char *argv[]){

   Random rnd = InitializeRandom();

   //metto gli stops
   ifstream instops("American_capitals.dat");
   Generation StartGen = SetStops(instops);
   instops.close();

   //apro l'ofstream
   ofstream outstops_rnd("Stops.txt");
   ofstream outfitness_rnd("Fitness.txt");
   ofstream outchromosomes_rnd("Chromosomes.txt");

   int NGenRnd=50000;

   //calcolo
   PrintGeneration(rnd,StartGen,outstops_rnd,outfitness_rnd,outchromosomes_rnd,NGenRnd,false);

   outstops_rnd.close();
   outfitness_rnd.close();
   outchromosomes_rnd.close();

   rnd.SaveSeed();
   return 0;
}

