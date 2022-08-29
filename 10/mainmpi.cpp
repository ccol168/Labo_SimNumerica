#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include <sstream>
#include "functions.h"
#include "path.h"
#include "generation.h"
#include "mpi.h"


using namespace std;

bool IsDouble(string str) {
    char* endptr = 0;
    const char * c = str.c_str();
    strtod(c, &endptr);
    if(*endptr != '\0' || endptr == c)
        return false;
    return true;
}


Generation SetStops(ifstream& instops) {
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

//scambio tra i processi i migliori percorsi
Generation Communicate (Random& rnd,int i, Generation NewGen,int pace, int rank, int ChooseRank) {

   int* Communication_vec = new int[50]; //vettore lungo quanto il numero di stops, nel nostro caso sono 50
   Path* Best_gens = new Path[10]; //un processo condivide con gli altri i suoi migliori 10 paths

   if ((i+pace)%50 == 1) { //ogni tot passi le varie simulazioni iniziano a scambiarsi informazioni

      for (int i=0; i<10; i++) {
         Path oldpath(50);
         oldpath = NewGen.GetOrderedChromosome(i+1);
         Communication_vec = oldpath.GetPath(); //creo il vettore da mandare in broadcast

         MPI_Bcast(Communication_vec,50,MPI_INTEGER,ChooseRank,MPI_COMM_WORLD);
         Path newpath(50,Communication_vec);
         Best_gens[i] = newpath; //raccolgo in Best_gens i migliori path
      }
      
      NewGen.CrossGen(rnd,Best_gens,10,5.); //mescolo le generazioni
      return NewGen;

   } else {
      return NewGen;
   }
}


void PrintGeneration(Random& rnd,Generation& StartGen,ofstream& outstops, ofstream& outfitness, ofstream& outchromosomes, int NGen, int rank) {
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
      //mando in broadcast un processo alla volta, sfasando di 1000 generazioni tra i vari processi
      NewGen = Communicate(rnd,i,NewGen,51,rank,0);
      NewGen = Communicate(rnd,i,NewGen,52,rank,1);
      NewGen = Communicate(rnd,i,NewGen,53,rank,2);
      NewGen = Communicate(rnd,i,NewGen,54,rank,3);
      NewGen = Communicate(rnd,i,NewGen,55,rank,4);
      NewGen = Communicate(rnd,i,NewGen,56,rank,5);
      NewGen = Communicate(rnd,i,NewGen,57,rank,6);
      NewGen = Communicate(rnd,i,NewGen,58,rank,7);
      NewGen = Communicate(rnd,i,NewGen,59,rank,8);
      NewGen = Communicate(rnd,i,NewGen,60,rank,9);

      outfitness<<NewGen.GetNumGen()<<"   "<<NewGen.BestFitness()<<"  "<<NewGen.MeanBestFitnesses()<<endl;
      NewGen.PrintOutBestChromosomes(outchromosomes,1);
      OldGen.CopyGen(NewGen);

      if (i%5000==0) cout<<"Made "<<i<<"th generation for process #"<<rank<<endl;

   }
   return;
}
 
int main (int argc, char *argv[]){
   
   int size,rank;
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   Random rnd = InitializeRandom(rank,rank+7,rank*40-8,rank-50); //uso 4 rank in parallelo

   ifstream instops("American_capitals.dat");
   Generation StartGen = SetStops(instops);
   instops.close();

   string rankstr = to_string(rank);
   ofstream outstops_rnd("MPIStops_"+rankstr+".txt");
   ofstream outfitness_rnd("MPIFitness_"+rankstr+".txt");
   ofstream outchromosomes_rnd("MPIChromosomes_"+rankstr+".txt");

   int NGenRnd=10000;

   PrintGeneration(rnd,StartGen,outstops_rnd,outfitness_rnd,outchromosomes_rnd,NGenRnd,rank);

   outstops_rnd.close();
   outfitness_rnd.close();
   outchromosomes_rnd.close();

   rnd.SaveSeed();

   MPI_Finalize(); 
   return 0;
}