#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include "functions.h"
#include <algorithm>

using namespace std;

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

pair<double*,double*> running_averages (double* random_numbers, int N, int L) {
    
   double sum;
   int k;
   double* med = new double [N];
   double* med2 = new double [N];

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         k = j+i*L;
         sum += random_numbers[k];
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;

   return {prog_sum,err_prog};
}

double DirectSampling (int paces, Random& rnd, double inter, double vol, double T, double inPrice, double strikePrice) {

   double z;
   double S;
   double Cs = 0;

   for (int i=0; i<paces; i++) {
      z = rnd.Gauss(0,1);
      S = inPrice * exp((inter-(pow(vol,2)/2)*T)+vol*z*sqrt(T));
      Cs += exp(-inter*T)*max(0.,(S-strikePrice));
   }

   cout<<z<<"  ";
   return (Cs/paces);

}

double DiscretePath (int paces, Random& rnd, double inter, double vol, double T, double inPrice, double strikePrice) {

   double z;
   double Spart, Sarr;
   double tpart, tarr;
   double Cs = 0;

   for (int i=0; i<paces; i++) {
      Spart=inPrice;
      tpart=0;
      for (int j=0; j<100; j++) {
         z = rnd.Gauss(0.,1.);
         tarr = tpart+(j/100); 
         Sarr = Spart*exp(inter-0.5*pow(vol,2)*(tarr-tpart)+inter*z*sqrt(tpart-tarr));
         Spart = Sarr;
         tpart = tarr;
      }
      Cs += exp(-inter*T)*max(0.,(Sarr-strikePrice));
   }

   return Cs/paces;
}

 
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

int M = 1000;
double Dir_samplings[M];

for (int i=0; i<M; i++) {
   Dir_samplings[i] = DirectSampling(100,rnd,0.1,0.25,1.,100.,100.);
}


auto res_direct = running_averages(Dir_samplings,10,100);
double* Dir_samplings_mean = res_direct.first;
double* Dir_samplings_err = res_direct.second;

print(Dir_samplings_mean,Dir_samplings_err,100);

/*
double Discr_paths[M];

for (int i=0; i<M; i++) {
   Discr_paths[i] = DiscretePath(10,rnd,0.1,0.25,1.,100.,100.);
}

auto res_path = running_averages(Discr_paths,10,100);
double* Discr_paths_mean = res_path.first;
double* Discr_paths_err = res_path.second;

printout((char*)"Results.txt",Dir_samplings_mean,Dir_samplings_err,100);
*/
//Checking input argument (see if necessary)
	//if (argc != 2 ) {
		//cout<< "Usage: <Quantity of number generated>" << endl;
		//exit(1);
	//}

	//int stop = atoi(argv[1]);

   rnd.SaveSeed();

   return 0;
}
