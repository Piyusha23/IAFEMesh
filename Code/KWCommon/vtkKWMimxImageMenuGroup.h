/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageMenuGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/15 03:12:51 $
Version:   $Revision: 1.11 $

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
// .NAME vtkKWMimxImageMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxImageMenuGroup_h
#define __vtkKWMimxImageMenuGroup_h


// MIMX Headers
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWFileBrowserDialog;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxImageViewProperties;
class vtkFocalPlaneContourRepresentation;
class vtkContourWidget;
class vtkImageActor;
class vtkKWMimxSaveImageGroup;
class vtkKWMimxImageThresholdGroup;
class vtkKWMimxLoadImageGroup;
class vtkKWMimxImageAutoSegmentGroup;
class vtkKWMimxImageManualSegmentGroup;
class vtkKWMimxCreateMaskFromFeatureGroup;
class vtkKWMimxCreateFeatureFromMaskGroup;
class vtkKWMimxCropImageGroup;
class vtkKWMimxSaveFeatureGroup;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxImageMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxImageMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxImageMenuGroup,vtkKWMimxMainMenuGroup);

  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  // Description:
  // Launch GUI for loading an image
  void LoadImageCallback();
  
  // Description:
  // Launch GUI for loading an ROI
  void LoadFeatureCallback();
  
  // Description:
  // Launch GUI to save an image
  void SaveImageCallback();
  
  // Description:
  // Launch GUI to save an ROI
  void SaveFeatureCallback();
  
  // Description:
  // Launch GUI for manual image tracing on an image (Current not used)
  void TraceImageCallback();
  
  // Description:
  // Launch GUI for deleting an image
  void DeleteImageCallback();
  
  // Description:
  // Launch GUI for thresholding an image
  void ThresholdImageCallback();
  
  // Description:
  // Launch GUI for Automated image segmentation
  void AutomatedImageSegmentationCallback();
  
  // Description:
  // Launch GUI for Manual image segmentation
  void ManualImageSegmentationCallback();
  
  // Description:
  // Launch GUI for Cropping an image
  void CropImageCallback();
  
  // Description:
  // Launch GUI for Converting a feature to a mask
  void FeatureToMaskCallback();
  
  // Description:
  // Launch GUI for converting a label image to a feature
  void MaskToFeatureCallback();
  
  // Description:
  // Set the various object types
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  
  // Description:
  // Get the Load Image Group to access load functions
  vtkGetObjectMacro(LoadImageGroup, vtkKWMimxLoadImageGroup);
  
  // Description:
  // Set the various object types
  vtkSetObjectMacro(FeatureList, vtkLinkedListWrapper);
  
protected:
	vtkKWMimxImageMenuGroup();
	~vtkKWMimxImageMenuGroup();
	vtkKWFileBrowserDialog *FileBrowserDialog;
	virtual void CreateWidget();
	vtkKWMimxImageViewProperties *MimxImageViewProperties;
  vtkFocalPlaneContourRepresentation *ContourRepresentation;
  vtkContourWidget *ContourWidget;
  vtkImageActor *ImageTraceActor;
  
  vtkKWMimxLoadImageGroup *LoadImageGroup;
  vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
  vtkKWMimxSaveImageGroup *SaveImageGroup;
  vtkKWMimxSaveFeatureGroup *SaveFeatureGroup;
  vtkKWMimxImageThresholdGroup *ThresholdImageGroup;
  vtkKWMimxImageAutoSegmentGroup *AutomatedSegmentImageGroup;
  vtkKWMimxImageManualSegmentGroup *ManualSegmentImageGroup;
  vtkKWMimxCropImageGroup *CropImageGroup;
  vtkKWMimxCreateMaskFromFeatureGroup *FeatureToMaskGroup;
  vtkKWMimxCreateFeatureFromMaskGroup *MaskToFeatureGroup;
  
private:
  vtkKWMimxImageMenuGroup(const vtkKWMimxImageMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxImageMenuGroup&); // Not implemented
};

#endif
