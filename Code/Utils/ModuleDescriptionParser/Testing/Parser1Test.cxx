/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/ModuleDescriptionParser/Testing/Parser1Test.cxx $
  Date:      $Date: 2012/12/07 19:08:59 $
  Version:   $Revision: 1.1.1.1 $

==========================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "ModuleDescriptionParser.h"
#include "ModuleDescription.h"

int main (int argc, char *argv[])
{
  if (argc < 2)
    {
    std::cout << "Usage: ParserTest1 xml_file" << std::endl;
    return EXIT_FAILURE;
    }

  std::ifstream fin(argv[1],std::ios::in);
  if (fin.fail())
    {
    std::cerr << argv[0] << ": Cannot open " << argv[1] << " for input" << std::endl;
    perror(argv[0]);
    return EXIT_FAILURE;
    }

  // Get the length of the file
  fin.seekg (0, std::ios::end);
  size_t len = fin.tellg();
  fin.seekg (0, std::ios::beg);
  char * XML = new char[len+1];
  fin.read (XML, len);

  ModuleDescription module;
  ModuleDescriptionParser parser;
  if (parser.Parse(XML, module))
    {
    return EXIT_FAILURE;
    }

  std::cout << module;

  return EXIT_SUCCESS;
}
