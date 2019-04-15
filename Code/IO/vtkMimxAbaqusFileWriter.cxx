/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxAbaqusFileWriter.cxx,v $
Language:  C++
Date:      $Date: 2012/12/07 19:08:59 $
Version:   $Revision: 1.1.1.1 $


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

#include "vtkMimxAbaqusFileWriter.h"

#include <vtkMath.h>
#include <vtksys/SystemTools.hxx>
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include "vtkSortDataArray.h"
#include <string.h>
#include <list>
#include <algorithm>
#include <vtksys/SystemTools.hxx>


//using std::list;
//using std::string;

vtkCxxRevisionMacro(vtkMimxAbaqusFileWriter, "$Revision: 1.1.1.1 $");
vtkStandardNewMacro(vtkMimxAbaqusFileWriter);

vtkMimxAbaqusFileWriter::vtkMimxAbaqusFileWriter()
{
  HeaderInformation = "";
  FileName = "";
  NodeElementFileName = "";
  UserName = "";
  Precision = 0;
}

vtkMimxAbaqusFileWriter::~vtkMimxAbaqusFileWriter()
{
}


//----------------------------------------------------------------------------
int vtkMimxAbaqusFileWriter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  //vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
    
  if (! vtkUnstructuredGrid::SafeDownCast(input)->GetPointData()->GetArray("Node_Numbers") )
  {
    vtkErrorMacro("Unstructured grid must contain field data called 'Node_Numbers'");
    return 0;
  }
  
  if (! vtkUnstructuredGrid::SafeDownCast(input)->GetCellData()->GetArray("Element_Numbers") )
  {
    vtkErrorMacro("Unstructured grid must contain field data called 'Element_Numbers'");
    return 0;
  }

  std::ofstream outfile;  
  outfile.open(this->FileName.c_str(), std::ios::out|std::fstream::trunc );

  // GetFilenameWithoutExtension
  if (NodeElementFileName.length() == 0)
  {
    this->NodeElementFileName = vtksys::SystemTools::GetFilenamePath( this->FileName );
	std::string fname = vtksys::SystemTools::GetFilenameWithoutExtension(this->FileName);
	if(this->NodeElementFileName.length() != 0)
	{
		this->NodeElementFileName += "/";
	}
	this->NodeElementFileName += fname;
    this->NodeElementFileName += "_input";
  }
  std::ofstream nodefile;  
  nodefile.open(this->NodeElementFileName.c_str(), std::ios::out|std::fstream::trunc );


  WriteHeader ( outfile );
  WriteHeadingSection( outfile );
  WriteNodeElementHeader( outfile );
  WriteNodes( nodefile, vtkUnstructuredGrid::SafeDownCast( input ) );
  WriteHexElements( nodefile, vtkUnstructuredGrid::SafeDownCast( input ) );
  WriteTetElements( nodefile, vtkUnstructuredGrid::SafeDownCast( input ) );
  WriteQuadElements( nodefile, vtkUnstructuredGrid::SafeDownCast( input ) );
  WriteNodeSets(nodefile, vtkUnstructuredGrid::SafeDownCast( input ) );
  //Get Unique Material Properties
  WriteElementSets( nodefile, vtkUnstructuredGrid::SafeDownCast( input ) );
  WriteMaterialHeader( outfile );
  WriteMaterialProperties( outfile, nodefile, vtkUnstructuredGrid::SafeDownCast( input ));
  WriteBoundaryConditions( outfile, vtkUnstructuredGrid::SafeDownCast( input ) );
  WriteFooter( outfile );

  return 1;
}

  
void vtkMimxAbaqusFileWriter::WriteHeader( std::ostream& os )
{
  os << "**=========================================================================" << std::endl;
  os << "**     MODEL:" << std::endl;
  os << "**        ==>" << vtksys::SystemTools::GetFilenameName(this->FileName) << std::endl; 
  os << "**" << std::endl;
  if ( this->UserName.length() > 0 )
  {
    os << "**     CREATED BY:" << std::endl;
    os << "**                " << this->UserName << std::endl;
  }
  
  os << "**" << std::endl;
  os << "**" << std::endl;
  if ( this->HeaderInformation.length() > 0 )
  {
    os << "**     MODEL DESCRIPTION:" << std::endl;
    size_t strIndex = HeaderInformation.find("\n", 0);
    if (strIndex != std::string::npos)
    {
      os << "**                " << 
        this->HeaderInformation.substr(0,strIndex) << 
        std::endl;
      while (strIndex != std::string::npos)
      {
        int prevIndex = strIndex + 1;
        strIndex = HeaderInformation.find("\n", prevIndex);
        if (strIndex == std::string::npos)
        {
          if (strIndex != this->HeaderInformation.length())
            os << "**                " << 
                 this->HeaderInformation.substr(prevIndex,this->HeaderInformation.length()-prevIndex+1) << 
                 std::endl;
        }
        else if (strIndex == prevIndex)
        {
          os << "**                " << std::endl;
        }
        else
        {
          os << "**                " << 
            this->HeaderInformation.substr(prevIndex,strIndex-prevIndex) << 
            std::endl;
        }
      }
    }
    else
    {
      os << "**                " << this->HeaderInformation << std::endl;
    }
  }
  
  
  
  os << "**" << std::endl;
  os << "**" << std::endl;
  os << "**     CREATED ON:" << std::endl;
  // Get Date - vtksys::SystemTools::GetCurrentDateTime
  std::string dateString = vtksys::SystemTools::GetCurrentDateTime("%m/%d/%Y %H:%M");
  // char * asctime(const struct tm* tm);
  // time()
  os << "**        ==> " << dateString << std::endl;
  os << "**" << std::endl;
  /*
  os << "**     RUN COMMAND:" << std::endl;
  os << "**        ==> abaqus job=filename datacheck" << std::endl; 
  os << "**        ==> abaqus job=filename" << std::endl;
  os << "**" << std::endl;
  os << "**" << std::endl;
  os << "**     DESCRIPTION:  Include a brief description of the model" << std::endl;
  os << "**" << std::endl;
  */
  os << "**     MODIFICATION HISTORY:" << std::endl;
  os << "**        ==>" << std::endl;
  os << "**" << std::endl;
  os << "**=========================================================================" << std::endl;
  os << "**                            MODEL DATA                                 **" << std::endl;
}

