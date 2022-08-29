#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include "functions.h"

using namespace std;

//definisco in modo comodo la posizione della particella
struct position {
   double x;
   double y;
   double z;
};

//ritorna la distanza della particella dal centro
double r (position pos) {
   return sqrt(pow(pos.x,2)+pow(pos.y,2)+pow(pos.z,2));
}

//ritorna la theta della particella data la posizione
double theta(position pos) {
   if (pos.x==0. && pos.y>0.) {
      return M_PI_2;
   } else if (pos.x==0. && pos.y<0.) {
      return 3*M_PI_2;
   } else if (pos.x>0. && pos.y>=0.) {
      return atan(pos.y/pos.x);
   } else if ((pos.x>0. && pos.y<0.) || (pos.x<0. && pos.y>0.)) {
      return atan(pos.y/pos.x) + 2*M_PI;
   } else if (pos.x<0. && pos.y<=0.) {
      return atan(pos.y/pos.x) + M_PI;
   } else if (pos.x==0. && pos.y==0.) {
      return 0.;
   } else {
      cerr<<"Error in theta: unexpected value" << endl;
      exit(1);
   }
}

//calcolo la probabilità usando la funzione d'onda di un orbitale 1s
double prob1s (position pos) {
   return pow(1/sqrt(M_PI)*pow(M_E,-r(pos)),2);
}

//calcolo la probabilità usando la funzione d'onda di un orbitale 2s
double prob2s (position pos) {
   return pow(1./8.*sqrt(2/M_PI)*r(pos)*pow(M_E,-r(pos)/2)*cos(theta(pos)),2);
}

//algoritmo di Metropolis usando step determinati da una distribuzione di prob uniforme
position MetropolisStepFlat (Random& rnd, position in_pos, bool Function1s,double par) {
   
   position test_pos;

   // genero il nuovo punto 
   test_pos.x = in_pos.x + par*rnd.Rannyu(-1,1);
   test_pos.y = in_pos.y + par*rnd.Rannyu(-1,1);
   test_pos.z = in_pos.z + par*rnd.Rannyu(-1,1);

   double alpha;

   if (Function1s) alpha=min(1.,prob1s(test_pos)/prob1s(in_pos));
   else alpha=min(1.,prob2s(test_pos)/prob2s(in_pos));

   double acceptLimit = rnd.Rannyu();

   //determina se il nuovo step viene accettato o meno
   if (acceptLimit<=alpha) {
      return test_pos;
   } else {
      return in_pos;
   }
} 

//algoritmo di Metropolis usando step determinati da una distribuzione gaussiana centrata nella posizione iniziale (processo ripetuto per ogni coordinata)
position MetropolisStepGauss (Random& rnd, position in_pos, bool Function1s,double sigma) {
   
   position test_pos;

   // genero il nuovo punto
   test_pos.x = rnd.Gauss(in_pos.x,sigma);
   test_pos.y = rnd.Gauss(in_pos.y,sigma);
   test_pos.z = rnd.Gauss(in_pos.z,sigma);

   double alpha;

   if (Function1s) alpha=min(1.,prob1s(test_pos)/prob1s(in_pos));
   else alpha=min(1.,prob2s(test_pos)/prob2s(in_pos));

   double acceptLimit = rnd.Rannyu();

   if (acceptLimit<=alpha) {
      return test_pos;
   } else {
      return in_pos;
   }
} 

//ottengo le medie e le incertezze cumulative
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

//compie il calcolo e ritorna i risultati all'aumentare dei blocchi
pair<double*,double*> running_averages (Random& rnd, int M, int N, position start_pos,bool Function1s,double par, bool FlatStep) {
    
   double sum;
   double* med = new double [N];
   double* med2 = new double [N];
   
   
   if ( (M%N) != 0) {
      cerr<<"Error in running_averages: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 

   int L = M/N;

   position in_pos = start_pos;
   position fin_pos;
   int Rejected = 0;

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         if (FlatStep) fin_pos = MetropolisStepFlat(rnd,in_pos,Function1s,par);
         else fin_pos = MetropolisStepGauss(rnd,in_pos,Function1s,par);

         if (fin_pos.x==in_pos.x && fin_pos.y==in_pos.y && fin_pos.z==in_pos.z)  Rejected++; //tengo conto degli step rejected

         sum += r(fin_pos); //aggiorno il valore
         in_pos=fin_pos;
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   cout<<"Acceptance Rate: "<< 1. - (double)Rejected/(double)M<<endl;

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;

   return {prog_sum,err_prog};
}

 
int main (int argc, char *argv[]){

   Random rnd = InitializeRandom();

   position start;
   start.x = 0.;
   start.y = 0.;
   start.z = 0.;

   int M=1000000;
   int N=100;

   auto resultsflat1s = running_averages(rnd,M,N,start,true,1.2,true);
   double* valflats1s = resultsflat1s.first;
   double* errflats1s = resultsflat1s.second;

   auto resultsflat2s = running_averages(rnd,M,N,start,false,3.1,true);
   double* valflats2s = resultsflat2s.first;
   double* errflats2s = resultsflat2s.second;

   auto resultsgauss1s = running_averages(rnd,M,N,start,true,0.75,false);
   double* valgauss1s = resultsgauss1s.first;
   double* errgauss1s = resultsgauss1s.second;

   auto resultsgauss2s = running_averages(rnd,M,N,start,false,1.95,false);
   double* valgauss2s = resultsgauss2s.first;
   double* errgauss2s = resultsgauss2s.second;
   
   printout((char*)"Results.txt",valflats1s,errflats1s,valflats2s,errflats2s,valgauss1s,errgauss1s,valgauss2s,errgauss2s,N);

   //provo a fare un test partendo in un punto con probabilità molto bassa
   position farstart;
   farstart.x = 100.;
   farstart.y = 100.;
   farstart.z = 100.;

   auto farstart_gauss2s = running_averages(rnd,M,N,farstart,false,1.95,false);
   double* farstart_val = farstart_gauss2s.first;
   double* farstart_err = farstart_gauss2s.second;

   printout((char*)"Farstart.txt",farstart_val,farstart_err,N);
	
	rnd.SaveSeed();
   return 0;
}


