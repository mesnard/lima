#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_BOW_EXPORTING)

########### next target ###############

SET(lima-lp-bagofwords_LIB_SRCS
   bowComplexToken.cpp
   bowDocument.cpp
   bowDocumentST.cpp
   bowNamedEntity.cpp
   bowTerm.cpp
   bowText.cpp
   bowToken.cpp
   bowXMLReader.cpp
   bowXMLWriter.cpp
   indexElement.cpp
   indexElementIterator.cpp
   defaultIdGenerator.cpp
   bowTokenIterator.cpp
   BoWRelation.cpp
   bowBinaryReaderWriter.cpp
   TextWriterBoWDocumentHandler.cpp
   BinaryWriterBoWDocumentHandler.cpp
)

add_library(lima-lp-bagofwords SHARED ${lima-lp-bagofwords_LIB_SRCS})

target_link_libraries(lima-lp-bagofwords lima-lp-misc lima-lp-propertycode lima-common-data lima-common-mediaticdata lima-common-fsaaccess ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES})

set_target_properties(lima-lp-bagofwords PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-bagofwords DESTINATION lib)


########### install files ###############
install(FILES
    AbstractBoWDocumentHandler.h
    bowDocument.h
    bowTerm.h
    bowXMLReader.h
    indexElementIterator.h
    BinaryWriterBoWDocumentHandler.h
    bowDocumentST.h
    bowText.h
    bowXMLWriter.h
    TextWriterBoWDocumentHandler.h
    bowBinaryReaderWriter.h
    bowNamedEntity.h
    bowToken.h
#     defaultIdGenerator.h
    bowComplexToken.h
    BoWRelation.h
    bowTokenIterator.h
    indexElement.h
  DESTINATION include/linguisticProcessing/common/BagOfWords)



add_subdirectory(tests)
