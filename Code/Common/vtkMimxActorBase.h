/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxActorBase.h,v $
Language:  C++
Date:      $Date: 2013/01/02 17:45:57 $
Version:   $Revision: 1.3 $

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
// .NAME vtkMimxActorBase - Base class for all the actor representations
// .SECTION Description
// vtkMimxActorBase is the abstract base class for all the pipeline setup for
// different types of actor visualization. Data types considered are vtkPolyData,
// vtkStructuredGrid (both plane and solid) and vtkUnstructuredGrid.
// See mimxCommonDefine.h for all the declarations.

#ifndef __vtkMimxActorBase_h
#define __vtkMimxActorBase_h

#include <list>
#include <string>

//VTK Headers
#include "vtkProp3D.h"

//MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkMimxCommonWin32Header.h"

class vtkActor;
class vtkDataSet;
class vtkPolyData;

class VTK_MIMXCOMMON_EXPORT vtkMimxActorBase : public vtkProp3D
{
public:
 // static vtkActorBase *New();
  vtkTypeRevisionMacro(vtkMimxActorBase,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get and set the data types
  vtkGetMacro(DataType, vtkIdType);
  vtkSetMacro(DataType, vtkIdType);
  
  // Description:
  // Get and set the if the dataset has been modified
  vtkSetMacro(IsModified, vtkIdType);
  vtkGetMacro(IsModified, vtkIdType);
  
  vtkGetMacro(Actor, vtkActor*);
  virtual double *GetBounds() {return NULL;}
  
  // Description:
  // Get/Set the object name, file path, and basename
  const char *GetFilePath();
  const char *GetFileName();
  const char *GetUniqueId();
  const char *GetFoundationName();
  void SetFilePath(const char *InputFilePath);
  void SetUniqueId( const char *Id);
  void SetObjectName(const char *FilterName, vtkIdType &Count);
  void SetFoundationName(const char *FoundationName);
  
protected:
  vtkMimxActorBase();
  virtual ~vtkMimxActorBase();
  
  vtkActor *Actor;
  vtkIdType DataType;
  
  // Description:
  // To keep track of whether the data is created/modified in the program
  int IsModified;
  
  // Description:
  // File path for a dataset that was read in.
  std::string FilePath;
  
  // Description:
  // File name for a dataset that was read in or user given name for a newly
  // created object during program execution.
  std::string FileName;
  
  // Description:
  // ID for object identification. This is to address objects with same FileName.
  std::string UniqueId;
  
  // Description:
  // Name that will be associated with the objects that were either read in or created
  // and also objects that are derived from the current object.
  std::string FoundationName;
  
private:
  vtkMimxActorBase(const vtkMimxActorBase&);  // Not implemented.
  void operator=(const vtkMimxActorBase&);  // Not implemented.
};

#endif

