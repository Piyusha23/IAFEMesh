#include "PolygonFill.h"
#include "mimxCommonDefine.h"

class MyLineDraw : public PolygonFill::LineDraw
{
public:
	
  MyLineDraw(ImageType::Pointer &image,
             int xAxis, int yAxis, int zAxis,
             int sliceNum, ImagePixelType value, int *count) :
  m_Image(image),
  m_XAxis(xAxis),
  m_YAxis(yAxis),
  m_ZAxis(zAxis),
  m_SliceNum(sliceNum),
  m_FillValue(value),
  m_Count(count)
  {
  }
  
  virtual void Draw(int YToScan, int LeftX, int NumPixels);
  
private:
  ImageType::Pointer m_Image;
  int m_XAxis,m_YAxis,m_ZAxis;
  int m_SliceNum;
  ImagePixelType m_FillValue;
  int *m_Count;
};
