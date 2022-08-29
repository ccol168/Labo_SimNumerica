#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include "functions.h"
#include "main.h"

using namespace std;

//calcolo funzione d'onda
double wavefunction (parameters in) {
   return exp(-pow(in.pos-in.mu,2)/(2*pow(in.sigma,2))) + exp(-pow(in.pos+in.mu,2)/(2*pow(in.sigma,2)));
}

//calcolo il modulo quadro della funzione d'onda
double square_mod (parameters in) {
   return pow(wavefunction(in),2);
}

//calcolo la derivata seconda della f d'onda
double second_derivative (parameters in) {
   return 1/pow(in.sigma,2)*((pow(in.pos-in.mu,2)/pow(in.sigma,2))-1)*exp(-pow(in.pos-in.mu,2)/(2*pow(in.sigma,2))) + (1/pow(in.sigma,2))*((pow(in.pos+in.mu,2)/pow(in.sigma,2))-1)*exp(-pow(in.pos+in.mu,2)/(2*pow(in.sigma,2)));
}

//calcolo il potenziale
double potential (double x) {
   return pow(x,4) - 2.5*pow(x,2);
}

//E(x) = (-0.5/Psi_t(x))*(d^2/dx^2(Psi_t(x))) + V(x) implementata
double hamiltonian (parameters in) {
   return (-0.5/wavefunction(in))*second_derivative(in) + potential(in.pos);
}


//muove tramite Metropolis la posizione per la quale valutare l'energia
parameters MetropolisHamIntegral (parameters in_pos,double par) {
   
   parameters test_pos;
   // genero il nuovo punto
   test_pos.pos = in_pos.pos + par*rnd.Rannyu(-1,1);
   test_pos.mu = in_pos.mu;
   test_pos.sigma = in_pos.sigma;

   double alpha;

   alpha = min(1.,square_mod(test_pos)/square_mod(in_pos)); //acceptance rate

   double acceptLimit = rnd.Rannyu();

   if (acceptLimit<=alpha) {
      return test_pos;
   } else {
      return in_pos;
   }
} 

//passa dalla media classica alla media cumulativa
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

//calcola il valore di E(x), fissate sigma e mu
pair<double*,double*> running_averages (int M, int N, parameters start_pos,double par) {
    
   double sum;
   double* med = new double [N];
   double* med2 = new double [N];
   
   
   if ( (M%N) != 0) {
      cerr<<"Error in running_averages: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 

   int L = M/N;

   parameters in_pos = start_pos;
   parameters fin_pos;
   int Rejected = 0;

   //faccio convergere l'algoritmo

   for (int i=0; i<ConvTime; i++) {
      fin_pos = MetropolisHamIntegral(in_pos,par);
   }

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         
         fin_pos = MetropolisHamIntegral(in_pos,par); //muovo la posizione
         
         if (fin_pos.pos==in_pos.pos)  Rejected++;

         sum += hamiltonian(fin_pos);
         in_pos=fin_pos;
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;


   return {prog_sum,err_prog};
}

//uguale a running_averages, ma genera anche un file con tutti i valori di x paercorsi col Metropolis
pair<double*,double*> outfiles (int M, int N, parameters start_pos,double par) {
    
   double sum;
   double* med = new double [N];
   double* med2 = new double [N];
   
   ofstream out ("Histo_psi.txt");
   
   if ( (M%N) != 0) {
      cerr<<"Error in outfiles: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 

   int L = M/N;

   parameters in_pos = start_pos;
   parameters fin_pos;
   int Rejected = 0;

   //faccio convergere l'algoritmo

   for (int i=0; i<ConvTime; i++) {
      fin_pos = MetropolisHamIntegral(in_pos,par);
   }

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         
         fin_pos = MetropolisHamIntegral(in_pos,par);

         out<<fin_pos.pos<<endl;
         
         if (fin_pos.pos==in_pos.pos)  Rejected++;

         sum += hamiltonian(fin_pos);
         in_pos=fin_pos;
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   out.close();

   cout<<"Acceptance Rate: "<< 1. - (double)Rejected/(double)M<<endl;

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;


   return {prog_sum,err_prog};
}

//algoritmo per annealing
void SimulatedAnnealing (parameters start_pos,double T_start,ofstream& AnnRes) {

   //cout<<endl<<"----------------------STARTING SIMULATED ANNEALING--------------------------------"<<endl;

   int AnnL = AnnSteps/AnnBlocks;
   parameters curr_pos,test_pos;
   curr_pos = start_pos;
   double alpha;
   double beta;
   double par = 0.1;
   double sum[AnnL];
   double sum2[AnnL];
   double med,err;
   double ham_start,ham_test;

   for (int i=0; i<AnnBlocks; i++ ) {

      beta = 1/T_start + i; //cambio la temperatura

      for (int j=0; j<AnnL; j++) {

         //fissa mu e sigma e calcola il valore dell'integrale (variando con MC la x) con quei due valori fissati. Procedi iterativamente.

         test_pos.mu = curr_pos.mu + par*rnd.Rannyu(-1.,1.);
         test_pos.sigma = curr_pos.sigma + par*rnd.Rannyu(-1.,1.);
      
         auto ham_start_values = running_averages(M,N,curr_pos,0.5);
         ham_start = ham_start_values.first[N-1];
          
         auto ham_test_values = running_averages(M,N,test_pos,0.5);
         ham_test = ham_test_values.first[N-1];

         alpha = min(1.,exp(-beta*(ham_test-ham_start)));
         double acceptLimit = rnd.Rannyu();

         if (acceptLimit<=alpha) {
            curr_pos=test_pos;
            sum[j] = ham_test;
            sum2[j] += pow(ham_test,2);
            if (ham_test < best_energy) { //seleziono il valore minimo di H(x) ottenuto variando sigma e mu, lo userò alla fine per valutare E_0
               best_energy = ham_test;
               best_parameters = test_pos;
            }
         } else if (acceptLimit>alpha) {
            sum[j] = ham_start;
            sum2[j] = pow(ham_start,2);
         }
         
      }

      auto res = cumulative_calc(sum,sum2,AnnL);
      med = res.first[AnnL-1];
      err = res.second[AnnL-1];

      AnnRes<< med <<"  "<<err<<endl;

      cout<<endl<<"Executed block number "<<i+1<<"/"<<AnnBlocks<<endl;
      cout<<"Beta = "<<beta<<endl;
      cout<<"Block median values = "<<med<<" +- "<<err<<endl;
      cout<<"Best values sigma = "<<best_parameters.sigma<<" and mu = "<<best_parameters.mu<<endl;
      cout<<"Best evaluation = "<<best_energy<< endl;
      cout<<"-----------------------------------------------------------------------------------------"<<endl<<endl;
   }

   return;
}


 
int main (int argc, char *argv[]){

   rnd = InitializeRandom();

   parameters start;
   start.pos = 0.;
   start.mu = 1.;
   start.sigma = 0.5;

   //Simulated annealing
   ofstream AnnRes ("AnnRes.txt");
   SimulatedAnnealing(start,1.,AnnRes);
   AnnRes.close();

   //Select as a starting point the best result obtained
   start=best_parameters;

   auto resultsham = outfiles(M,N,start,0.5);
   double* valham = resultsham.first;
   double* errsham = resultsham.second;

   printout((char*)"Results.txt",valham,errsham,N);
   cout << "Final results: " << valham[N-1] << " +- " << errsham[N-1] << endl;

	rnd.SaveSeed();
   return 0;
}
