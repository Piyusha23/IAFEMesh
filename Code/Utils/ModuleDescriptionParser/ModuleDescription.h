/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/ModuleDescriptionParser/ModuleDescription.h $
  Date:      $Date: 2012/12/07 19:08:59 $
  Version:   $Revision: 1.1.1.1 $

==========================================================================*/

#ifndef __ModuleDescription_h
#define __ModuleDescription_h

#include "ModuleDescriptionParserWin32Header.h"

#include "ModuleParameterGroup.h"

#include "ModuleProcessInformation.h"
#include "ModuleLogo.h"

#include <string>
#include <vector>

class ModuleDescriptionParser_EXPORT ModuleDescription
{
public:
  ModuleDescription();
  ModuleDescription(const ModuleDescription &md);

  void operator=(const ModuleDescription &md);

  void SetCategory(const std::string &cat)
  {
    this->Category = cat;
  }

  const std::string&  GetCategory() const
  {
    return this->Category;
  }
  
  void SetTitle(const std::string &title)
  {
    this->Title = title;
  }

  const std::string& GetTitle() const
  {
    return this->Title;
  }

  void SetDescription(const std::string &description)
  {
    this->Description = description;
  }

  const std::string& GetDescription() const
  {
    return this->Description;
  }

  void SetVersion(const std::string &version)
  {
    this->Version = version;
  }

  const std::string& GetVersion() const
  {
    return this->Version;
  }

  void SetDocumentationURL(const std::string &documentationURL)
  {
    this->DocumentationURL = documentationURL;
  }

  const std::string& GetDocumentationURL() const
  {
    return this->DocumentationURL;
  }

  void SetLicense(const std::string &license)
  {
    this->License = license;
  }

  const std::string& GetLicense() const
  {
    return this->License;
  }

  void SetAcknowledgements(const std::string &acknowledgements)
  {
    this->Acknowledgements = acknowledgements;
  }

  const std::string& GetAcknowledgements() const
  {
    return this->Acknowledgements;
  }

  void SetContributor(const std::string &contributor)
  {
    this->Contributor = contributor;
  }

  const std::string& GetContributor() const
  {
    return this->Contributor;
  }

  // Set the type of module: Unknown, SharedObjectModule, CommandLineModule
  void SetType(const std::string &type)
  {
    if (type == "SharedObjectModule" || type == "CommandLineModule")
      {
      this->Type = type;
      }
    else
      {
      this->Type = "Unknown";
      }
  }

  // Get the type of the module: Unknown, SharedObjectModule, CommandLineModule
  const std::string& GetType() const
  {
    return this->Type;
  }
  
  // Set the target for the module.  This is the entry point for a
  // shared object module and the full command (with path) for an executable.
  void SetTarget(const std::string &target)
  {
    this->Target = target;
  }

  // Get the target for the module.  This is the entry point for a
  // shared object module and the full command (with path) for an executable.
  const std::string& GetTarget() const
  {
    return this->Target;
  }

  // Set the location for the module.  This is path to the file (shared
  // object or executable) for the module.
  void SetLocation(const std::string &target)
  {
    this->Location = target;
  }

  // Get the location for the module.  This is path to the file (shared
  // object or executable) for the module.
  const std::string& GetLocation() const
  {
    return this->Location;
  }
  
  void SetLogo(const ModuleLogo& logo);
  const ModuleLogo& GetLogo() const;
  
  void AddParameterGroup(const ModuleParameterGroup &group)
  {
    this->ParameterGroups.push_back(group);
  }

  const std::vector<ModuleParameterGroup>& GetParameterGroups() const
  {
    return this->ParameterGroups;
  }

  std::vector<ModuleParameterGroup>& GetParameterGroups()
  {
    return this->ParameterGroups;
  }
  
  void SetParameterGroups(const std::vector<ModuleParameterGroup>& groups)
  {
      this->ParameterGroups = groups;
  }

  bool HasParameter(const std::string& name) const;

  bool SetParameterDefaultValue(const std::string& name,
                                const std::string& value);

  std::string GetParameterDefaultValue(const std::string& name) const;

  const ModuleProcessInformation* GetProcessInformation() const
    {return &ProcessInformation;}
  
  ModuleProcessInformation* GetProcessInformation()
    {return &ProcessInformation;}
  
  
private:
  std::string Type;
  std::string Category;
  std::string Title;
  std::string Location;
  std::string Description;
  std::string Version;
  std::string DocumentationURL;
  std::string License;
  std::string Acknowledgements;
  std::string Contributor;
  std::string Target;
  std::vector<ModuleParameterGroup> ParameterGroups;  

  ModuleProcessInformation ProcessInformation;
  ModuleLogo Logo;
};

ModuleDescriptionParser_EXPORT std::ostream & operator<<(std::ostream &os, const ModuleDescription &module);

#endif
