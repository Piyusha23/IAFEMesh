/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: ApplicationBase.txx,v $
Language:  C++
Date:      $Date: 2012/12/07 19:08:59 $
Version:   $Revision: 1.1.1.1 $

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _ApplicationBase_txx
#define _ApplicationBase_txx

#include "ApplicationBase.h"

namespace itk
{

    template < typename TParser, typename TPreprocessor, typename TRegistrator> 
        ApplicationBase< TParser,TPreprocessor, TRegistrator>
        ::ApplicationBase()
            {
            m_Parser       = ParserType::New();
            m_Preprocessor = PreprocessorType::New();
            m_Registrator  = RegistratorType::New();
            }


    template < typename TParser, typename TPreprocessor, typename TRegistrator>
        void
        ApplicationBase< TParser,TPreprocessor, TRegistrator>
        ::Execute()
            {

            /**************************
             * Parse input
             **************************/
            std::cout << "Parsing input ... " << std::endl;

            try
                {
                this->InitializeParser();
                m_Parser->Execute();
                }
            catch( itk::ExceptionObject& err)
                {
                std::cout << "Caught an ITK exception: " << std::endl;
                std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
                throw err;
                }
            catch(...)
                {
                std::cout << "Error occurred during input parsing." << std::endl;
                throw;
                }

            /**************************
             * Preprocess the images before registration
             **************************/

            std::cout << "Preprocess the images ... " << std::endl;

            try
                {
                this->InitializePreprocessor();
                m_Preprocessor->Execute();
                }
            catch( itk::ExceptionObject& err)
                {
                std::cout << "Caught an ITK exception: " << std::endl;
                std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
                throw err;
                }
            catch(...)
                {
                std::cout << "Error occured during preprocessing." << std::endl;
                throw;
                }


            /**************************
             * Registered the processed images
             **************************/
            std::cout << "Register the images ... " << std::endl;

            try
                {
                this->InitializeRegistrator();
                m_Preprocessor = NULL;
                m_Parser = NULL;
                m_Registrator->Execute();
                }
            catch( itk::ExceptionObject& err)
                {
                std::cout << "Caught an ITK exception: " << std::endl;
                std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
                throw err;
                }
            catch(...)
                {
                std::cout << "Error occured during registration" << std::endl;
                throw;
                }
            }

} // namespace itk

#endif