void vtkMimxAbaqusFileWriter::WriteHeadingSection( std::ostream& os )
{
/*
  if ( Heading.length() )
  {
    os << "*HEADING" << endl;
    os << Heading << endl;
  }
*/
}

void vtkMimxAbaqusFileWriter::WriteNodeElementHeader( std::ostream& os )
{
  os << "**=========================================================================" << std::endl;
  os << "**                                               Node / Element Definitions" << std::endl;
  os << "**" << std::endl;
  os << "*INCLUDE, INPUT=" << vtksys::SystemTools::GetFilenameWithoutExtension(this->NodeElementFileName) << std::endl;
  os << "**" << std::endl;
  os << "**" << std::endl;
}    


void vtkMimxAbaqusFileWriter::WriteNodes( std::ostream& os, vtkUnstructuredGrid *grid )
{
  vtkIntArray *nodeArray = vtkIntArray::SafeDownCast( grid->GetPointData()->GetArray("Node_Numbers") );
  int numberOfPoints = grid->GetNumberOfPoints();
  
  os << "*NODE" << std::endl;
  
  for( int pointIndex = 0; pointIndex < numberOfPoints; pointIndex++ )
  {
    double point[3];
    grid->GetPoint( pointIndex, point );
    os << nodeArray->GetValue(pointIndex) << ", " << point[0] << ", " << point[1] << ", " << point[2] << std::endl;
  }
}

void vtkMimxAbaqusFileWriter::WriteHexElements( std::ostream& os, vtkUnstructuredGrid *grid )
{  
  vtkIntArray *nodeArray = vtkIntArray::SafeDownCast( grid->GetPointData()->GetArray("Node_Numbers") );
  vtkIntArray *elementArray = vtkIntArray::SafeDownCast( grid->GetCellData()->GetArray("Element_Numbers") );
  
  int numberOfCells = grid->GetNumberOfCells( );
  bool headerFlag = true;
  
  for( int elementId = 0; elementId < numberOfCells; elementId++ )
  {
    int cellType = grid->GetCellType( elementId );
    if ( cellType == VTK_HEXAHEDRON )
    {
      if ( headerFlag )
      {
        os << "**" << std::endl;
        os << "**" << std::endl;
        os << "*ELEMENT, TYPE=C3D8" << std::endl;
        headerFlag = false;
      }
      
      vtkIdList *idlist = grid->GetCell( elementId )->GetPointIds( );
      int numberOfIds = idlist->GetNumberOfIds( );
      
      os << elementArray->GetValue(elementId);
      
      for ( int count = 0; count < numberOfIds; count ++ )
      {
        os << ", " << nodeArray->GetValue( idlist->GetId(count) );
      }
      os << std::endl;
    }
  }
}

			  
void vtkMimxAbaqusFileWriter::WriteTetElements( std::ostream& os, vtkUnstructuredGrid *grid )
{  
  vtkIntArray *nodeArray = vtkIntArray::SafeDownCast( grid->GetPointData()->GetArray("Node_Numbers") );
  vtkIntArray *elementArray = vtkIntArray::SafeDownCast( grid->GetCellData()->GetArray("Element_Numbers") );

  int numberOfCells = grid->GetNumberOfCells( );  
  bool headerFlag = false;
  
  for( int elementId = 0; elementId < numberOfCells; elementId++ )
  {
    int cellType = grid->GetCellType( elementId );
    if ( cellType == VTK_TETRA )
    {
      if ( headerFlag )
      {
        os << "**" << std::endl;
        os << "**" << std::endl;
        os << "*ELEMENT, TYPE=C3D4" << std::endl;
        headerFlag = false;
      }
      vtkIdList *idlist = grid->GetCell( elementId )->GetPointIds( );
      int numberOfIds = idlist->GetNumberOfIds( );
      
      os << elementArray->GetValue(elementId);
      
      for ( int count = 0; count < numberOfIds; count ++ )
      {
        os << ", " << nodeArray->GetValue( idlist->GetId(count) );
      }
      os << std::endl;
    }
  }
}

