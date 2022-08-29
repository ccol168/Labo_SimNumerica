#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "random.h"
#include "path.h"


using namespace std;

Path :: Path(){
    m_N = 0;
    m_path = NULL;
    m_pairprob = 0.0;
    m_jumpprob = 0.0;
    m_invprob = 0.0;
    m_crossprob = 0.0;
    m_singjumpprob = 0.0;
}

Path :: Path(int N) {
    m_N = N;
    m_path = new int[N];
    m_pairprob = 0.1;
    m_jumpprob = 0.1;
    m_invprob = 0.1;
    m_singjumpprob = 0.15;
    m_crossprob = 0.8;
}

//non ho definito il distruttore poichè si comporta in modo strano con la multimap in Generation: comunque non dà problemi sotto le 20000 generazioni
Path :: ~Path(){
    //delete[] m_path; 
}

void Path::SetPath(int N) {
    m_N = N;
    m_path = new int[N];
    m_pairprob = 0.1;
    m_jumpprob = 0.1;
    m_invprob = 0.1;
    m_singjumpprob = 0.15;
    m_crossprob = 0.8;

    return;
}

void Path::BasePath() {
    for (int i=0; i<m_N; i++) {
        m_path[i] = i+1;
    }
    CheckPath();
    return;
}

void Path::ScrambledPath(Random& rnd) {
    BasePath();
    for (int i=2; i<=m_N;i++) {
        Swap(i,rnd.RandInt(2,m_N));
    }
    CheckPath();
    return;
}

bool Path::GoodPath() {
    if (m_path [0] != 1) return false;
    else {
        int good_numb=0;
        bool flag;
        for (int i=0; i<m_N; i++) {
            flag = false;
            for (int j=0; j<m_N;j++) {
                if (m_path[j]==i+1) {
                    flag = true;
                    good_numb++;
                    break;
                }
            }
            if (flag == false) return false;
        }

        if (good_numb != m_N) return false;
        

        return true;//lunghezza del tratto su cui fare crossing-over
    } 
}

void Path::Print() {
    for (int i=0;i<m_N;i++) {
        cout<<m_path[i]<<"  ";
    }
    cout<<endl;
    return;
}

void Path::PrintOut(ofstream& out) {
    for (int i=0;i<m_N;i++) {
        out<<m_path[i]<<"  ";
    }
    out<<endl;
    return;
}

Path& Path::operator=(Path& start_path) {
    m_N = start_path.GetN();
    m_pairprob = start_path.GetPairProb();
    m_jumpprob = start_path.GetJumpProb();
    m_invprob = start_path.GetInvProb();
    m_crossprob= start_path.GetCrossProb();

    m_path = new int[m_N];

    for (int i=0; i<m_N; i++) m_path[i] = start_path.GetElement(i+1);

    return *this;
}

void Path::CopyPath(Path& start_path) {
    m_N = start_path.GetN();
    m_pairprob = start_path.GetPairProb();
    m_jumpprob = start_path.GetJumpProb();
    m_invprob = start_path.GetInvProb();
    m_crossprob= start_path.GetCrossProb();

    m_path = new int[m_N];

    for (int i=0; i<m_N; i++) m_path[i] = start_path.GetElement(i+1);
    CheckPath();
    return;

}

void Path::Swap(int i) {
   Swap(i,i+1);
   return;
}

void Path::Swap(int i,int j) {
    if (i<=m_N && j <= m_N && i!=1 && j!=1) {
        i--;
        j--;
        int park = m_path[i];
        m_path[i] = m_path[j];
        m_path[j] = park; 
        return;
    } else if (i==1 && j!=1){ 
        int park = m_path[1];
        m_path[1] = m_path[j];
        m_path[j] = park; 
    } else if (j==1 && i!=1){ 
        int park = m_path[1];
        m_path[1] = m_path[i];
        m_path[i] = park; 
    } else return;
}

void Path::PairMutation(Random& rnd) {
    double start = rnd.Rannyu();
    if (start>m_pairprob) return;
    else {
        Swap(rnd.RandInt(1,m_N));
        CheckPath();
    }
}

