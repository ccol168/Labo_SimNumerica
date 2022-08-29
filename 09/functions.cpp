#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "functions.h"
#include "random.h"


using namespace std;

double error (double x, double y, int n) {
	if (n==0) {
		return 0;
	} else {
		return sqrt((y-pow(x,2))/n);
	}
}

void print (double* vett, int N) {
   for (int i = 0; i < N; i++)
   {
      cout << vett[i] << endl;
   }
   return;
}

void print (double* vett_1, double* vett_2, int N) {
   for (int i = 0; i < N; i++)
   {
      cout << vett_1[i] << "  "<<vett_2[i]<< endl;
   }
   return;
}

void print (double* vett, double* vett_2,double* vett_3, int N) {

   for (int i = 0; i < N; i++)
   {
      cout << vett[i]<<"  "<<vett_2[i]<<"  "<<vett_3[i]<< endl;
   }
   return;
}


void print (double* vett, double* vett_2,double* vett_3,double* vett_4, int N) {

   for (int i = 0; i < N; i++)
   {
      cout << vett[i]<<"  "<<vett_2[i]<<"  "<<vett_3[i]<<"  "<<vett_4[i] << endl;
   }
   return;
}

void print (double* vett, double* vett_2,double* vett_3,double* vett_4, double* vett_5, int N) {

   for (int i = 0; i < N; i++)
   {
      cout << vett[i]<<"  "<<vett_2[i]<<"  "<<vett_3[i]<<"  "<<vett_4[i]<< "  "<< vett_5[i] << endl;
   }
   return;
}

double chi_squared (double* rand_numbers, int N, int L){

   double chi = 0;
   int counter;
   double mean = L/N;
   double limmin;
   double limmax;
    
   for (int i=0; i<N; i++) {
      counter = 0;
      limmin = (double)i/(double)N;
      limmax = (double)(i+1)/(double)N;
      for (int j=0; j<L; j++) {
         if (rand_numbers[j]>limmin && rand_numbers[j]<limmax) {
            counter++;
            //cout <<"SHOT  "<< limmin << "  " << rand_numbers[j] <<"  " << limmax << endl;
         } 
      }

      chi += pow(counter-mean,2)/mean;
      //cout << chi << "  " << counter << "  " << endl;
   }

   return chi;
}

void printout (char* filename,double* vett1, double* vett2, int N) {
    ofstream out (filename);

    for (int i=0;i<N;i++) {
        out<<vett1[i]<<"  "<<vett2[i]<<endl;
    }
    cout<<"File "<<filename<<" created and arrays printed"<<endl<<endl;;

    out.close();

}

void printout (char* filename,double* vett1, double* vett2, double* vett3, int N) {
    ofstream out (filename);

    for (int i=0;i<N;i++) {
        out<<vett1[i]<<"  "<<vett2[i]<<"  "<<vett3[i]<<endl;
    }
    cout<<"File "<<filename<<" created and arrays printed"<<endl<<endl;

    out.close();

}

void printout (char* filename,double* vett1, double* vett2, double* vett3, double* vett4, int N) {
    ofstream out (filename);

    for (int i=0;i<N;i++) {
        out<<vett1[i]<<"  "<<vett2[i]<<"  "<<vett3[i]<<"  "<< vett4[i]<<endl;
    }
    cout<<"File "<<filename<<" created and arrays printed"<<endl<<endl;

    out.close();

}

void printout (char* filename,double* vett1, double* vett2, double* vett3, double* vett4, double* vett5, int N) {
    ofstream out (filename);

    for (int i=0;i<N;i++) {
        out<<vett1[i]<<"  "<<vett2[i]<<"  "<<vett3[i]<<"  "<< vett4[i]<<"  "<<vett5[i]<<endl;
    }
    cout<<"File "<<filename<<" created and arrays printed"<<endl<<endl;

    out.close();

}

void printout (char* filename,double* vett1, double* vett2, double* vett3, double* vett4, double* vett5, double* vett6, double* vett7, double* vett8, int N) {
   ofstream out (filename);

   for (int i=0;i<N;i++) {
      out<<vett1[i]<<"  "<<vett2[i]<<"  "<<vett3[i]<<"  "<< vett4[i]<<"  "<<vett5[i]<<"  "<<vett6[i]<<"  "<<vett7[i]<<"  "<<vett8[i]<<endl;
   }
   cout<<"File "<<filename<<" created and arrays printed"<<endl<<endl;

   out.close();

}

Random InitializeRandom(){
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

   return rnd;
}





