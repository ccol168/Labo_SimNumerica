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
#include "functions.h"

using namespace std;

struct position {
   double x;
   double y;
   double z;
};


double ExpoCut (Random& rnd) {
   double Try;
   for (;;) {
      Try = rnd.Expo(1.,1.-1./M_E);
      if (Try < 1.) {
         return Try;
      } else {
         continue;
      }
   }
}


double func (double x) {
   return (M_PI/2.)*cos(M_PI*x/2.);
}

double func2 (double x){
   return func(x)*(1.-(1/M_E))/(pow(M_E,-x));
}

double distance (position pos) {
   return sqrt(pow(pos.x,2)+pow(pos.y,2)+pow(pos.z,2));
}

position DiscrStep (Random& rnd, position in_pos) {
   double step = rnd.Rannyu();
   position fin_pos; 
   fin_pos.x = in_pos.x;
   fin_pos.y = in_pos.y;
   fin_pos.z = in_pos.z;

   if (step <= 1./6.) {
      fin_pos.x = in_pos.x + 1.;
      return fin_pos;
   } else if (step>1./6. && step<= 2./6.) {
      fin_pos.x = in_pos.x - 1. ;
      return fin_pos;
   } else if (step>2./6. && step <= 3./6.) {
      fin_pos.y = in_pos.y + 1.;
      return fin_pos; 
   } else if (step>3./6. && step <= 4./6.) {
      fin_pos.y = in_pos.y - 1.;
      return fin_pos;
   } else if (step>4./6. && step <= 5./6.) {
      fin_pos.z = in_pos.z + 1.;
      return fin_pos;
   } else if (step > 5./6.)  {
      fin_pos.z = in_pos.z - 1.;
      return fin_pos;
   }
}


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

pair <double,double> cumulative_calc_single (double* med, double* med2, int N) {

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

   return {prog_sum[N-1],err_prog[N-1]};
}

pair<double*,double*> running_averages_fast (Random& rnd, int M, int N, bool ImpSamp) {
    
   double sum;
   double* med = new double [N];
   double* med2 = new double [N];
   
   if ( (M%N) != 0) {
      cerr<<"Error in running_averages: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 

   int L = M/N;

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         if (ImpSamp==false) {
            sum += func(rnd.Rannyu());
         } if (ImpSamp == true){
            sum += func2(ExpoCut(rnd));
         }
         
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;

   return {prog_sum,err_prog};
}

pair <double*,double*> RandomWalk (Random& rnd,int M, int N, int t) {

   if ( (M%N) != 0) {
      cerr<<"Error in RandomWalk: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 
   int L = M/N;

   position* points = new position[M];
   for (int i=0; i<M; i++) {
      points[i].x = 0.;
      points[i].y = 0.;
      points[i].z = 0.;
   }

   int l;
   double sum;
   double* med = new double[N];
   double* med2 = new double[N];
   double* ave_dist = new double[t];
   double * ave_err = new double[t];


   for (int i = 0 ; i<t; i++) {
      for (int j=0; j<N; j++) {
         sum = 0.;
         for (int k=0; k<L; k++) {
            l = k+j*L;
            points[l] = DiscrStep(rnd,points[l]);
            sum += distance(points[l]);
         }
         med[j] = sum/(double)L;
         med2[j] = pow(med[j],2);
      }
      auto results = cumulative_calc_single(med,med2,N);
      ave_dist[i] = results.first;
      ave_err[i] = results.second;
   }

   return {ave_dist,ave_err};
}
 
int main (int argc, char *argv[]){

   Random rnd = InitializeRandom();
	
   int M = 10000;
   int N = 100; //how many running blocks
   
   auto results1 = running_averages_fast(rnd,M,N,false);
   double* prog_sum = results1.first;
   double* err_prog = results1.second; 

   auto results2 = running_averages_fast(rnd,M,N,true);
   double* prog_sum_imp = results2.first;
   double* err_prog_imp = results2.second; 

   //printout((char*)"Data.txt",prog_sum,err_prog,prog_sum_imp,err_prog_imp,N);

   int Walks = 10000;
   int WalkBlocks = 100;
   int WalkTime = 1000;
   auto results = RandomWalk(rnd,Walks,WalkBlocks,WalkTime);
   double* ave_dist = results.first;
   double* ave_err = results.second;

   cout<<endl<<"RESULTS"<<endl;
   print(ave_dist,ave_err,WalkTime);


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