void Path::JumpMutation(Random& rnd) {
    double start = rnd.Rannyu();
    if (start>m_jumpprob) return;
    else {
        int partjumps = rnd.RandInt(2,6);
        int jump = rnd.RandInt(partjumps,m_N-partjumps); 
        int start_swap = rnd.RandInt(1,m_N);

        for (int i=0;i<3;i++) {
            Swap(Pbc(start_swap+i),Pbc(start_swap+jump+i));
        }
        CheckPath();
        return;
    }
}

void Path::SingleJumpMutation(Random& rnd) {
    double start = rnd.Rannyu();
    if (start>m_singjumpprob) return;
    else {
        int jump;
        if (m_N>20) jump = rnd.RandInt(3,m_N-10); 
        else if (m_N>10 && m_N<20) jump = rnd.RandInt(3,m_N-5); 
        else return;
        int start_swap = rnd.RandInt(1,m_N);

        Swap(Pbc(start_swap),Pbc(start_swap+jump));
    
        CheckPath();
        return;
    }

}

void Path::InversionMutation(Random& rnd) {
    double start = rnd.Rannyu();
    if (start>m_invprob) return;
    else {
        int lenght_inv = rnd.RandInt(2,m_N/2);
        int start_inv = rnd.RandInt(1,m_N);
        for (int i=0; i<(int)lenght_inv/2;i++) {
            Swap(Pbc(start_inv+i),Pbc(start_inv+lenght_inv-i));
        }
        CheckPath();
        return;
    }
}

void Path::SetProbabilities(double a, double b, double c, double d,double e) {
    m_pairprob=a;
    m_jumpprob=b;
    m_invprob=c;
    m_crossprob=d;
    m_singjumpprob=e;
    return;
}

void Path::SetSingleProb(double val, int pos) {
    if (pos==1) m_pairprob=val;
    else if (pos==2) m_jumpprob=val;
    else if (pos==3) m_invprob=val;
    else if (pos==4) m_crossprob=val;
    else if (pos==5) m_singjumpprob=val;
    else cerr<<"ERROR in SetSingleProb: set probability is not valid" <<endl;
    return;
}

void Path::CrossingOver(Random& rnd, Path& parent) {
    if (m_N != parent.GetN()) {
        cerr<<"Error in CrossingOver: impossible to cross over paths with different lenghts"<<endl;
        exit(1);
    }
    double start = rnd.Rannyu();
    if (start>m_crossprob) return;
    else { 
        int lenght_cross;
        //lunghezza del tratto su cui fare crossing-over
        if (m_N-5<5) lenght_cross = rnd.RandInt(1,m_N-1); 
        else lenght_cross = rnd.RandInt(3,m_N-5);
        //punto di partenza
        int start_cross = rnd.RandInt(2,m_N-lenght_cross+1); 
        //Set all the crossed-over part to zero
        for (int i=start_cross;i<start_cross+lenght_cross;i++) {
            m_path[i-1] = 0;
        }
        bool found;
        int NumbSet = 0;

        //Replace the crossed-over part with parent's elements, in the order in which they appear on the parent
        for (int i=0;i<parent.GetN();i++) {
            found = false;
            for (int j=0; j<m_N;j++) {
                if (parent.GetElement(i+1) == m_path[j]) {
                    found = true;
                    break;
                }
            } 
            if (!found) {
                m_path[start_cross+NumbSet-1]=parent.GetElement(i+1);
                NumbSet++;
            }
            
        }
        if (NumbSet != lenght_cross) cerr<<"Error in CrossingOver: NumbSet and lenght_cross are two different numbers"<<endl;
        CheckPath();
        return;
    }
}

void Path::GenerateSon(Random& rnd, Path& father,  Path& mother) {
    if (mother.GetN() != father.GetN()) {
        cerr<<"Error in GenerateSon: impossible to generate son with parents of different lenght"<<endl;
        return;
    }
    CopyPath(mother);
    CrossingOver(rnd,father);
    
    Mutate(rnd);
    return;
}

void Path::Mutate(Random& rnd){
    
    PairMutation(rnd);
    
    JumpMutation(rnd);
    
    InversionMutation(rnd);

    SingleJumpMutation(rnd);

    return;
}

void Path::CheckPath() {
    if(!GoodPath()) {
        cerr<<"Error in Path: generated an unacceptable Path"<<endl;
        Print();
        exit (1); 
    } else return;
}

int Path::Pbc (int n) {
    if (n>m_N) return n-m_N;
    else return n;
}

