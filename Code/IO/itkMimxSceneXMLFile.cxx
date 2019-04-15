/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMimxSceneXMLFile.cxx,v $
  Language:  C++
  Date:      $Date: 2013/01/15 23:05:54 $
  Version:   $1.0$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkMimxSceneXMLFile_txx
#define _itkMimxSceneXMLFile_txx

#include <itkMimxSceneXMLFile.h>
#include <itksys/SystemTools.hxx>
#include "itkMetaDataObject.h"
#include "itkIOCommon.h"
#define RAISE_EXCEPTION(s) \
        { ExceptionObject exception(__FILE__, __LINE__); \
        exception.SetDescription(s); \
        throw exception; }


namespace itk
{

int
MimxSceneXMLFileReader::
CanReadFile(const char *name)
{
  if(!itksys::SystemTools::FileExists(name) ||
     itksys::SystemTools::FileIsDirectory(name) ||
     itksys::SystemTools::FileLength(name) == 0)
    return 0;
  return 1;
}

void 
MimxSceneXMLFileReader::
StartElement(const char * name,const char ** itkNotUsed(atts))
{
#if 0
  std::cout << "<" << name << " ";
  if(atts != 0)
    {
    for(int i = 0; atts[i] != 0; i++)
      std::cout << atts[i] << " ";
    }
  std::cout << ">" << std::endl;
#endif  
  if(itksys::SystemTools::Strucmp(name,"Scene") == 0) 
    {
    m_Mode = "Scene";
    }
  else if(itksys::SystemTools::Strucmp(name,"Object") == 0) 
    {
		m_CurrentObject = new itk::MimxSceneInfo;
    m_CurrentObject->m_FileName = "";
    m_CurrentObject->m_Binary = false;
    m_CurrentObject->m_ObjectType = "";
    m_CurrentObject->m_FileType = "";
    m_Mode = "Object";
    }
  else if(itksys::SystemTools::Strucmp(name,"Type") == 0) 
    {
    m_Mode = "Type";
    }
  else if(itksys::SystemTools::Strucmp(name,"Format") == 0) 
    {
    m_Mode = "Format";
    }
  else if(itksys::SystemTools::Strucmp(name,"Binary") == 0) 
    {
    m_Mode = "Binary";
    }
  else if(itksys::SystemTools::Strucmp(name,"Filename") == 0) 
    {
    m_Mode = "Filename";
    }
  
}

void 
MimxSceneXMLFileReader::
EndElement(const char *name)
{
#if 0
  std::cout << "</" << name << ">" << std::endl;
#endif
  if(itksys::SystemTools::Strucmp(name,"Scene") == 0) 
    {
    
    }
  else if(itksys::SystemTools::Strucmp(name,"Object") == 0)
    {
    m_SceneData.push_back(m_CurrentObject);
    }
  else if(itksys::SystemTools::Strucmp(name,"Type") == 0)
    {
    m_CurrentObject->m_ObjectType = m_CurrentData;
    }
  else if(itksys::SystemTools::Strucmp(name,"Format") == 0)
    {
    m_CurrentObject->m_FileType = m_CurrentData;
    }
  else if(itksys::SystemTools::Strucmp(name,"Binary") == 0)
    {
    if ((m_CurrentData == "1") || (m_CurrentData == "true"))
      {
      m_CurrentObject->m_Binary = true;
      }
    else
      {
      m_CurrentObject->m_Binary = false;
      }
    }
  else if(itksys::SystemTools::Strucmp(name,"Filename") == 0)
    {
     m_CurrentObject->m_FileName = m_CurrentData;
    }
}

void 
MimxSceneXMLFileReader::
CharacterDataHandler(const char *inData, int inLength)
{
#if 0
  for(int i = 0; i < inLength; i++)
    std::cout << inData[i];
  std::cout << std::endl;
#endif
  m_CurrentData = "";
  for(int i = 0; i < inLength; i++)
    m_CurrentData = m_CurrentData + inData[i];
    
  //std::cout << "Mode " << m_Mode << std::endl;
  //std::cout << "Value " << m_CurrentData << std::endl;
}

unsigned int 
MimxSceneXMLFileReader::
GetNumberOfObjects()
{
  return m_SceneData.size();
}


bool 
MimxSceneXMLFileReader::
GetSceneObjectInfoByIndex(unsigned int index, std::string &type, std::string &fileName, std::string &fileFormat, bool &binary)
{
  if (index >= m_SceneData.size())
  {
    return false;
  }
  
  std::vector<itk::MimxSceneInfo*>::iterator it;
  unsigned int currentIndex = 0;
  for (it=m_SceneData.begin();it!=m_SceneData.end();it++)
  {
    if (currentIndex == index)
    {
      type = (*it)->m_ObjectType;
      fileName = (*it)->m_FileName;
      fileFormat = (*it)->m_FileType;
      binary = (*it)->m_Binary;
      return true;
    }
    currentIndex++;
  }
  
  return false;
}


void
MimxSceneXMLFileWriter::
AddSurfaceFile(const char *name, bool binary, const char *type)
{
	itk::MimxSceneInfo *feature = new itk::MimxSceneInfo;
  feature->m_FileName = name;
  feature->m_Binary = binary;
  feature->m_ObjectType = "Surface";
  feature->m_FileType = type;
  m_SceneData.push_back(feature);
}

void
MimxSceneXMLFileWriter::
AddFeatureFile(const char *name, bool binary, const char *type)
{
	itk::MimxSceneInfo *feature = new itk::MimxSceneInfo;
  feature->m_FileName = name;
  feature->m_Binary = binary;
  feature->m_ObjectType = "Feature";
  feature->m_FileType = type;
  m_SceneData.push_back(feature);
}

void
MimxSceneXMLFileWriter::
AddBuildignBlockFile(const char *name, bool binary, const char *type)
{
	itk::MimxSceneInfo *feature = new itk::MimxSceneInfo;
  feature->m_FileName = name;
  feature->m_Binary = binary;
  feature->m_ObjectType = "BuildingBlock";
  feature->m_FileType = type;
  m_SceneData.push_back(feature);
}

void
MimxSceneXMLFileWriter::
AddMeshFile(const char *name, bool binary, const char *type)
{
	itk::MimxSceneInfo *feature = new itk::MimxSceneInfo;
  feature->m_FileName = name;
  feature->m_Binary = binary;
  feature->m_ObjectType = "Mesh";
  feature->m_FileType = type;
  m_SceneData.push_back(feature);
}

void
MimxSceneXMLFileWriter::
AddImageFile(const char *name, bool binary, const char *type)
{
	itk::MimxSceneInfo *feature = new itk::MimxSceneInfo;
  feature->m_FileName = name;
  feature->m_Binary = binary;
  feature->m_ObjectType = "Image";
  feature->m_FileType = type;
  m_SceneData.push_back(feature);
}



int
MimxSceneXMLFileWriter::
CanWriteFile(const char * itkNotUsed(name))
{
  return true;                  // not sure what else to say
}



int
MimxSceneXMLFileWriter::
WriteFile()
{
  //
  // sanity checks
  if(m_SceneData.size() == 0)
    {
    std::string errmsg("No Scene to Write");
    RAISE_EXCEPTION(errmsg);
    }
  if(m_Filename.length() == 0)
    {
    std::string errmsg("No filename given");
    RAISE_EXCEPTION(errmsg);
    }
  std::ofstream output(m_Filename.c_str());
  if(output.fail())
    {
    std::string errmsg("Can't Open ");
    errmsg += m_Filename;
    RAISE_EXCEPTION(errmsg);
    }
  
  WriteStartElement("?xml version=\"1.0\"?",output);
  output << std::endl;
  WriteStartElement("!DOCTYPE IAFEMESH-SCENE",output);
  output << std::endl;

  //
  // Write out metadata

  WriteStartElement("Scene",output);
  output << std::endl << std::endl << "  ";

  std::vector<itk::MimxSceneInfo*>::iterator it;

  for (it=m_SceneData.begin();it!=m_SceneData.end();it++)
  {
    WriteStartElement("Object",output);
    output << std::endl << "    ";
    
    WriteStartElement("Type",output);
    output << (*it)->m_ObjectType;
    WriteEndElement("Type",output);
    output << std::endl << "    ";
    
    WriteStartElement("Format",output);
    output << (*it)->m_FileType;
    WriteEndElement("Format",output);
    output << std::endl << "    ";
    
    WriteStartElement("Binary",output);
    output << (*it)->m_Binary;
    WriteEndElement("Binary",output);
    output << std::endl << "    ";
    
    WriteStartElement("Filename",output);
    output << (*it)->m_FileName;
    WriteEndElement("Filename",output);
    output << std::endl << "  ";
    
    WriteEndElement("Object",output);
    output << std::endl << "  ";


  }
  output << std::endl;
  WriteEndElement("Scene",output);
  output << std::endl;
  output.close();
  
  return 0;
}

}

#endif
