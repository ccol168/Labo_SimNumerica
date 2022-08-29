#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "random.h"
#include "path.h"
#include "generation.h"
#include <map>
#include <iterator>

using namespace std;

double distance (Point start, Point stop) {
    double deltax = start.GetX()-stop.GetX();
    double deltay = start.GetY()-stop.GetY();

    return sqrt(pow(deltax,2)+pow(deltay,2));
}

Generation :: Generation() {
    m_chromosomes = NULL;
    m_NChromosomes = 0;
    m_stops = NULL;
    m_NStops = 0;
    m_ordered=false;
    m_NumGen = 0;
}

Generation :: ~Generation() {}

void Generation :: GenerateCircularStops(Random& rnd,int N) {
    m_NStops = N;
    m_stops = new Point[m_NStops];
    double theta;
    for (int i=0; i<m_NStops; i++) {
        theta = rnd.Rannyu(0,2*M_PI);
        m_stops[i].SetPoint(cos(theta),sin(theta));
    }
    return;
}

void Generation :: GenerateRandomStops(Random& rnd,int N) {
    m_NStops = N;
    m_stops = new Point[m_NStops];
    for (int i=0; i<m_NStops; i++) {
        m_stops[i].SetPoint(rnd.Rannyu(),rnd.Rannyu());
    }
    return;
}

void Generation :: AppendStop(double x, double y) {
    int OldStops = m_NStops;
    m_NStops++;
    Point* park = m_stops;
    m_stops = new Point[m_NStops];
    for (int i=0;i<OldStops;i++) {
        m_stops[i]=park[i];
    }
    m_stops[OldStops].SetPoint(x,y);

    return;
}

void Generation::StartingChromosomes(Random& rnd,int N) {

    if (m_NStops==0 || m_stops==NULL) {
        cerr<<"Error in StartingChromosomes: no stops set"<<endl;
        exit(1);
    } else {
        m_NumGen = 1;
        m_NChromosomes = N;
        m_chromosomes = new Path [m_NChromosomes];

        for (int i=0;i<m_NChromosomes;i++) {
            m_chromosomes[i].SetPath(m_NStops);
            m_chromosomes[i].ScrambledPath(rnd);
        }
        return;
    }
}

void Generation::PrintStops () {
    if (m_stops == NULL || m_NStops==0) {
        cerr << "Error in PrintStops: no stops set" << endl;
        exit(1);
    } else {
        for (int i=0;i<m_NStops;i++) {
            cout<<m_stops[i].GetX()<<"  "<<m_stops[i].GetY()<<endl;
        }
        return;
    }
}

void Generation::PrintOutStops (ofstream& out) {
    if (m_stops == NULL || m_NStops==0) {
        cerr << "Error in PrintStops: no stops set" << endl;
        exit(1);
    } else {
        for (int i=0;i<m_NStops;i++) {
            out<<m_stops[i].GetX()<<"  "<<m_stops[i].GetY()<<endl;
        }
        return;
    }
}

void Generation::PrintChromosomes () {
    if (m_NChromosomes==0 || m_chromosomes==NULL) {
        cerr<<"Error in PrintChromosomes: no chromosomes set" <<endl;
        exit(1);
    } else {
        for (int i=0;i<m_NChromosomes;i++) {
            m_chromosomes[i].Print();
        }
        return;
    }
}

double Generation::CalculateChromosomeFitness(int n) {
    if (n<=0 || n>m_NChromosomes) {
        cerr<<"Error in CalculateChromosomeFitness: invalid chosen chromosome"<<endl;
        return 0;
    } else {
        double sum=0;
        int PosStart;
        int PosStop;
        for (int i=1; i<m_NStops; i++) {
            PosStart = m_chromosomes[n-1].GetElement(i);
            PosStop = m_chromosomes[n-1].GetElement(i+1); 
            sum += distance(m_stops[PosStart-1],m_stops[PosStop-1]);
        }
        PosStart = m_chromosomes[n-1].GetElement(m_NStops);
        PosStop = m_chromosomes[n-1].GetElement(1);
        sum += distance(m_stops[PosStart-1],m_stops[PosStop-1]);

        return sum;
    }
}

void Generation::OrderChromosomes() {
    if (m_NChromosomes==0 || m_chromosomes==NULL) {
        cerr<<"Error in OrderChromosomes: no chromosomes set" <<endl;
        return;
    } else if (!m_ordered) {
        for (int i=0; i<m_NChromosomes;i++) {
            m_order.insert(pair<double,Path>(CalculateChromosomeFitness(i+1),m_chromosomes[i]));
        }
        m_ordered=true;
        return;
    } else if (m_ordered) {
        return;
    }
}

void Generation::PrintOrder() {
    if (!m_ordered) OrderChromosomes(); 
    multimap<double,Path>::iterator itr;
    for (itr=m_order.begin(); itr != m_order.end(); ++itr ) {
        cout<<itr->first<<"       ";
        itr->second.Print();
    }
    return;
}

void Generation::PrintOutOrder(ofstream& out) {
    if (!m_ordered) OrderChromosomes(); 
    multimap<double,Path>::iterator itr;
    for (itr=m_order.begin(); itr != m_order.end(); ++itr ) {
        out<<itr->first<<"       ";
        itr->second.PrintOut(out);
    }
    return;
}