void vtkMimxAbaqusFileWriter::WriteQuadElements( std::ostream& os, vtkUnstructuredGrid *grid )
{  
  vtkIntArray *nodeArray = vtkIntArray::SafeDownCast( grid->GetPointData()->GetArray("Node_Numbers") );
  vtkIntArray *elementArray = vtkIntArray::SafeDownCast( grid->GetCellData()->GetArray("Element_Numbers") );
  
  int numberOfCells = grid->GetNumberOfCells( );
  int headerFlag = true;
  
  for( int elementId = 0; elementId < numberOfCells; elementId++ )
  {
    int cellType = grid->GetCellType( elementId );
    if ( cellType == VTK_QUAD )
    {
      if ( headerFlag )
      {
        os << "**" << std::endl;
        os << "**" << std::endl;
        os << "*ELEMENT, TYPE=R3D4" << std::endl;
        headerFlag = false;
      }
      vtkIdList *idlist = grid->GetCell( elementId )->GetPointIds( );
      int numberOfIds = idlist->GetNumberOfIds( );
      
      os << elementArray->GetValue(elementId);
      
      for ( int count = 0; count < numberOfIds; count ++ )
      {
        os << ", " << nodeArray->GetValue( idlist->GetId(count) );
      }
      os << std::endl;
    }
  }
}

void vtkMimxAbaqusFileWriter::WriteElementSets( std::ostream& os, vtkUnstructuredGrid *grid )
{  
	// loop through all element sets
	int i,j;
	vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
		grid->GetFieldData()->GetAbstractArray("Element_Set_Names"));
	int numelementarrays = stringarray->GetNumberOfTuples();

	vtkIntArray *elementnumarray = vtkIntArray::SafeDownCast(
		grid->GetCellData()->GetArray("Element_Numbers"));
	for (i=0; i<numelementarrays; i++)
	{
		vtkIntArray *intarray = vtkIntArray::SafeDownCast(grid->GetCellData()->GetArray(stringarray->GetValue(i)));
		/*if(intarray && strcmp(grid->GetCellData()->GetArray(i)->GetName(), "Element_Numbers")
			&& intarray->GetNumberOfComponents() == 1)
		{*/
      std::list<int> elementnum;
			elementnum.clear();
			// add the nodes belonging to the node set to the list
			for (j=0; j<intarray->GetNumberOfTuples(); j++)
			{
				if(intarray->GetValue(j))
				{
					elementnum.push_back(elementnumarray->GetValue(j));
				}
			}
			if (!elementnum.size())
			{
				return;
			}
			// sort the entries in ascending order
			elementnum.sort();			
			vtkIdList *storearray = vtkIdList::New();
			do 
			{
				storearray->InsertUniqueId(elementnum.front());
				elementnum.pop_front();
			} while(elementnum.size());

			int numberOfElements = storearray->GetNumberOfIds();
			const char *arrayname = stringarray->GetValue(i);
			os << "*ELSET, ELSET=" << arrayname << ", GENERATE" << std::endl;
			// loop through all the nodes in the node set
			int firstelement, lastelement, currelement;
			if(numberOfElements > 1)
			{
				for (j=0; j<numberOfElements; j++)
				{
					if(j == 0)
					{
						firstelement = storearray->GetId(j);
						lastelement = firstelement;
						currelement = storearray->GetId(j+1);
						j++;
					}
					if(currelement != lastelement+1)
					{
						os<<firstelement<<", "<<lastelement<<std::endl;
						firstelement = currelement;
						lastelement = currelement;
						if(j < numberOfElements-1)
						{
							currelement = storearray->GetId(j+1);
						}
						if (j == numberOfElements-1)
						{
							os<<firstelement<<", "<<lastelement<<std::endl;
						}
					}
					else
					{
						lastelement = currelement;
						if(j < numberOfElements-1)
						{
							currelement = storearray->GetId(j+1);
						}
						if(j == numberOfElements-1)
						{
							os<<firstelement<<", "<<lastelement<<std::endl;
						}
					}
				}
			}
			else{
				firstelement = storearray->GetId(0);
				lastelement = storearray->GetId(0);
				os<<firstelement<<", "<<lastelement<<std::endl;
			}
		//}
	} 
}

void vtkMimxAbaqusFileWriter::WriteMaterialHeader( std::ostream& os )
{
  os << "**=========================================================================" << std::endl;
  os << "**                                                    Material  Definitions" << std::endl;
  os << "**" << std::endl;
}
    
