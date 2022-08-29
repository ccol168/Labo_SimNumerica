#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include "functions.h"
#include <algorithm>

using namespace std;

//compie il sampling diretto della funzione
double DirectSampling (Random& rnd, double inter, double vol, double T, double inPrice, double strikePrice,bool isCall) {

   double z;
   double S;
   double Cs = 0;

   z = rnd.Gauss(0,1); //genero un numero casuale secondo una gaussiana
   S = inPrice * exp((inter-(pow(vol,2)/2)*T)+vol*z*sqrt(T)); //calcolo la gaussiana al tempo T in modo diretto
   if (isCall==true) {   
      Cs += exp(-inter*T)*max(0.,(S-strikePrice)); //ottengo la stima del valore della opzione
   } else if (isCall==false) {
      Cs += exp(-inter*T)*max(0.,strikePrice-S);
   }

   return Cs;
}

//compio il sampling della funzione dividendo il moto browniano da S(0) ad S(T) in 100 steps
double DiscretePath (Random& rnd, double inter, double vol, double T, double inPrice, double strikePrice, bool isCall) {

   double z;
   double Spart, Sarr;
   double Cs = 0;
   int numsteps=100; //numero di steps
   double step = 1./(double)numsteps; //lunghezza dello step


   Spart=inPrice;
   for (int j=0; j<numsteps; j++) { //vado a fare il percorso usando 100 step discreti
      z = rnd.Gauss(0.,1.);
      Sarr = Spart*exp((inter-0.5*pow(vol,2))*step+vol*z*sqrt(step));
      Spart = Sarr;
   }
   if (isCall==true) {   
      Cs += exp(-inter*T)*max(0.,(Sarr-strikePrice));
   } else if (isCall==false) {
      Cs += exp(-inter*T)*max(0.,strikePrice-Sarr);
   }
   

   return Cs;
}


//calcola le medie cumulative su N blocchi
pair <double*,double*> cumulative_calc (double* med, double* med2, int N) {

   double* prog_sum2 = new double [N];
   double* prog_sum = new double[N];
   double* err_prog = new double [N];

   for(int i=0; i<N; i++) {
      for (int j=0; j<i+1; j++) {
         prog_sum[i] += med[j];
         prog_sum2[i] += med2[j];
      }
   prog_sum[i] /= (i+1); //media cumulativa
	prog_sum2[i] /= (i+1) ;	//quadrato della media cumulativa
	err_prog[i] = error(prog_sum[i],prog_sum2[i],i) ; //incertezza statistica
   }

   return {prog_sum,err_prog};
}

//calcola la media e l'incertezza per ognuna delle stime del valore delle opzioni
pair<double*,double*> running_averages (Random& rnd, int M, int N, bool isDirectSampling, bool isCall) {

   if ( (M%N) != 0) {
      cerr<<"Error in running_averages: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 
    
   double sum;
   double* med = new double [N];
   double* med2 = new double [N];
   int L = M/N;

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         if (isDirectSampling) sum += DirectSampling(rnd,0.1,0.25,1.,100.,100.,isCall);
         else sum += DiscretePath(rnd,0.1,0.25,1.,100.,100.,isCall);
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;

   return {prog_sum,err_prog};
}
 
int main (int argc, char *argv[]){

   //inizializzazione del generatore di numeri casuali
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


   int M = 1000000; //volte in cui viene ripetuta la simulazione di S(T)
   int N = 100; //numero blocchi

   //direct sampling per opzioni call
   auto res_direct = running_averages(rnd,M,N,true,true);
   double* Dir_samplings_mean = res_direct.first;
   double* Dir_samplings_err = res_direct.second;

   //direct sampling per opzioni put
   auto res_direct_put = running_averages(rnd,M,N,true,false);
   double* Dir_samplings_mean_put = res_direct_put.first;
   double* Dir_samplings_err_put = res_direct_put.second;

   cout<<endl<<"Direct sampling calculations completed"<<endl<<endl;

   //discrete sampling per opzioni call
   auto res_path = running_averages(rnd,M,N,false,true);
   double* Discr_paths_mean = res_path.first;
   double* Discr_paths_err = res_path.second;

   //discrete sampling per opzioni put
   auto res_path_put = running_averages(rnd,M,N,false,false);
   double* Discr_paths_mean_put = res_path_put.first;
   double* Discr_paths_err_put = res_path_put.second;

   cout<<"Discrete sampling calculations completed"<<endl<<endl;

   //stampa dei risultati in due file, uno per le call e uno per le put
   printout((char*)"Results.txt",Dir_samplings_mean,Dir_samplings_err,Discr_paths_mean,Discr_paths_err,N);
   printout((char*)"Results_put.txt",Dir_samplings_mean_put,Dir_samplings_err_put,Discr_paths_mean_put,Discr_paths_err_put,N);


   rnd.SaveSeed();

   return 0;
}
