#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <math.h>
#include "functions.h"

using namespace std;

// ho usato il metodo accept-reject per fare un sample della funzione y=1/sqrt(1-x**2) (la cui cumulativa è y=cos(x))
// sono stati applicati sia un cut sulle y (cutY) e di conseguenza è stato determinato un cut sulle x. Questo processo è necessario in quanto la funzione diverge in x = 1 e x = -1.
// un sample di questo tipo non è computazionalmente efficiente, ma, dati i buoni risultati ottenuti (vedi notebook) e il tempo ridotto di calcolo (circa 10 secondi) ho ritenuto 
// accettabile un algoritmo di questo tipo in modo da evitare di usare pigreco.
double AcceptReject (Random& rnd) {
   double x;
   double y;
   double cutY = 400.;

   for(;;) {
      x=rnd.Rannyu(-1.,1.);
      if ( x < -(sqrt((pow(cutY,2)-1)/pow(cutY,2))) || x > (sqrt((pow(cutY,2)-1)/pow(cutY,2))) )  { //fissa il cut sulle x a partire da cutY
         return x;
      }
      y=rnd.Rannyu(0.,cutY);
      if (y <= 1./(sqrt(1-pow(x,2)))) {
         return x;
      } 
   }
}


//prende i blocchi separati e restituisce la media e gli errori cumulativi
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


pair<double*,double*> running_averages_fast (Random& rnd,double subtract, double exponent, int M, int N) {
    
   double sum;
   double* med = new double [N];
   double* med2 = new double [N];
   
   if ( (M%N) != 0) {
      cerr<<"Error in running_averages: number of blocks non divisible per total number of data"<<endl;
      exit(1);
   } 

   //genera i singoli blocchi, contenenti ognuno una media e un errore associato alla media

   int L = M/N;

   for (int i=0; i<N; i++){
      sum = 0;
      for (int j=0; j<L; j++) {
         sum += pow(rnd.Rannyu()-subtract,exponent); 
      }  
      med[i] = sum/L; //media dei blocchi singoli
      med2[i] = pow(med[i],2); //media quadrata dei blocchi singoli
   }

   // ottiene i valori cumulativi e li restituisce

   auto res = cumulative_calc(med,med2,N);
   double* prog_sum = res.first;
   double* err_prog = res.second;

   return {prog_sum,err_prog};
}

// metodo di Buffon per il calcolo di pigreco 
// parametrizzo l'ago con i due valori y1 e y2 corrispondenti ai due estremi dell'ago
pair <double*,double*> Buffon (int N,int L, Random& rnd) {
   
   double y1, y2;
   int counter;
   double need_len = 0.2;
   double d = 0.25;
   double* Buff_estimations = new double [L];
   double* Buff_estimations2 = new double [L];

   for (int k=0; k<L; k++) {
      counter = 0;
      for (int i=0; i<N; i++) {
         y1 = rnd.Rannyu();
         y2 = y1 + need_len*AcceptReject(rnd); //dato il primo estremo dell'ago, determino il secondo tramite l'angolo phi. AcceptReject(rnd) restituisce cos(phi)
         for (int j=0; j<5; j++) {
            if ((y1<j*d && y2 > j*d) || (y1>j*d && y2<j*d)) {
               counter++;
               break;
            }
         }
      }
      // riempio i vettori contenenti rispettivamente il valor medio dato dalla formula di Buffon e l'errore associato
      Buff_estimations[k] = (2*need_len*(double)N)/((double)counter*d); 
      Buff_estimations2[k] = pow(Buff_estimations[k],2);
   }

   auto res = cumulative_calc(Buff_estimations,Buff_estimations2,L);
   double* prog_res = res.first;
   double* prog_err = res.second;

   return {prog_res,prog_err};
}

// somma N lanci di dadi uniformi, esponenziali o gaussiani
double sum (Random& rnd,int N, string DiceType ) {
   if (DiceType != "Flat" && DiceType != "Exponential" && DiceType != "Lorentzian") {
      cerr<<"Error in sum: no valid dice specified." << endl;
      cerr<<"Dice types admitted: Flat,Exponential,Lorentzian"<<endl;
      exit(1);
   } else {
      double sum=0;
      for (int i=0; i<N; i++) {
         if(DiceType=="Flat") {
            sum += rnd.Rannyu();
         } else if (DiceType=="Exponential"){
            sum += rnd.Expo(1.);
         } else if (DiceType=="Lorentzian"){
            sum += rnd.Cauchy(0.,1.);
         }
      }
      return sum;
   }

}
 
int main (int argc, char *argv[]){

   // InitializeRandom è in functions.cpp
   Random rnd = InitializeRandom();

   int M = 100000;
   int N = 100; //how many running blocks
   int L = M/N;

   // calcolo valor medio ed errore associato
   auto results1 = running_averages_fast(rnd,0.,1.,M,N);
   double* prog_sum = results1.first;
   double* err_prog = results1.second;

   // calcolo dev std ed errore associato
   auto results2 = running_averages_fast(rnd,0.5,2.,M,N);
   double* dev_prog = results2.first;
   double* err_dev_prog = results2.second;

   double chi_total[N];
   double rand_slice[L];

   // calcolo il chi quadro (la funzione chi_squared si trova in functions.cpp)
   for (int i=0; i<N; i++) {
      for (int j=0; j<L; j++) {
         rand_slice[j] = rnd.Rannyu();
      }
      chi_total[i] = chi_squared(rand_slice,N,L);
   }
   
   // stampo i risultati (le funzioni printout sono in functions.cpp)
   printout((char*)"Data.txt",prog_sum,err_prog,dev_prog,err_dev_prog,chi_total,N);

   // metodo di Buffon

   int Buff_blocks = 100;

   auto Buff_res = Buffon(10000,Buff_blocks,rnd);
   double* pi_estimations = Buff_res.first;
   double* pi_errors = Buff_res.second;

   printout((char*)"Buffon_results.txt",pi_estimations,pi_errors,Buff_blocks);

   // Lancio di dadi uniformi, gaussiani e Lorentziani

   int LL = 100;
   int NN = 10000;
   double FlatSum[NN];
   double ExpoSum[NN];
   double LorSum[NN];

   for (int i=0;i<NN;i++ ) {
      FlatSum[i]=sum(rnd,LL,"Flat");
      ExpoSum[i]=sum(rnd,LL,"Exponential");
      LorSum[i]=sum(rnd,LL,"Lorentzian");
   }

   printout((char*)"Histograms.txt",FlatSum,ExpoSum,LorSum,NN);
   
	
   rnd.SaveSeed();

   return 0;
}
