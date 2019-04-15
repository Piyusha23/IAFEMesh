PROJECT ( MIMXIO )

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/IO ${IAFEMMESH_BINARY_DIR}/Code/IO )

SET ( IO_SRCS 
    vtkMimxAbaqusFileWriter.cxx
    vtkMimxIntervertebralDiscAbaqusWriter.cxx
    vtkPluginFilterWatcher.cxx
    ModuleProcessInformation.cxx
    itkMimxSceneXMLFile.cxx
)

ADD_LIBRARY( mimxIO ${IO_SRCS} )
INSTALL_TARGETS(/lib/IaFeMesh mimxIO)
INSTALL_FILES(/include/IaFeMesh/IO "(\\.h|\\.txx)$")
