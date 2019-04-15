/*=========================================================================

  Module:    $RCSfile: vtkLinkedListWrapper.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkLinkedListWrapper - container class for different actors
// .SECTION Description
// It was necessitated to write this class because kwwidgets do not allow
// templated classes. It is a linked list taking in vtkMimxActorBase class
// datatypes.
//
// .SECTION See Also
// vtkAbstractIterator, vtkAbstractList, vtkAbstractMap

#include "vtkObject.h"

#include "vtkMimxActorBase.h"
#include "vtkSetGet.h" // For vtkTypeMacro.

#include "vtkLinkedList.txx"

#ifndef __vtkLinkedListWrapper_h
#define __vtkLinkedListWrapper_h

//MIMX Headers
#include "vtkMimxCommonWin32Header.h"


class VTK_MIMXCOMMON_EXPORT vtkLinkedListWrapper : public vtkObject
{
public:
	static vtkLinkedListWrapper *New();
  vtkTypeMacro(vtkLinkedListWrapper, vtkObject);
  
  // Description:
  //Linked List of vtkMimxActorBase
  vtkLinkedList<vtkMimxActorBase*> *List;
  
  // Description:
  // Add an item to the linked list
  int AppendItem(vtkMimxActorBase*);
  
  // Description:
  // Get the nth item from the list
  vtkMimxActorBase* GetItem(vtkIdType);
  
  // Description:
  // Get the nth item from the list
  vtkMimxActorBase* GetItem(const char *Name);
  
  // Description:
  // Get total number of items in the list
  int GetNumberOfItems();
  
  // Description:
  // Remove the item in the specified position
  int RemoveItem(int );
  
  // Description:
  // Get the item number from the list
  int GetItemNumber(const char *Name);
  
protected:
  vtkLinkedListWrapper();
  virtual ~vtkLinkedListWrapper();
  
private:
  vtkLinkedListWrapper(const vtkLinkedListWrapper&); // Not implemented
  void operator=(const vtkLinkedListWrapper&); // Not implemented
};
#endif 