void vtkMimxAbaqusFileWriter::WriteMaterialProperties( std::ostream& os, ostream& osi, vtkUnstructuredGrid *grid )
{
	int i;
	bool firstImageProp = true;
  bool hasImageProp = false;
	vtkStringArray *strarray = vtkStringArray::SafeDownCast(
		grid->GetFieldData()->GetAbstractArray("Element_Set_Names"));

	if(!strarray)
	{
		return;
	}
	int numEntries = strarray->GetNumberOfTuples();

	for (i=0; i<numEntries; i++)
	{
		char str[256];
		strcpy(str, strarray->GetValue(i));
		strcat(str, "_Constant_Youngs_Modulus");
		vtkDoubleArray *youngarray = vtkDoubleArray::SafeDownCast(
			grid->GetFieldData()->GetArray(str));

		char strpoi[256];
		strcpy(strpoi, strarray->GetValue(i));
		strcat(strpoi, "_Constant_Poissons_Ratio");
		vtkFloatArray *poissonarray = vtkFloatArray::SafeDownCast(
			grid->GetFieldData()->GetArray(strpoi));

		if(youngarray && poissonarray)
		{
			float youngsmodulus = youngarray->GetValue(0);
			float poissonsratio = poissonarray->GetValue(0);
			os << "*SOLID SECTION, ELSET=" << strarray->GetValue(i) 
				<< ", MATERIAL=" << strarray->GetValue(i) << std::endl;
			os << "*MATERIAL, NAME=" << strarray->GetValue(i) << std::endl; 
			os << "*ELASTIC" << std::endl;
			if (this->Precision != 0) 
			{
  			os.setf(std::ios_base::fixed,std::ios::floatfield);
  			os.precision( this->Precision );
			}
			os << youngsmodulus << ", " << poissonsratio << std::endl;
			if (this->Precision != 0) 
			{
			  os.unsetf(std::ios::floatfield);
			  os.precision( 0 );
			}
		}
		else
		{
			
			if(poissonarray)
			{
				if ( firstImageProp )
  			{
  			  WriteMaterialHeader( osi );
  			  firstImageProp = false;
  			}
			
				vtkIntArray *elementArray = vtkIntArray::SafeDownCast( 
					grid->GetCellData()->GetArray(strarray->GetValue(i)) );

				//int numberOfCells = grid->GetNumberOfCells( );
				
				char str[256];
				strcpy(str, strarray->GetValue(i));
				strcat(str, "_Image_Based_Material_Property");
				
				char rebinstr[256];
				strcpy(rebinstr, str);
				strcat(rebinstr, "_ReBin");
				// store the elements with repeating material properties
				vtkDoubleArray *materialArray  = vtkDoubleArray::SafeDownCast( 
					grid->GetCellData()->GetArray(rebinstr));
				if(!materialArray)	materialArray  = vtkDoubleArray::SafeDownCast( 
					grid->GetCellData()->GetArray(str));
				if(materialArray)
					this->WriteRepeatingMaterialProperties(
						elementArray, materialArray, grid, osi, 
						strarray->GetValue(i), poissonarray->GetValue(0));
				hasImageProp = true;
			}
		}
	}
	os << "**" << std::endl;
	if (hasImageProp)
	  osi << "**" << std::endl;
	// check for the reference node, if exists write it out
	vtkStringArray *nodearray = vtkStringArray::SafeDownCast(
		grid->GetFieldData()->GetAbstractArray("Reference_Node_Parameters"));
  if(nodearray)
  {
	  os <<"*RIGID BODY, ELSET= "<<nodearray->GetValue(1)<<", REF NODE= "<<nodearray->GetValue(0)<<std::endl;
  }
}

