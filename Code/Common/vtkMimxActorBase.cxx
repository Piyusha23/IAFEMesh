/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxActorBase.cxx,v $
Language:  C++
Date:      $Date: 2013/01/01 21:45:59 $
Version:   $Revision: 1.2 $

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

#include <sstream>

// VTK Headers
#include "vtkActor.h"
#include "vtkDataSet.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"

// MIMX Headers
#include "vtkMimxActorBase.h"

vtkCxxRevisionMacro(vtkMimxActorBase, "$Revision: 1.2 $");


//---------------------------------------------------------------------------
vtkMimxActorBase::vtkMimxActorBase()
{
  this->FileName.clear();
  this->FilePath.clear();
  this->UniqueId.clear();
  this->FoundationName.clear();
  this->DataType = 0;
  this->IsModified = 1;
}

//---------------------------------------------------------------------------
vtkMimxActorBase::~vtkMimxActorBase()
{
  this->FileName.clear();
  this->FilePath.clear();
  this->UniqueId.clear();
  this->FoundationName.clear();
}

//---------------------------------------------------------------------------
void vtkMimxActorBase::SetFilePath(const char *InputFilePath)
{
  this->FilePath = InputFilePath;
}

//---------------------------------------------------------------------------
void vtkMimxActorBase::SetUniqueId( const char *Id)
{
  this->UniqueId = Id;
}

//---------------------------------------------------------------------------
void vtkMimxActorBase::SetFoundationName(const char *created)
{
	this->FoundationName = created;
	this->FileName = created;
}

//---------------------------------------------------------------------------
void vtkMimxActorBase::SetObjectName(const char* newFileName, vtkIdType &Count)
{
	std::string tempbuffer;
	tempbuffer = this->FoundationName;
	tempbuffer += "_";
  tempbuffer += newFileName;
  
  std::stringstream ss;
  ss << Count;
  tempbuffer += ss.str();
  
  this->FileName = tempbuffer;
}
//---------------------------------------------------------------------------
const char* vtkMimxActorBase::GetFilePath()
{
  return this->FileName.c_str();
}

//---------------------------------------------------------------------------
const char* vtkMimxActorBase::GetFileName()
{
  return this->FileName.c_str();
}

//---------------------------------------------------------------------------
const char* vtkMimxActorBase::GetUniqueId()
{
  return this->UniqueId.c_str();
}

//---------------------------------------------------------------------------
const char* vtkMimxActorBase::GetFoundationName()
{
  return this->FoundationName.c_str();
}

//---------------------------------------------------------------------------
void vtkMimxActorBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


