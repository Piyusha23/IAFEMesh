#include "MyLineDraw.h"


void MyLineDraw::Draw(int YToScan, int LeftX, int RightX)
{
  ImageType::RegionType::SizeType size =
  this->m_Image->GetLargestPossibleRegion().GetSize();
  ImageType::IndexType index;
  index[this->m_YAxis] = YToScan;
  index[this->m_ZAxis] = m_SliceNum;
  for(int i = LeftX; i <= RightX; i++)
    {
    index[this->m_XAxis] = i;
#if 1
    for(int j = 0; j < 3; j++)
      {
      if((index[j] < 0 || index[j] >= static_cast<int>(size[j])))
        {
        std::cerr << "out of bounds "
        << index << " (" << size << ")" << std::endl;
        return;
        }
      }
#endif
    (*(this->m_Count))++;
    this->m_Image->SetPixel(index,m_FillValue);
    }
}
