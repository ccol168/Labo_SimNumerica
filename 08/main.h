struct parameters {
   double pos;
   double mu;
   double sigma;
};

Random rnd;
//global variables for annealing
int AnnSteps = 100000; //steps per blocco
int AnnBlocks = 100; //# blocchi
int M = 10000; //# monte-carlo steps fatti ogni volta che valuto un integrale di H(x)
int N = 100; //blocchi in cui divido i monte-carlo steps fatti ogni volta
int ConvTime = 10000; //blocchi fatti "a vuoto" per far convergere l'integrale

//store dei parametri per la simulazione con H(x) più basso
parameters best_parameters;
double best_energy = 1000; 