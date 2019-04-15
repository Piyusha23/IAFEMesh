/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadImageGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/16 02:17:17 $
Version:   $Revision: 1.4 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkKWMimxLoadIMageGroup - Load an image from disk
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It will load a 
// image from disk

#ifndef __vtkKWMimxLoadImageGroup_h
#define __vtkKWMimxLoadImageGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWComboBoxWithLabel;
class vtkKWFileBrowserDialog;
class vtkKWLoadSaveDialog;
class vtkEntryChangedEventCallback;
class vtkKWFrameWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxLoadImageGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxLoadImageGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxLoadImageGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  // Description:
  // General public interface to load an image
  // This class uses ITK for image I/O. The file is tested
  // to see if it is a DICOM file. If it is a DICOM file
  // the DICOM series reader is used to load the data instead
  // of the ITK image factory.
  void LoadImage(const char *FilePath);
  
protected:
  vtkKWMimxLoadImageGroup();
  ~vtkKWMimxLoadImageGroup();
  virtual void CreateWidget();

  // Description:
  // Internal interface to load a DICOM series
  void LoadDicomSeries(const char *filename);
  
  // Description:
  // Internal interface to load an ITK image
  void LoadItkImage(const char *filename);
  
private:
  vtkKWMimxLoadImageGroup(const vtkKWMimxLoadImageGroup&); // Not implemented
  void operator=(const vtkKWMimxLoadImageGroup&); // Not implemented

};

#endif


