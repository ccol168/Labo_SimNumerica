#ifndef __Path__
#define __Path__

#include "point.h"
#include "random.h"

class Path {

private:
  int m_N; //lunghezza
  int* m_path; //contenuto

  //mutation probabilities
  double m_pairprob;
  double m_jumpprob;
  double m_invprob;
  double m_crossprob;
  double m_singjumpprob;

protected:

public:
  // constructors
  Path(); //path vuoto
  Path(int); //path lungo N e prob di mutazione di default
  Path(int,int*); //path lungo N e m_path predefinito
  // destructor 
  ~Path();
  // methods
  int Pbc(int); //applica periodic boundary conditions
  int GetN() {return m_N;} //ottiene la lunghezza del path
  int GetElement (int i) {return m_path[i-1];} //ottiene l'elemento i della posizione
  int* GetPath() {return m_path;}
  // ritorna le varie prob di mutazione
  double GetPairProb() {return m_pairprob;}
  double GetJumpProb() {return m_jumpprob;}
  double GetInvProb() {return m_invprob;}
  double GetCrossProb()  {return m_crossprob;}
  void SetProbabilities(double,double,double,double,double); //imposto le 5 prob di mutazione
  void SetSingleProb(double,int); //imposto la prob di mutazione numero i
  void SetPath(int); //genero un path vuoto lungo i 
  void Print(); // stampo tutte le posizioni del path
  void PrintOut(std::ofstream& out); // stampo sul file out le posizioni del path


  void BasePath(); //genero il path [1,2,...,N-1,N]
  void ScrambledPath(Random&); //genero un path casuale
  bool GoodPath(); //ritorna True se il path è valido 
  void CheckPath(); //controlla se il path è valido
  void Swap(int); // scambio il contenuto una posizione i con la successiva
  void Swap(int,int); // cambio una posizione i con una j
  
  // due modi equivalenti per copiare un path
  Path& operator= (Path&);
  void CopyPath(Path&); 

  //mutations and crossing-over
  void Mutate(Random&); //applica in serie tutte le 4 mutazioni
  void PairMutation(Random&); // mutazione #1
  void JumpMutation(Random&); // mutazione #2
  void SingleJumpMutation(Random&); // mutazione #3
  void InversionMutation(Random&); // mutazione #4
  void CrossingOver(Random&,Path&); // applica il crossing over con un altro path
  void GenerateSon(Random&,Path&,Path&); // genera un "figlio" da due "genitori"
};

#endif // __Path__