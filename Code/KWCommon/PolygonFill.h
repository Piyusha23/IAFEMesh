#ifndef PolygonFill_h
#define PolygonFill_h

#include <vector>

namespace PolygonFill
{
/* Describes a single point (used for a single vertex) */
class Point 
{
public:
  int X;   /* X coordinate */
  int Y;   /* Y coordinate */
  Point() : X(-99999),Y(-99999) {}
};

class PointListHeader 
{
public:
   int Length;                /* # of points */
   Point * PointPtr;   /* pointer to list of points */
  PointListHeader(int length) 
  {
    this->PointPtr = new Point[length]; Length = length; 
  }
  ~PointListHeader() { delete [] this->PointPtr; }
};

class LineDraw
{
public:
  virtual void Draw(int YToScan, int LeftX, int RightX) = 0;
  virtual ~LineDraw() {}
};
extern int Fill(PointListHeader *VertexList,
                       LineDraw *drawer);
}

#endif // PolygonFill_h
