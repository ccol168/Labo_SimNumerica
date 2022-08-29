/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>

using namespace std;
 
int main (int argc, char *argv[]){

   Random rnd;
   int seed[4];
   int p1, p2;

//Selecting prime numbers
   ifstream Primes("Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2 ;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

//Setting seed, p1 and p2 from the file seed.in
   ifstream input("seed.in");
   string property;
   if (input.is_open()){
      while ( !input.eof() ){
         input >> property;
         if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed,p1,p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

//Checking input argument (see if necessary)
	//if (argc != 2 ) {
		//cout<< "Usage: <Quantity of number generated>" << endl;
		//exit(1);
	//}

	//int stop = atoi(argv[1]);
	
    int M = 10000;
    int N = 100; //how many running blocks
    double L = M/N;
        
    double rand_numbers[M];
    double med[N];
    double med2[N];
    double prog_sum[N];
    double prog_sum2[N];
    
    double sum;
    int k;

  	for(int i=0; i<M; i++){
     		rand_numbers[i] = rnd.Rannyu();
  	}

    for (int i=0; i<N; i++){
        sum = 0;
        for (int j=0; j<L; j++) {
            k = j+i*L;
            sum += rand_numbers[k];
        }
        med[i] = sum/L;
        med2[i] = pow(med[i],2);           
    }
    
    for(int i=0; i<N; i++) {
           for (int j=0; j<i+1; j++) {
               prog_sum[i] += med[j];
               prog_sum2[i] += med2[j];
           }
            prog_sum[i] /= (i+1);
            prog_sum2[i] /= (i+1) ;
    }
	
	for(int i=0; i<N; i++){
     	cout << prog_sum[i] << "  " << prog_sum2[i] << endl;
    }
	
	//cout << endl << "Stop  " << stop << endl;

	

    //per poter riprendere dal seme a cui siamo arrivati, in modo da continuare la sequenza
  	rnd.SaveSeed();
   	return 0;
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
