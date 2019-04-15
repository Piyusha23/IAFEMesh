#include "vtkMimxContourTraceWidget.h"
#include "vtkMimxContourTraceRepresentation.h"
#include "vtkContourWidget.h"
#include "vtkContourRepresentation.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataPointPlacer.h"
#include "vtkPropCollection.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkPropPicker.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkPolyData.h"
#include "vtkCellPicker.h"
#include "vtkOBBTree.h"
#include "vtkClipPolyData.h"
#include "vtkBox.h"
#include "vtkAppendPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkIdList.h"
#include "vtkCellData.h"
#include "vtkPolyDataWriter.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkMath.h"
#include "vtkCellArray.h"

void GetSlope2(int currSegNum, vtkIdList *idList, vtkPolyData *poly, double slope1[3], double slope2[3]);
void ComputeMidpoint2(double pt1[3], double pt2[3], double pt3[3]);
void InterpolatePolydataNodes2(vtkPolyData *inputPD);


vtkCxxRevisionMacro(vtkMimxContourTraceWidget, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkMimxContourTraceWidget);

//----------------------------------------------------------------------
vtkMimxContourTraceWidget::vtkMimxContourTraceWidget()
{
  //This functions will allow for polydata selection in contour widget
  this->CallbackMapper->SetCallbackMethod(vtkCommand::CharEvent,
                                          vtkEvent::NoModifier, 0, 1, "s",
										  vtkMimxContourTraceWidget::SButtonDown,
                                          this, vtkMimxContourTraceWidget::SButtonDownCallback);
 
  //This functions will allow for polydata selection in contour widget
  this->CallbackMapper->SetCallbackMethod(vtkCommand::CharEvent,
                                          vtkEvent::NoModifier, 0, 1, "d",
										  vtkMimxContourTraceWidget::DButtonDown,
                                          this, vtkMimxContourTraceWidget::DButtonDownCallback);

  this->CreateDefaultRepresentation();

  this->Isolated_Contours = vtkPolyData::New();
  this->Compiled_Contours = NULL;

  sliceposition = 0;
  axis = 0;

}
//----------------------------------------------------------------------
vtkMimxContourTraceWidget::~vtkMimxContourTraceWidget()
{
}
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::SButtonDownCallback(vtkAbstractWidget *w)
{
  std::cout << "Pressed S" << std::endl;
return;
  vtkMimxContourTraceWidget *self = reinterpret_cast<vtkMimxContourTraceWidget*>(w);
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	if (!self->CurrentRenderer || !self->CurrentRenderer->IsInViewport(X, Y))
	{
		self->WidgetEvent = vtkMimxContourTraceWidget::Outside;
		return;
	}
	
	vtkAssemblyPath *path;
	vtkCellPicker *cPicker = vtkCellPicker::New();
	cPicker->AddPickList(vtkPolyDataPointPlacer::SafeDownCast(
		vtkOrientedGlyphContourRepresentation::SafeDownCast(self->WidgetRep)
		->GetPointPlacer())->GetPropPicker()->GetPickList()->GetLastProp());
	cPicker->PickFromListOn();
	cPicker->SetTolerance(0.005);
	cPicker->Pick(X,Y,0.0,self->CurrentRenderer);
	path = cPicker->GetPath();
	
	double pickedLoc[3] = {0,0,0};

	if(path !=NULL)
	{
	  int PickedCell = cPicker->GetCellId();
	  if(PickedCell > -1)
	  {
		cPicker->GetPickPosition(pickedLoc);	
	  }
	}

	//Isolate the contours for the given slice
	self->IsolateSliceContours();

	int choseone = self->IdentifyCell(pickedLoc[0], pickedLoc[1], pickedLoc[2], self->Isolated_Contours);
	
	vtkIdList *keepids = vtkIdList::New();
	vtkIdList *otherids = vtkIdList::New();

	vtkDoubleArray *Darraykeep = vtkDoubleArray::New();
	Darraykeep->SetName("Slice_Position");

	vtkIntArray *Iarraykeep = vtkIntArray::New();
	Iarraykeep->SetName("Axis");

	vtkDoubleArray *Darrayother = vtkDoubleArray::New();
	Darrayother->SetName("Slice_Position");

	vtkIntArray *Iarrayother = vtkIntArray::New();
	Iarrayother->SetName("Axis");

	vtkDoubleArray *darray = vtkDoubleArray::SafeDownCast(self->Isolated_Contours->GetCellData()->GetArray("Slice_Position"));
	vtkIntArray *iarray = vtkIntArray::SafeDownCast(self->Isolated_Contours->GetCellData()->GetArray("Axis"));

	//From the slice's contours ... copy all cells but the one selected for deletion
	for(int i = 0; i < self->Isolated_Contours->GetNumberOfCells(); i++)
	{
		if(i == choseone)
		{
			keepids->InsertNextId(i);
			Darraykeep->InsertNextValue(darray->GetValue(i));						
			Iarraykeep->InsertNextValue(iarray->GetValue(i));
		}
		else
		{
			otherids->InsertNextId(i);
			Darrayother->InsertNextValue(darray->GetValue(i));						
			Iarrayother->InsertNextValue(iarray->GetValue(i));
		}
	}

	vtkPolyData *temporaryPD = vtkPolyData::New();
	temporaryPD->Allocate(1,1);
	vtkPointLocator *pointLocator = NULL;
	
	//Copy noninvolved cells back to Compiled_Contours
	if(otherids->GetNumberOfIds()>0)
	{
		temporaryPD->CopyCells(self->Isolated_Contours,otherids,pointLocator);
		temporaryPD->GetCellData()->AddArray(Darrayother);
		temporaryPD->GetCellData()->AddArray(Iarrayother);
		temporaryPD->Update();

		//Copy back to Compiled_Contours
		self->AppendCell(temporaryPD);

		//Also update visible contours for the slice
		self->Slice_Contours->DeepCopy(temporaryPD);
		self->UpdateVisibleSliceContours();
	}
	else if(otherids->GetNumberOfIds()==0)
	{
		self->Slice_Contours->DeepCopy(temporaryPD);
		self->UpdateVisibleSliceContours();
	}

	vtkPolyData *temporaryPD2 = vtkPolyData::New();
	temporaryPD2->Allocate(1,1);
	vtkPointLocator *pointLocator2 = NULL;

	//Add involved cells for modification
	if(keepids->GetNumberOfIds()>0)
	{
		temporaryPD2->CopyCells(self->Isolated_Contours,keepids,pointLocator2);
		temporaryPD2->GetCellData()->AddArray(Darraykeep);
		temporaryPD2->GetCellData()->AddArray(Iarraykeep);
		temporaryPD2->Update();

		self->ModifyContour(temporaryPD2);
	}
		
	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
	self->Interactor->Render();
}
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::DButtonDownCallback(vtkAbstractWidget *w)
{
	std::cout << "Pressed D" << std::endl;
 return; 
  vtkMimxContourTraceWidget *self = reinterpret_cast<vtkMimxContourTraceWidget*>(w);
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	if (!self->CurrentRenderer || !self->CurrentRenderer->IsInViewport(X, Y))
	{
		self->WidgetEvent = vtkMimxContourTraceWidget::Outside;
		return;
	}

	vtkAssemblyPath *path;
	vtkCellPicker *cPicker = vtkCellPicker::New();
	cPicker->AddPickList(vtkPolyDataPointPlacer::SafeDownCast(
		vtkOrientedGlyphContourRepresentation::SafeDownCast(self->WidgetRep)
		->GetPointPlacer())->GetPropPicker()->GetPickList()->GetLastProp());
	cPicker->PickFromListOn();
	cPicker->SetTolerance(0.005);
	cPicker->Pick(X,Y,0.0,self->CurrentRenderer);
	path = cPicker->GetPath();
	
	double pickedLoc[3] = {0,0,0};

	//Determine picked point in 3-space
	if(path !=NULL)
	{
	  int PickedCell = cPicker->GetCellId();
	  if(PickedCell > -1)
	  {
		cPicker->GetPickPosition(pickedLoc);	
	  }
	}

	//Isolate the contours for the given slice
	self->IsolateSliceContours();

	int choseone = self->IdentifyCell(pickedLoc[0], pickedLoc[1], pickedLoc[2], self->Isolated_Contours);
	
	vtkIdList *keepids = vtkIdList::New();

	vtkDoubleArray *Darray = vtkDoubleArray::New();
	Darray->SetName("Slice_Position");

	vtkIntArray *Iarray = vtkIntArray::New();
	Iarray->SetName("Axis");

	vtkDoubleArray *darray = vtkDoubleArray::SafeDownCast(self->Isolated_Contours->GetCellData()->GetArray("Slice_Position"));
	vtkIntArray *iarray = vtkIntArray::SafeDownCast(self->Isolated_Contours->GetCellData()->GetArray("Axis"));

	//From the slice's contours ... copy all cells but the one selected for deletion
	for(int i = 0; i < self->Isolated_Contours->GetNumberOfCells(); i++)
	{
		if(i != choseone)
		{
			keepids->InsertNextId(i);
			Darray->InsertNextValue(darray->GetValue(i));						
			Iarray->InsertNextValue(iarray->GetValue(i));
		}
	}

	vtkPolyData *temporaryPD = vtkPolyData::New();
	temporaryPD->Allocate(1,1);
	vtkPointLocator *pointLocator = NULL;
	
	//Copy noninvolved cells back to Compiled_Contours
	if(keepids->GetNumberOfIds()>0)
	{
		temporaryPD->CopyCells(self->Isolated_Contours,keepids,pointLocator);
		temporaryPD->GetCellData()->AddArray(Darray);
		temporaryPD->GetCellData()->AddArray(Iarray);
		temporaryPD->Update();

		//append temporary PD to Compiled contours
		self->AppendCell(temporaryPD);

		//reassign the slice's contours to that which are still left
		self->Slice_Contours->DeepCopy(temporaryPD);
		self->UpdateVisibleSliceContours();
	}

	else if(keepids->GetNumberOfIds()==0)
	{
		self->Slice_Contours->DeepCopy(temporaryPD);
		self->UpdateVisibleSliceContours();
	}

	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
	self->Interactor->Render();
}
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::AddPolyData(vtkPolyData *InputPolyData)
{
	this->Compiled_Contours = InputPolyData;
} 
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::AddPositionInfo(double sliceLoc, int axisview)
{
	this->sliceposition = sliceLoc;
	this->axis = axisview;
}
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::IsolateSliceContours()
{
	this->Isolated_Contours->Initialize();

	//Create a list of ids for the active contours...and everything else in another list
	vtkIdList *activeids = vtkIdList::New();
	vtkIdList *inactiveids = vtkIdList::New();

	vtkDoubleArray *darray = vtkDoubleArray::SafeDownCast(this->Compiled_Contours->GetCellData()->GetArray("Slice_Position"));
	vtkIntArray *iarray = vtkIntArray::SafeDownCast(this->Compiled_Contours->GetCellData()->GetArray("Axis"));

	vtkDoubleArray *DarrayActive = vtkDoubleArray::New();
	DarrayActive->SetName("Slice_Position");
	
	vtkIntArray *IarrayActive = vtkIntArray::New();
	IarrayActive->SetName("Axis");

	vtkDoubleArray *DarrayInactive = vtkDoubleArray::New();
	DarrayInactive->SetName("Slice_Position");

	vtkIntArray *IarrayInactive = vtkIntArray::New();
	IarrayInactive->SetName("Axis");

	for(int i = 0; i<this->Compiled_Contours->GetNumberOfCells(); i++)
	{
		//Establish which axis to investigate for finding contours on a slice
		if(darray->GetValue(i) == sliceposition)
		{
			activeids->InsertNextId(i);
			DarrayActive->InsertNextValue(darray->GetValue(i));
			IarrayActive->InsertNextValue(iarray->GetValue(i));
		}
		else
		{
			inactiveids->InsertNextId(i);
			DarrayInactive->InsertNextValue(darray->GetValue(i));
			IarrayInactive->InsertNextValue(iarray->GetValue(i));
		}
	}
		
	//Get Active Contours
	vtkPolyData *temporaryPD = vtkPolyData::New();
	temporaryPD->Allocate(1,1);
	vtkPointLocator *pointLocator = NULL;

	temporaryPD->CopyCells(this->Compiled_Contours,activeids,pointLocator);
	temporaryPD->GetCellData()->AddArray(DarrayActive);
	temporaryPD->GetCellData()->AddArray(IarrayActive);
	temporaryPD->Update();

	this->Isolated_Contours->DeepCopy(temporaryPD);
	this->Isolated_Contours->Update();
	temporaryPD->Delete();

	//Update the CompiledContours List
	vtkPolyData *temporaryPD2 = vtkPolyData::New();
	temporaryPD2->Allocate(1,1);
	//vtkPointLocator *pointLocator2 = NULL;
	vtkPolyData *temporaryPD3 = vtkPolyData::New();
	temporaryPD3->Allocate(1,1);

	if(inactiveids->GetNumberOfIds()>0)
	{
		temporaryPD2->CopyCells(this->Compiled_Contours,inactiveids,pointLocator);
		temporaryPD2->GetCellData()->AddArray(DarrayInactive);
		temporaryPD2->GetCellData()->AddArray(IarrayInactive);
		temporaryPD2->Update();

		this->Compiled_Contours->DeepCopy(temporaryPD3);
		this->Compiled_Contours->DeepCopy(temporaryPD2);
		this->Compiled_Contours->Update();
		temporaryPD2->Delete();
	}
	else
	{
		//Set Compiled to zero since no other slices contain contours
		vtkPolyData *temporaryPD3 = vtkPolyData::New();
		temporaryPD3->Allocate(1,1);
		this->Compiled_Contours->DeepCopy(temporaryPD3);
	}
}
//----------------------------------------------------------------------
int vtkMimxContourTraceWidget::IdentifyCell(double x, double y, double z, vtkPolyData *InputPolyData)
{
	double selected_point[3] = {x, y, z};
	
	int cellId = 0;
	int pointid = this->cp(InputPolyData,selected_point);

	for(int i = 0; i<InputPolyData->GetNumberOfCells(); i++)
	{
		if(InputPolyData->IsPointUsedByCell(pointid, i)!=0)
		{
			cellId = i;
		}
	}

	return cellId; 
}
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::AppendCell(vtkPolyData *InputPolyData)
{
	//Append copied polydata to Compiled_Contours
	vtkAppendPolyData *appendfilter = vtkAppendPolyData::New();
	appendfilter->SetInput(this->Compiled_Contours);
	appendfilter->AddInput(InputPolyData);
	appendfilter->Update();

	//Update All_Contours with new cell
	this->Compiled_Contours->DeepCopy(appendfilter->GetOutput());
	this->Compiled_Contours->Update();
}
//----------------------------------------------------------------------
void vtkMimxContourTraceWidget::ModifyContour(vtkPolyData *InputPolyData)
{
	this->Initialize(InputPolyData,1);
}
//----------------------------------------------------------------------
int vtkMimxContourTraceWidget::cp(vtkPolyData *inputPoly, double point[3])
{
	//Snap External Vertices of Building Block to Closest Points on Surface
	//Temporary distance calculated value
	double mindistance = 999999999;
	double tempdistance = 0;
	int pointid = 0; 

	//Storage of coordinates of point with shortest distance to vertex
	double origcoords[3] = {point[0],point[1],point[2]};
	double ptcoords[3] = {0,0,0};

	//total number of points
	vtkPoints *poly_points = vtkPoints::SafeDownCast(inputPoly->GetPoints());

	//Loop for all points
	for(int j=0; j < poly_points->GetNumberOfPoints(); j++)
	{
		poly_points->GetPoint(j,ptcoords);
		double x_dist = pow((origcoords[0] - ptcoords[0]),2);
		double y_dist = pow((origcoords[1] - ptcoords[1]),2);
		double z_dist = pow((origcoords[2] - ptcoords[2]),2);
		tempdistance = sqrt(x_dist+y_dist+z_dist);

		if(tempdistance < mindistance)
		{
			mindistance = tempdistance;
			pointid = j;
		}
	}
	return pointid;
}
//----------------------------------------------------------------------------------
void vtkMimxContourTraceWidget::AddSlicePolyData(vtkPolyData *InputPolyData)
{
	this->Slice_Contours = InputPolyData;
}
//----------------------------------------------------------------------------------
void vtkMimxContourTraceWidget::AddSliceActor(vtkActor *InputActor)
{
	this->SliceActor = InputActor;
}
//----------------------------------------------------------------------------------
void vtkMimxContourTraceWidget::UpdateVisibleSliceContours()
{
	this->SliceActor->SetVisibility(0);

	vtkPolyData *temp_Slice_Contours = vtkPolyData::New();
	temp_Slice_Contours->Allocate(1,1);

	//Interpolate slice's contours
	for(int i = 0; i < this->Slice_Contours->GetNumberOfCells(); i++)
	{
		//Copy Individual Contours
		vtkIdList *indivContour = vtkIdList::New();
		indivContour->InsertNextId(i);	
			
		vtkPolyData *tempPoly = vtkPolyData::New();
		tempPoly->Allocate(1,1);
		tempPoly->CopyCells(this->Slice_Contours,indivContour);
		tempPoly->Update();

		InterpolatePolydataNodes2(tempPoly);

		vtkAppendPolyData *appendfilter = vtkAppendPolyData::New();
		appendfilter->SetInput(temp_Slice_Contours);
		appendfilter->AddInput(tempPoly);
		appendfilter->Update();

		temp_Slice_Contours->DeepCopy(appendfilter->GetOutput());
	}

	this->Slice_Contours->DeepCopy(temp_Slice_Contours);
	this->Slice_Contours->Modified();

	this->SliceActor->SetVisibility(1);

}
//----------------------------------------------------------------------------------
//Code for Interpolation Visualization - derived from Bezier Interpolator by Kiran
void GetSlope2(int currSegNum, vtkIdList *idList, vtkPolyData *poly, double slope1[3], double slope2[3])
{
	// get the appropriate segment numbers
	// as of now it is assumed the contour is closed
	int prevSegNum;
	int nextSegNum;

	int numSegs = idList->GetNumberOfIds()-1;
	if(currSegNum == 0)	
	{
		prevSegNum = numSegs-1;
		nextSegNum = currSegNum+1;
	}
	else if(currSegNum == numSegs-1)
	{
		prevSegNum = currSegNum-1;
		nextSegNum = 0;
	}
	else
	{
		prevSegNum = currSegNum-1;
		nextSegNum = currSegNum+1;
	}

	// compute the slope of current segment
	double pt1[3], pt2[3];
	int idx1 = idList->GetId(currSegNum);
	int idx2 = idList->GetId(currSegNum+1);
	double slopecurr[3];
	poly->GetPoint(idx1, pt1);
	poly->GetPoint(idx2, pt2);
	for (int i=0; i<3; i++)
	{
		slopecurr[i] = pt2[i] - pt1[i];
	}
	vtkMath::Normalize(slopecurr);

	// compute the slopes for prev and next segments
	// for prev segment
	idx1 = idList->GetId(prevSegNum);
	idx2 = idList->GetId(prevSegNum+1);
	poly->GetPoint(idx1, pt1);
	poly->GetPoint(idx2, pt2);
	for (int i=0; i<3; i++)
	{
		slope1[i] = pt2[i] - pt1[i];
	}
	vtkMath::Normalize(slope1);
	for (int i=0; i<3; i++)
	{
		slope1[i] = slope1[i] + slopecurr[i];
	}
	vtkMath::Normalize(slope1);

	// next segment
	idx1 = idList->GetId(nextSegNum);
	idx2 = idList->GetId(nextSegNum+1);
	poly->GetPoint(idx1, pt1);
	poly->GetPoint(idx2, pt2);
	for (int i=0; i<3; i++)
	{
		slope2[i] = pt2[i] - pt1[i];
	}
	vtkMath::Normalize(slope2);
	for (int i=0; i<3; i++)
	{
		slope2[i] = slope2[i] + slopecurr[i];
	}
	vtkMath::Normalize(slope2);

}
void ComputeMidpoint2(double pt1[3], double pt2[3], double pt3[3])
{
	for (int i=0; i<3; i++)
	{
		pt3[i] = (pt1[i] + pt2[i])/2.0;
	}
}
void InterpolatePolydataNodes2(vtkPolyData *inputPD)
{
	// loop through all line segments
	vtkIdList *idList = vtkIdList::New();
	inputPD->GetCellPoints(0, idList);

	vtkCollection *coll = vtkCollection::New();
	int numSegments = idList->GetNumberOfIds()-1;
	for (int i=0; i<numSegments; i++)
	{
		vtkPoints *points = vtkPoints::New();
		coll->AddItem(points);
		int idx1 = idList->GetId(i);
		int idx2 = idList->GetId(i+1);

		int MaximumCurveLineSegments = 100;
		int maxRecursion = 0;
		int tmp = 3;
		double MaximumCurveError = 0.005;

		while ( 2*tmp < MaximumCurveLineSegments )
		{
			tmp *= 2;
			maxRecursion++;
		}

		if ( maxRecursion == 0 )
		{
			return;
		}

		// There are four control points with 3 components each, plus one
		// value for the recursion depth of this point
		double *controlPointsStack = new double[(3*4+1)*(maxRecursion+1)];
		int stackCount = 0;

		double slope1[3];
		double slope2[3];

		GetSlope2(i, idList, inputPD, slope1, slope2);

		controlPointsStack[0] = 0;
		double *p1 = controlPointsStack+1;
		double *p2 = controlPointsStack+4;
		double *p3 = controlPointsStack+7;
		double *p4 = controlPointsStack+10;
		
		inputPD->GetPoint(idx1, p1);
		inputPD->GetPoint(idx2, p4);

		double distance = sqrt( vtkMath::Distance2BetweenPoints( p1, p4 ) );

		p2[0] = p1[0] + .333*distance*slope1[0];
		p2[1] = p1[1] + .333*distance*slope1[1];
		p2[2] = p1[2] + .333*distance*slope1[2];

		p3[0] = p4[0] - .333*distance*slope2[0];
		p3[1] = p4[1] - .333*distance*slope2[1];
		p3[2] = p4[2] - .333*distance*slope2[2];

		stackCount++;

		while ( stackCount )
		{
			//process last point on stack
			int recursionLevel = static_cast<int>(controlPointsStack[13*(stackCount-1)]);

			p1 = controlPointsStack + 13*(stackCount-1)+1;
			p2 = controlPointsStack + 13*(stackCount-1)+4;
			p3 = controlPointsStack + 13*(stackCount-1)+7;
			p4 = controlPointsStack + 13*(stackCount-1)+10;

			double totalDist = 0;
			totalDist += sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
			totalDist += sqrt(vtkMath::Distance2BetweenPoints(p2,p3));
			totalDist += sqrt(vtkMath::Distance2BetweenPoints(p3,p4));

			distance = sqrt(vtkMath::Distance2BetweenPoints(p1,p4));

			if ( recursionLevel >= maxRecursion || distance == 0 ||
				(totalDist - distance)/distance < MaximumCurveError ) 
			{
				points->InsertNextPoint(p2 );
				points->InsertNextPoint(p3 );

				if ( stackCount > 1 )
				{
					points->InsertNextPoint(p4 );
				}
				stackCount--;
			}
			else
			{
				double p12[3], p23[3], p34[3], p123[3], p234[3], p1234[3];

				ComputeMidpoint2( p1, p2, p12 );
				ComputeMidpoint2( p2, p3, p23 );
				ComputeMidpoint2( p3, p4, p34 );
				ComputeMidpoint2( p12, p23, p123 );
				ComputeMidpoint2( p23, p34, p234 );
				ComputeMidpoint2( p123, p234, p1234 );

				// add these two points to the stack
				controlPointsStack[13*(stackCount-1)] = recursionLevel+1;
				controlPointsStack[13*(stackCount)] = recursionLevel+1;

				double *newp1 = controlPointsStack + 13*(stackCount)+1;
				double *newp2 = controlPointsStack + 13*(stackCount)+4;
				double *newp3 = controlPointsStack + 13*(stackCount)+7;
				double *newp4 = controlPointsStack + 13*(stackCount)+10;

				newp1[0] = p1[0];
				newp1[1] = p1[1];
				newp1[2] = p1[2];

				newp2[0] = p12[0];
				newp2[1] = p12[1];
				newp2[2] = p12[2];

				newp3[0] = p123[0];
				newp3[1] = p123[1];
				newp3[2] = p123[2];

				newp4[0] = p1234[0];
				newp4[1] = p1234[1];
				newp4[2] = p1234[2];

				p1[0] = p1234[0];
				p1[1] = p1234[1];
				p1[2] = p1234[2];

				p2[0] = p234[0];
				p2[1] = p234[1];
				p2[2] = p234[2];

				p3[0] = p34[0];
				p3[1] = p34[1];
				p3[2] = p34[2];

				stackCount++;
			}
		}
		delete [] controlPointsStack;
	}

	// create the polyline with interpolated points
	vtkPolyData *interpPoly = vtkPolyData::New();
	vtkPoints *totPoints = vtkPoints::New();
	for (int i=0; i<numSegments; i++)
	{
		vtkPoints *points = vtkPoints::SafeDownCast(coll->GetItemAsObject(i)); 
		int numPts = points->GetNumberOfPoints();
		for (int j=0; j< numPts; j++)
		{
			totPoints->InsertNextPoint(points->GetPoint(j));
		}
	}

	int numCells = totPoints->GetNumberOfPoints()-1;
	vtkCellArray *cellArray = vtkCellArray::New();
	cellArray->InsertNextCell(numCells);
	
	for(int i=0; i<numCells; i++)
	{
		cellArray->InsertCellPoint(i);
	}
	interpPoly->SetPoints(totPoints);
	interpPoly->SetLines(cellArray);

	inputPD->DeepCopy(interpPoly);
}