void vtkMimxAbaqusFileWriter::WriteBoundaryConditions( std::ostream& os, vtkUnstructuredGrid *grid )
{  
  
  os << "**=========================================================================" << std::endl;
  os << "**                          HISTORY DATA                                 **" << std::endl;
  os << "**=========================================================================" << std::endl;
  char Concatenate[256];
  int i, j;
  vtkStringArray *nodesetnamestring = vtkStringArray::SafeDownCast(
	  grid->GetFieldData()->GetAbstractArray("Node_Set_Names"));

  vtkStringArray *elementsetnamestring = vtkStringArray::SafeDownCast(
	  grid->GetFieldData()->GetAbstractArray("Element_Set_Names"));
  if(!nodesetnamestring)	return;

  vtkIntArray *NumSteps = vtkIntArray::SafeDownCast(
	  grid->GetFieldData()->GetArray("Boundary_Condition_Number_Of_Steps"));
  if(!NumSteps)	return;
  for(j=0; j<NumSteps->GetValue(0); j++)
  {
	  // obtain step input parameter information
	  char stepParameters[128];
	  sprintf(stepParameters, "Step_%d_%s", j+1, "Input_Parameters");
	  vtkStringArray *stepParametersArray = vtkStringArray::SafeDownCast(
		  grid->GetFieldData()->GetAbstractArray(stepParameters));

	  char subHeading[128];
	  sprintf(subHeading, "Step_%d_%s", j+1, "SubHeading");
	  vtkStringArray *subHeadingArray = vtkStringArray::SafeDownCast(
		  grid->GetFieldData()->GetAbstractArray(subHeading));

	  char stepnum[256];
	  sprintf(stepnum, "%d", j+1);
	  //itoa(j+1, stepnum, 10);
	  os << "**------------------------------------------------------------STEP " << j+1 << std::endl;
	  os << "*STEP";
	  if(stepParametersArray)
	  {
		  os << ", INC="<<stepParametersArray->GetValue(0);
		  if (vtksys::SystemTools::UpperCase(stepParametersArray->GetValue(1)) != "DEFAULT")
		    os << ", AMPLITUDE=" << vtksys::SystemTools::UpperCase(stepParametersArray->GetValue(1));
		  os << ", NLGEOM="<<stepParametersArray->GetValue(2)<<", UNSYMM="<<stepParametersArray->GetValue(3);
	  }
	  os << std::endl;

	  if(subHeadingArray)
	  {
		  if(subHeadingArray->GetValue(0) != "")
			os<<subHeadingArray->GetValue(0)<< std::endl;
	  }

	  if(stepParametersArray)
	  {
		os << "*"<<stepParametersArray->GetValue(4) << std::endl;
		os<<stepParametersArray->GetValue(5)<<std::endl;
	  }
	  else
	  {
	  os<<"**"<<std::endl;
	  os << "*STATIC" << std::endl;
	  }
	  
	  //os<<"**"<<std::endl;
	  //os << "*BOUNDARY, OP=NEW" << std::endl;
	  bool bcFlag = false;
	  vtkFloatArray *floatarray;
	  for(i=0; i<nodesetnamestring->GetNumberOfValues(); i++)
	  {
		  const char* nodesetname =  nodesetnamestring->GetValue(i);

		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Displacement", "X", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if (!bcFlag)
			  {
			    bcFlag = true;
			    os<<"**"<<std::endl;
      	  os << "*BOUNDARY, OP=NEW" << std::endl;
			  }
			  os << nodesetname<<", 1, 1, "<<floatarray->GetValue(0) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Displacement", "Y", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if (!bcFlag)
			  {
			    bcFlag = true;
			    os<<"**"<<std::endl;
      	  os << "*BOUNDARY, OP=NEW" << std::endl;
			  }
			  os << nodesetname<<", 2, 2, "<<floatarray->GetValue(0) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Displacement", "Z", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if (!bcFlag)
			  {
			    bcFlag = true;
			    os<<"**"<<std::endl;
      	  os << "*BOUNDARY, OP=NEW" << std::endl;
			  }
			  os << nodesetname<<", 3, 3, "<<floatarray->GetValue(0) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Rotation", "X", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if (!bcFlag)
			  {
			    bcFlag = true;
			    os<<"**"<<std::endl;
      	  os << "*BOUNDARY, OP=NEW" << std::endl;
			  }
			  os << nodesetname<<", 4, 4, "<< vtkMath::DegreesToRadians() * floatarray->GetValue(0) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Rotation", "Y", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if (!bcFlag)
			  {
			    bcFlag = true;
			    os<<"**"<<std::endl;
      	  os << "*BOUNDARY, OP=NEW" << std::endl;
			  }
			  os << nodesetname<<", 5, 5, "<< vtkMath::DegreesToRadians() * floatarray->GetValue(0) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Rotation", "Z", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if (!bcFlag)
			  {
			    bcFlag = true;
			    os<<"**"<<std::endl;
      	  os << "*BOUNDARY, OP=NEW" << std::endl;
			  }
			  os << nodesetname<<", 6, 6, "<< vtkMath::DegreesToRadians() * floatarray->GetValue(0) << std::endl;
		  }
	  }
	  
	  os << "**" << std::endl;
	  bool cload = false;
	  for(i=0; i<nodesetnamestring->GetNumberOfValues(); i++)
	  {
		  const char* nodesetname =  nodesetnamestring->GetValue(i);
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Force", "X", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  int k;
		  int numPoints = 0;
		  vtkIntArray *intarray = vtkIntArray::SafeDownCast(grid->GetPointData()->GetArray(nodesetname));
		  if(!intarray)	return;
		  for (k=0; k<intarray->GetNumberOfTuples(); k++)
		  {
			  if(intarray->GetValue(k))	numPoints++;
		  }

		  if(floatarray)
		  {
			  if(!cload)
			  {
				  cload = true;
				  os << "*CLOAD" << std::endl;
			  }
			  os << nodesetname<<", 1, "<<floatarray->GetValue(0)/static_cast<float>(numPoints) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Force", "Y", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if(!cload)
			  {
				  cload = true;
				  os << "*CLOAD" << std::endl;
			  }
			  os << nodesetname<<", 2, "<<floatarray->GetValue(0)/static_cast<float>(numPoints) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Force", "Z", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if(!cload)
			  {
				  cload = true;
				  os << "*CLOAD" << std::endl;
			  }
			  os << nodesetname<<", 3, "<<floatarray->GetValue(0)/static_cast<float>(numPoints) << std::endl;
		  }
		  //
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Moment", "X", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if(!cload)
			  {
				  cload = true;
				  os << "*CLOAD" << std::endl;
			  }
			  os << nodesetname<<", 4, "<<floatarray->GetValue(0)/static_cast<float>(numPoints) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Moment", "Y", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if(!cload)
			  {
				  cload = true;
				  os << "*CLOAD" << std::endl;
			  }
			  os << nodesetname<<", 5, "<<floatarray->GetValue(0)/static_cast<float>(numPoints) << std::endl;
		  }
		  //
		  this->ConcatenateStrings("Step", stepnum, nodesetname, "Moment", "Z", Concatenate);
		  floatarray = vtkFloatArray::SafeDownCast(
			  grid->GetFieldData()->GetArray(Concatenate));
		  if(floatarray)
		  {
			  if(!cload)
			  {
				  cload = true;
				  os << "*CLOAD" << std::endl;
			  }
			  os << nodesetname<<", 6, "<<floatarray->GetValue(0)/static_cast<float>(numPoints) << std::endl;
		  }
	  }
	  // print and output statements
	  // loop through all node and element sets
	  os<<"**"<<std::endl;
	  os<<"**------- Output Requests  --->"<<std::endl;
	  os<<"**"<<std::endl;
	  int numNodeSets = nodesetnamestring->GetNumberOfValues();
	  int numElementSets = elementsetnamestring->GetNumberOfValues();
	  // for node sets
	  for (i=0; i<numNodeSets; i++)
	  {
		  const char *nodesetname = nodesetnamestring->GetValue(i);
		  char nodePrint[128];
		  sprintf(nodePrint, "Step_%d_%s_%s_%s", j+1, "Node_Set", nodesetname, "Print");
		  vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
			  grid->GetFieldData()->GetAbstractArray(nodePrint));
		  if ( (stringarray) && (stringarray->GetValue(0) == "YES") )
		  {
			  os<<"*NODE PRINT, NSET="<<nodesetname<<", FREQUENCY="<<stringarray->GetValue(1)<<", SUMMARY="
				  <<stringarray->GetValue(2)<<", TOTALS="<<stringarray->GetValue(3)<<std::endl;
			  this->PrintVariablesOfInterest(stringarray->GetValue(4), os);
		  }
	  }
	  // for element sets
	  for (i=0; i<numElementSets; i++)
	  {
		  const char *elementsetname = elementsetnamestring->GetValue(i);
		  char elementPrint[128];
		  sprintf(elementPrint, "Step_%d_%s_%s_%s", j+1, "Element_Set", elementsetname, "Print");
		  vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
			  grid->GetFieldData()->GetAbstractArray(elementPrint));
		  if ( (stringarray) && (stringarray->GetValue(0) == "YES") )
		  {
			  os<<"*EL PRINT, ELSET="<<elementsetname<<", FREQUENCY="<<stringarray->GetValue(1)<<
				  ", POSITION="<<vtksys::SystemTools::UpperCase(stringarray->GetValue(2)) << 
				  "," << std::endl << "SUMMARY=" <<stringarray->GetValue(3)<<", TOTALS=" << 
				  stringarray->GetValue(4)<<std::endl;
			  this->PrintVariablesOfInterest(stringarray->GetValue(5), os);
		  }
	  }
    os<<"**"<<std::endl;
    
	  // output statements
	  // for node sets
	  bool outputrequest = false;
	  for (i=0; i<numNodeSets; i++)
	  {
		  const char *nodesetname = nodesetnamestring->GetValue(i);
		  char nodePrint[128];
		  sprintf(nodePrint, "Step_%d_%s_%s_%s", j+1, "Node_Set", nodesetname, "Output");
		  vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
			  grid->GetFieldData()->GetAbstractArray(nodePrint));
		  if ( (stringarray) && (stringarray->GetValue(0) == "YES") )
		  {
			  if(!outputrequest)
			  {
				  outputrequest = true;
				  os<<"*OUTPUT,FIELD"<<std::endl;
			  }
			  os<<"*NODE OUTPUT, NSET="<<nodesetname;
			  if (stringarray->GetValue(1) == "ALL")
			  {
			    os << ", VARIABLE=ALL" << std::endl;
			  }
			  else
			  {
			    os << std::endl;
			    this->PrintVariablesOfInterest(stringarray->GetValue(1), os);
			  }
		  }
	  }
	  os<<"**"<<std::endl;
	  // for element sets
	  for (i=0; i<numElementSets; i++)
	  {
		  const char *elementsetname = elementsetnamestring->GetValue(i);
		  char elementPrint[128];
		  sprintf(elementPrint, "Step_%d_%s_%s_%s", j+1, "Element_Set", elementsetname, "Output");
		  vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
			  grid->GetFieldData()->GetAbstractArray(elementPrint));
		  if ( (stringarray) && (stringarray->GetValue(0) == "YES") )
		  {
			  if(!outputrequest)
			  {
				  outputrequest = true;
				  os<<"*OUTPUT,FIELD"<<std::endl;
			  }
			  os << "*ELEMENT OUTPUT,ELSET="<<elementsetname<<", POSITION=" << 
			        vtksys::SystemTools::UpperCase(stringarray->GetValue(1));
			  if (stringarray->GetValue(2) == "ALL")
			  {
			    os << ", VARIABLE=ALL" << std::endl;
			  }
			  else
			  {
			    os << std::endl;
			    this->PrintVariablesOfInterest(stringarray->GetValue(2), os);
			  }  		  
		  }
	  }
	  os<<"**"<<std::endl;
	  os << "*END STEP" << std::endl;
	  os<<"**"<<std::endl;
  }
  os << "**" << std::endl;
}
//---------------------------------------------------------------------------------------------------
void vtkMimxAbaqusFileWriter::WriteNodeSets(std::ostream& os, vtkUnstructuredGrid *grid )
{
	// loop through all node sets
	int i,j;
	vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
		grid->GetFieldData()->GetAbstractArray("Node_Set_Names"));
	int numnodearrays = stringarray->GetNumberOfTuples();

	vtkIntArray *nodenumarray = vtkIntArray::SafeDownCast(
		grid->GetPointData()->GetArray("Node_Numbers"));
	for (i=0; i<numnodearrays; i++)
	{
		vtkIntArray *intarray = vtkIntArray::SafeDownCast(grid->GetPointData()->GetArray(stringarray->GetValue(i)));
		//if(intarray && strcmp(grid->GetPointData()->GetArray(i)->GetName(), "Node_Numbers")
		//	&& intarray->GetNumberOfComponents() == 1)
		//{
			std::list<int> nodenum;
			nodenum.clear();
			// add the nodes belonging to the node set to the list
			for (j=0; j<intarray->GetNumberOfTuples(); j++)
			{
				if(intarray->GetValue(j))
				{
					nodenum.push_back(nodenumarray->GetValue(j));
				}
			}
			if(!nodenum.size())
			{
				continue;
			}
			// sort the entries in ascending order
			nodenum.sort();			
			vtkIdList *storearray = vtkIdList::New();
			do 
			{
				storearray->InsertUniqueId(nodenum.front());
				nodenum.pop_front();
			} while(nodenum.size());
	
			int numberOfNodes = storearray->GetNumberOfIds();
			const char *arrayname = stringarray->GetValue(i);
			os << "*NSET, NSET=" << arrayname << ", GENERATE" << std::endl;
			// loop through all the nodes in the node set
			int firstnode, lastnode, currnode;
			if(numberOfNodes > 1)
			{
				for (j=0; j<numberOfNodes; j++)
				{
					if(j == 0)
					{
						firstnode = storearray->GetId(j);
						lastnode = firstnode;
						currnode = storearray->GetId(j+1);
						j++;
					}
					if(currnode != lastnode+1)
					{
						os<<firstnode<<", "<<lastnode<<std::endl;
						firstnode = currnode;
						lastnode = currnode;
						if(j < numberOfNodes-1)
						{
							currnode = storearray->GetId(j+1);
						}
						if (j == numberOfNodes-1)
						{
							os<<firstnode<<", "<<lastnode<<std::endl;
						}
					}
					else
					{
						lastnode = currnode;
						if(j < numberOfNodes-1)
						{
							currnode = storearray->GetId(j+1);
						}
						if(j == numberOfNodes-1)
						{
							os<<firstnode<<", "<<lastnode<<std::endl;
						}
					}
				}
			}
			else{
				firstnode = storearray->GetId(0);
				lastnode = storearray->GetId(0);
				os<<firstnode<<", "<<lastnode<<std::endl;
			}
		//}
	}
}
//---------------------------------------------------------------------------------------------------
void vtkMimxAbaqusFileWriter::WriteRepeatingMaterialProperties(
	vtkIntArray *ElementIds, vtkDoubleArray *MatProp, 
	vtkUnstructuredGrid *grid , std::ostream& os, 
	const char *ElementSetName, double PoissonsRatio)
{
	vtkIntArray *elementArray = vtkIntArray::SafeDownCast( 
		grid->GetCellData()->GetArray("Element_Numbers") );
	int i, j;
	//int numEle = elementArray->GetNumberOfTuples();

	//list all the unique material properties
	vtkDoubleArray *UniqueMatProp = vtkDoubleArray::New();

	for (i=0; i<MatProp->GetNumberOfTuples(); i++)
	{
		if(MatProp->GetValue(i) >= 0.0)
		{
			bool status = true;
			for (j=0; j<UniqueMatProp->GetNumberOfTuples(); j++)
			{
				if(UniqueMatProp->GetValue(j) == MatProp->GetValue(i))
				{
					status = false;
					break;
				}
			}
			if(status)	
			{
				UniqueMatProp->InsertNextValue(MatProp->GetValue(i));
			}
		}
	}
	// sort the material properties in ascending order
	//list<double> matprop;
	//matprop.clear();
	//for(i=0; i<UniqueMatProp->GetNumberOfTuples(); i++)
	//{
	//	matprop.push_back(UniqueMatProp->GetValue(i));
	//}
	// based on unique material properties generate element sets.
	vtkSortDataArray::Sort(UniqueMatProp);
	for (i=0; i<UniqueMatProp->GetNumberOfTuples(); i++)
	{
		vtkIntArray *intarray = vtkIntArray::New();
		for (j=0; j<MatProp->GetNumberOfTuples(); j++)
		{
			if (UniqueMatProp->GetValue(i) == MatProp->GetValue(j))
			{
				intarray->InsertNextValue(elementArray->GetValue(j));
			}
		}
		if(intarray->GetNumberOfTuples() > 0)
		{
			std::list<int> elementnum;
			elementnum.clear();
			// add the nodes belonging to the node set to the list
			for (j=0; j<intarray->GetNumberOfTuples(); j++)
			{
				elementnum.push_back(intarray->GetValue(j));
			}
			if (!elementnum.size())
			{
				return;
			}
			// sort the entries in ascending order
			elementnum.sort();			
			vtkIdList *storearray = vtkIdList::New();
			do 
			{
				storearray->InsertUniqueId(elementnum.front());
				elementnum.pop_front();
			} while(elementnum.size());

			int numberOfElements = storearray->GetNumberOfIds();
			os << "*ELSET, ELSET=" << ElementSetName<<"_"<< i+1 << ", GENERATE" << std::endl;
			// loop through all the nodes in the node set
			int firstelement, lastelement, currelement;
			if(numberOfElements > 1)
			{
				for (j=0; j<numberOfElements; j++)
				{
					if(j == 0)
					{
						firstelement = storearray->GetId(j);
						lastelement = firstelement;
						currelement = storearray->GetId(j+1);
						j++;
					}
					if(currelement != lastelement+1)
					{
						os<<firstelement<<", "<<lastelement<<std::endl;
						firstelement = currelement;
						lastelement = currelement;
						if(j < numberOfElements-1)
						{
							currelement = storearray->GetId(j+1);
						}
						if (j == numberOfElements-1)
						{
							os<<firstelement<<", "<<lastelement<<std::endl;
						}
					}
					else
					{
						lastelement = currelement;
						if(j < numberOfElements-1)
						{
							currelement = storearray->GetId(j+1);
						}
						if(j == numberOfElements-1)
						{
							os<<firstelement<<", "<<lastelement<<std::endl;
						}
					}
				}
			}
			else{
				firstelement = storearray->GetId(0);
				lastelement = storearray->GetId(0);
				os<<firstelement<<", "<<lastelement<<std::endl;
			}
			storearray->Delete();
			/*
			os << "*SOLID SECTION, ELSET=" << ElementSetName<<"_"<<i 
				<< ", MATERIAL=" << ElementSetName<<"_"<<i << std::endl;
			os << "*MATERIAL, NAME=" << ElementSetName<<"_"<<i << std::endl; 
			os << "*ELASTIC" << std::endl;
			os << UniqueMatProp->GetValue(i) << ", " << PoissonsRatio << std::endl;	
      */
		}
		intarray->Delete();
	}
	
	os << "**==============================================" << std::endl;
	// Write out the Solid Section Definitions
	for (i=0; i<UniqueMatProp->GetNumberOfTuples(); i++)
	{
		vtkIntArray *intarray = vtkIntArray::New();
		for (j=0; j<MatProp->GetNumberOfTuples(); j++)
		{
			if (UniqueMatProp->GetValue(i) == MatProp->GetValue(j))
			{
				intarray->InsertNextValue(elementArray->GetValue(j));
			}
		}
		if (intarray->GetNumberOfTuples() > 0)
		{
  		os << "*SOLID SECTION, ELSET=" << ElementSetName<<"_"<<i+1 
				 << ", MATERIAL=" << ElementSetName<<"_"<<i+1 << std::endl;
		}
		intarray->Delete();
  }
  
  os << "**==============================================" << std::endl;
  // Write out the Material Section Definitions
	for (i=0; i<UniqueMatProp->GetNumberOfTuples(); i++)
	{
		vtkIntArray *intarray = vtkIntArray::New();
		for (j=0; j<MatProp->GetNumberOfTuples(); j++)
		{
			if (UniqueMatProp->GetValue(i) == MatProp->GetValue(j))
			{
				intarray->InsertNextValue(elementArray->GetValue(j));
			}
		}
		if (intarray->GetNumberOfTuples() > 0)
		{
  		os << "*MATERIAL, NAME=" << ElementSetName<<"_"<<i+1 << std::endl; 
			os << "*ELASTIC" << std::endl;
			if (this->Precision != 0) 
			{
  			os.setf(std::ios_base::fixed,std::ios::floatfield);
  			os.precision( this->Precision );
			}
			os << UniqueMatProp->GetValue(i) << ", " << PoissonsRatio << std::endl;	
			if (this->Precision != 0) 
			{
  			os.unsetf(std::ios::floatfield);
  			os.precision( 0 );
			}	
		}
		intarray->Delete();
  }
  
	//
	UniqueMatProp->Delete();
}
//---------------------------------------------------------------------------------------------------
void vtkMimxAbaqusFileWriter::WriteFooter( std::ostream& os )
{
 /* os << "**" << std::endl;
  os << "**" << std::endl;
  os << "*ENDSTEP" << std::endl;*/
  os << "**=========================================================================" << std::endl;
  os << "**                         END OF ABAQUS INPUT DECK                      **" << std::endl;
  os << "**=========================================================================" << std::endl;
}
//---------------------------------------------------------------------------------------------------
void vtkMimxAbaqusFileWriter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------------------------------
void vtkMimxAbaqusFileWriter::ConcatenateStrings(const char* Step, const char* Num, 
		const char* NodeSetName, const char* Type, const char* Direction, char *Name)
{
	strcpy(Name, Step);
	strcat(Name, "_");
	strcat(Name,Num);
	strcat(Name, "_");
	strcat(Name, NodeSetName);
	strcat(Name, "_");
	strcat(Name,Type);
	strcat(Name, "_");
	strcat(Name, Direction);
}
//-----------------------------------------------------------------------------------------------------
void vtkMimxAbaqusFileWriter::PrintVariablesOfInterest(vtkStdString &variable, std::ostream &os)
{
	if (variable.find(",") != std::string::npos)
	{
		int pos = 0;
		std::string substr;
		while(variable.find(",", pos) != std::string::npos)
		{
			int currpos = variable.find(",", pos);
			substr = variable.substr(pos, currpos-pos);
			vtksys::SystemTools::ReplaceString(substr, " ", "");
			os << vtksys::SystemTools::UpperCase(substr) << std::endl;
			pos = currpos +1;
		}
		substr = variable.substr(pos, variable.size());
		vtksys::SystemTools::ReplaceString(substr, " ", "");
		os << vtksys::SystemTools::UpperCase(substr) << std::endl;
	}
	else
	{
		if (variable != "")
		{

			vtksys::SystemTools::ReplaceString(variable, " ", "");
			os << vtksys::SystemTools::UpperCase(variable) << std::endl;
		}
	}
}
//------------------------------------------------------------------------------------------------------
