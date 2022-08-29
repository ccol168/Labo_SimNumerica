#ifndef __Point__
#define __Point__

// rappresenta uno stop, con classi utili
class Point {

private:
  double m_x;
  double m_y;

protected:

public:
  // constructors
  Point();
  Point(double, double);
  // destructor 
  ~Point();
  Point& operator=(Point&);
  // methods (imposto i parametri o li ottengo)
  void SetPoint (double, double);
  void SetX (double);
  void SetY (double);
  double GetX () {return m_x;}
  double GetY () {return m_y;}
};

#endif 