#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "point.h"

using namespace std;

Point :: Point() {
    m_x = 0.;
    m_y = 0.;
}

Point :: Point(double x, double y) {
    m_x = x;
    m_y = y;
}

Point :: ~Point(){}

void Point :: SetPoint(double x, double y) {
    m_x = x;
    m_y = y;

    return;
}

void Point :: SetX (double x) {
    m_x = x;

    return;
}

void Point :: SetY (double y) {
    m_y = y;

    return;
}

Point& Point::operator= (Point& start_point) {
    m_x = start_point.GetX();
    m_y = start_point.GetY();
    
    return *this;
}