double Generation::BestFitness() {
    if (!m_ordered) OrderChromosomes();

    return m_order.begin()->first;

}

double Generation::MeanBestFitnesses(int n) {
    if(!m_ordered) OrderChromosomes();
    if (n>m_NChromosomes) n=m_NChromosomes;
    double sum = 0.;
    multimap<double,Path>::iterator itr;
    itr = m_order.begin();
    for (int i=0;i<n;i++) {
        sum += itr->first;
        itr++;
    }
    return sum/(double)n;
}

double Generation::MeanBestFitnesses() {
    int n = m_NChromosomes/2;
    return MeanBestFitnesses(n);
}

void Generation::PrintBestChromosomes(int n) {
    if (!m_ordered) OrderChromosomes();
    if (n>m_NChromosomes) n=m_NChromosomes;
    multimap<double,Path>::iterator itr;
    itr = m_order.begin();
    for (int i=0;i<n;i++) {
        cout<<itr->first<<"       ";
        itr->second.Print();
        itr++;
    }
    return;
}

void Generation::PrintOutBestChromosomes(ofstream& out,int n) {
    if (!m_ordered) OrderChromosomes();
    if (n>m_NChromosomes) n=m_NChromosomes;
    multimap<double,Path>::iterator itr;
    itr = m_order.begin();
    for (int i=0;i<n;i++) {
        out<<itr->first<<"       ";
        itr->second.PrintOut(out);
        itr++;
    }
    return;
}

void Generation::PrintBestChromosomes() {
    int n = m_NChromosomes/2;
    PrintBestChromosomes(n);
    return;
}

void Generation::PrintOutBestChromosomes(ofstream& out) {
    int n = m_NChromosomes/2;
    PrintOutBestChromosomes(out,n);
    return;
}

Path& Generation::GetOrderedChromosome(int n) {
    if (!m_ordered) OrderChromosomes();
    multimap<double,Path>::iterator itr;

    itr = m_order.begin();
    int count=1;
    while (count<n) {
        count++;
        itr++;
    }
    Path& result = itr->second;
    return result;

}

void Generation::NewGeneration(Random& rnd, Generation& old_gen) {

    if (!old_gen.IsOrdered()) old_gen.OrderChromosomes();

    m_ordered = false;
    m_order.clear();
    
    double NewGen_Exponent=10;
    m_NumGen = old_gen.GetNumGen() + 1;

    m_NStops = old_gen.GetNStops();
    m_stops = new Point[m_NStops];

    for (int i=0;i<m_NStops;i++) {
        m_stops[i]=old_gen.GetStop(i+1);
    }

    m_NChromosomes = old_gen.GetNChromosomes();
    m_chromosomes = new Path[m_NChromosomes];
    int ChosenFatherPlace;
    int ChosenMotherPlace;
    for (int i=0; i<m_NChromosomes; i++) {
        ChosenFatherPlace = int(m_NChromosomes*pow(rnd.Rannyu(),NewGen_Exponent))+1;
        ChosenMotherPlace = int(m_NChromosomes*pow(rnd.Rannyu(),NewGen_Exponent))+1;

        m_chromosomes[i].GenerateSon(rnd,old_gen.GetOrderedChromosome(ChosenFatherPlace),old_gen.GetOrderedChromosome(ChosenMotherPlace));
        
    }
    OrderChromosomes();
    return;
} 

void Generation::NewGeneration(Random& rnd, Generation& old_gen, double NewGen_Exponent) {

    if (!old_gen.IsOrdered()) old_gen.OrderChromosomes();

    m_ordered = false;
    m_order.clear();
    
    m_NumGen = old_gen.GetNumGen() + 1;

    m_NStops = old_gen.GetNStops();
    m_stops = new Point[m_NStops];

    for (int i=0;i<m_NStops;i++) {
        m_stops[i]=old_gen.GetStop(i+1);
    }

    m_NChromosomes = old_gen.GetNChromosomes();
    m_chromosomes = new Path[m_NChromosomes];
    int ChosenFatherPlace;
    int ChosenMotherPlace;
    for (int i=0; i<m_NChromosomes; i++) {
        ChosenFatherPlace = int(m_NChromosomes*pow(rnd.Rannyu(),NewGen_Exponent))+1;
        ChosenMotherPlace = int(m_NChromosomes*pow(rnd.Rannyu(),NewGen_Exponent))+1;

        m_chromosomes[i].GenerateSon(rnd,old_gen.GetOrderedChromosome(ChosenFatherPlace),old_gen.GetOrderedChromosome(ChosenMotherPlace));
        
    }
    OrderChromosomes();
    return;
} 

void Generation::CopyGen(Generation& old_gen) {
    m_NumGen = old_gen.GetNumGen();
    m_NStops = old_gen.GetNStops();
    delete[] m_stops;
    m_stops = new Point[m_NStops];
    m_ordered = false;
    m_order.clear();

    for (int i=0;i<m_NStops;i++) {
        m_stops[i]=old_gen.GetStop(i+1);
    }

    m_NChromosomes = old_gen.GetNChromosomes();
    delete[] m_chromosomes;
    m_chromosomes = new Path[m_NChromosomes];

    for (int i=0;i<m_NChromosomes;i++) {
        m_chromosomes[i].CopyPath(old_gen.GetChromosome(i+1));
    }
    OrderChromosomes();

    return;
}