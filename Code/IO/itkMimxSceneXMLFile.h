/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMimxSceneXMLFile.h,v $
  Language:  C++
  Date:      $Date: 2013/01/15 23:05:54 $
  Version:   $1.0$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMimxSceneXMLFile_h
#define __itkMimxSceneXMLFile_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkXMLFile.h"
namespace itk
{

class MimxSceneInfo
{
public:
  std::string m_FileName;
  bool        m_Binary;
  std::string m_ObjectType;
  std::string m_FileType;
};

/** \class MimxSceneXMLFileReader
 * 
 * Reads an XML-format file containing an IA-FEMesh Scene file
 */
class MimxSceneXMLFileReader : public XMLReader<std::string>
{
public:
  /** Standard typedefs */ 
  typedef MimxSceneXMLFileReader                 Self;
  typedef XMLReader<std::string>                 Superclass;
  typedef SmartPointer<Self>                     Pointer;


  /** Run-time type information (and related methods). */
  itkTypeMacro(MimxSceneXMLFileReader, XMLReader);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
public:
  /** Determine if a file can be read */
  virtual int CanReadFile(const char* name);
  
  /** Get Number of Objects in Scene */
  unsigned int GetNumberOfObjects();
  
  /** Get Object Scene Information */
  bool GetSceneObjectInfoByIndex(unsigned int index, std::string &type, std::string &fileName, std::string &fileFormat, bool &binary);
  
protected:
  MimxSceneXMLFileReader() {};
  virtual ~MimxSceneXMLFileReader() {};

  virtual void StartElement(const char * name,const char **atts);
  virtual void EndElement(const char *name);
  virtual void CharacterDataHandler(const char *inData, int inLength);
private:
  MimxSceneXMLFileReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<itk::MimxSceneInfo*> m_SceneData;
  itk::MimxSceneInfo*            m_CurrentObject;
  std::string               m_Mode;
  std::string               m_CurrentData;
};



/** \class MimxSceneXMLFileWriter
 * 
 * Writes an XML-format file containing an IA-FEMesh scene
 */
class MimxSceneXMLFileWriter : public XMLWriterBase<std::string>
{
public:
  /** standard typedefs */
  typedef XMLWriterBase<std::string> Superclass;
  typedef MimxSceneXMLFileWriter Self;
  typedef SmartPointer<Self>                     Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MimxSceneXMLFileWriter,XMLWriterBase<std::string>);
  
  /** Test whether a file is writable. */
  virtual int CanWriteFile(const char* name);
  
  /** Actually write out the file in question */
  virtual int WriteFile();
  
  /** Add filenames for the scene */
  virtual void AddSurfaceFile(const char *name, bool binary, const char *type);
  virtual void AddFeatureFile(const char *name, bool binary, const char *type);
  virtual void AddBuildignBlockFile(const char *name, bool binary, const char *type);
  virtual void AddMeshFile(const char *name, bool binary, const char *type);
  virtual void AddImageFile(const char *name, bool binary, const char *type);
  
  virtual void Initialize()
  {
	  m_SceneData.clear();
  }

  std::vector<itk::MimxSceneInfo*> GetSceneData()
  {
	  return m_SceneData;
  }
protected:
  MimxSceneXMLFileWriter() {};
  virtual ~MimxSceneXMLFileWriter() {};
private:
  MimxSceneXMLFileWriter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  std::vector<itk::MimxSceneInfo*> m_SceneData;
};

}
#endif
