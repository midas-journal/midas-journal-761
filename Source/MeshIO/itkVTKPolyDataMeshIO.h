/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVTKPolyDataMeshIO.h,v $
  Language:  C++
  Date:      $Date: 2010-10-12 21:52:40 $
  Version:   $Revision: 0.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVTKPolyDataMeshIO_h
#define __itkVTKPolyDataMeshIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkByteSwapper.h"
#include "itkMetaDataObject.h"
#include "itkMeshIOBase.h"

#include <fstream>
#include <vector>
#include <itksys/SystemTools.hxx>

namespace itk
{
/** \class VTKPolyDataMeshIO
 * \brief
 * Reads a vtkPolyData file and create an itk::Mesh<> or itk::QuadEdgeMesh<>
 *
 * \author wanlin zhu   wanl.zhu@gmail.com
 */
class ITK_EXPORT VTKPolyDataMeshIO:public MeshIOBase
{
public:
  /** Standard "Self" typedef. */
  typedef VTKPolyDataMeshIO          Self;
  typedef MeshIOBase                 Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef Superclass::SizeValueType PointIdentifier;
  typedef Superclass::SizeValueType CellIdentifier;

  typedef std::string               StringType;
  typedef std::vector< StringType > StringVectorType;
  typedef std::stringstream         StringStreamType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VTKPolyDataMeshIO, MeshIOBase);

  /**-------- This part of the interfaces deals with reading data. ----- */

  /** Determine if the file can be read with this MeshIO implementation.
  * \param FileNameToRead The name of the file to test for reading.
  * \post Sets classes MeshIOBase::m_FileName variable to be FileNameToWrite
  * \return Returns true if this MeshIO can read the file specified.
  */
  virtual bool CanReadFile(const char *FileNameToRead);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadMeshInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void ReadPoints(void *buffer);

  virtual void ReadCells(void *buffer);

  virtual void ReadPointData(void *buffer);

  virtual void ReadCellData(void *buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */
  /** Determine if the file can be written with this MeshIO implementation.
   * \param FileNameToWrite The name of the file to test for writing.
   * \post Sets classes MeshIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this MeshIO can write the file specified.
   */
  virtual bool CanWriteFile(const char *FileNameToWrite);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteMeshInformation();

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void WritePoints(void *buffer);

  virtual void WriteCells(void *buffer);

  virtual void WritePointData(void *buffer);

  virtual void WriteCellData(void *buffer);

  virtual void Write(){}
protected:
  VTKPolyDataMeshIO();
  virtual ~VTKPolyDataMeshIO() {}

  void PrintSelf(std::ostream & os, Indent indent) const;

  template< typename T >
  void UpdateCellInformation(T *buffer)
    {
    unsigned int numberOfVertices = 0;
    unsigned int numberOfVertexIndices = 0;
    unsigned int numberOfLines = 0;
    unsigned int numberOfLineIndices = 0;
    unsigned int numberOfPolygons = 0;
    unsigned int numberOfPolygonIndices = 0;

    unsigned long index = 0;

    for ( unsigned long ii = 0; ii < this->m_NumberOfCells; ii++ )
      {
      MeshIOBase::CellGeometryType cellType = static_cast< MeshIOBase::CellGeometryType >( static_cast< int >( buffer[index++] ) );
      unsigned int nn = static_cast< unsigned int >( buffer[index++] );
      switch ( cellType )
        {
        case VERTEX_CELL:
          numberOfVertices++;
          numberOfVertexIndices += nn + 1;
          break;
        case POLYLINE_CELL:
          numberOfLines++;
          numberOfLineIndices += nn + 1;
          break;
        case TRIANGLE_CELL:
          numberOfPolygons++;
          numberOfPolygonIndices += nn + 1;
          break;
        case POLYGON_CELL:
          numberOfPolygons++;
          numberOfPolygonIndices += nn + 1;
          break;
        default:
          itkExceptionMacro(<< "Currently we dont support this cell type");
        }

      index += nn;
      }

    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    EncapsulateMetaData< unsigned int >(metaDic, "numberOfVertices", numberOfVertices);
    EncapsulateMetaData< unsigned int >(metaDic, "numberOfVertexIndices", numberOfVertexIndices);
    EncapsulateMetaData< unsigned int >(metaDic, "numberOfLines", numberOfLines);
    EncapsulateMetaData< unsigned int >(metaDic, "numberOfLineIndices", numberOfLineIndices);
    EncapsulateMetaData< unsigned int >(metaDic, "numberOfPolygons", numberOfPolygons);
    EncapsulateMetaData< unsigned int >(metaDic, "numberOfPolygonIndices", numberOfPolygonIndices);
    return;
    }

  template< typename T >
  void ReadPointsBufferAsASCII(std::ifstream & inputFile, T *buffer)
    {
    std::string line;

    while ( !inputFile.eof() )
      {
      std::getline(inputFile, line, '\n');

      if ( line.find("POINTS") != std::string::npos )
        {
        /**  Load the point coordinates into the itk::Mesh */
        unsigned long numberOfComponents = this->m_NumberOfPoints * this->m_PointDimension;
        for ( unsigned int ii = 0; ii < numberOfComponents; ii++ )
          {
          inputFile >> buffer[ii];
          }
        }
      }
    }

  template< typename T >
  void ReadPointsBufferAsBINARY(std::ifstream & inputFile, T *buffer)
    {
    std::string line;

    while ( !inputFile.eof() )
      {
      std::getline(inputFile, line, '\n');

      if ( line.find("POINTS") != std::string::npos )
        {
        /**  Load the point coordinates into the itk::Mesh */
        unsigned long numberOfComponents = this->m_NumberOfPoints * this->m_PointDimension;
        inputFile.read( reinterpret_cast< char * >( buffer ), numberOfComponents * sizeof( T ) );
        if ( itk::ByteSwapper< T >::SystemIsLittleEndian() )
          {
          itk::ByteSwapper< T >::SwapRangeFromSystemToBigEndian(buffer, numberOfComponents);
          }
        }
      }
    }

  void ReadCellsBufferAsASCII(std::ifstream & inputFile, void *buffer);

  void ReadCellsBufferAsBINARY(std::ifstream & inputFile, void *buffer);

  template< typename T >
  void ReadPointDataBufferAsASCII(std::ifstream & inputFile, T *buffer)
    {
    StringType line;

    while ( !inputFile.eof() )
      {
      std::getline(inputFile, line, '\n');
      if ( line.find("POINT_DATA") != std::string::npos )
        {
        if ( !inputFile.eof() )
          {
          std::getline(inputFile, line, '\n');
          }
        else
          {
          itkExceptionMacro("UnExpected end of line while trying to read POINT_DATA");
          }

        /** For scalars we have to read the next line of LOOKUP_TABLE */
        if ( line.find("SCALARS") != std::string::npos && line.find("COLOR_SCALARS") == std::string::npos )
          {
          if ( !inputFile.eof() )
            {
            std::getline(inputFile, line, '\n');
            if ( line.find("LOOKUP_TABLE") == std::string::npos )
              {
              itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
              }
            }
          else
            {
            itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
            }
          }

        /** for VECTORS or NORMALS or TENSORS, we could read them directly */
        unsigned long numberOfComponents = this->m_NumberOfPoints * this->m_NumberOfPointPixelComponents;
        for ( unsigned int ii = 0; ii < numberOfComponents; ii++ )
          {
          inputFile >> buffer[ii];
          }
        }
      }
    }

  template< typename T >
  void ReadPointDataBufferAsBINARY(std::ifstream & inputFile, T *buffer)
    {
    StringType line;

    while ( !inputFile.eof() )
      {
      std::getline(inputFile, line, '\n');
      if ( line.find("POINT_DATA") != std::string::npos )
        {
        if ( !inputFile.eof() )
          {
          std::getline(inputFile, line, '\n');
          }
        else
          {
          itkExceptionMacro("UnExpected end of line while trying to read POINT_DATA");
          }

        /** For scalars we have to read the next line of LOOKUP_TABLE */
        if ( line.find("SCALARS") != std::string::npos && line.find("COLOR_SCALARS") == std::string::npos )
          {
          if ( !inputFile.eof() )
            {
            std::getline(inputFile, line, '\n');
            if ( line.find("LOOKUP_TABLE") == std::string::npos )
              {
              itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
              }
            }
          else
            {
            itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
            }
          }

        /** for VECTORS or NORMALS or TENSORS, we could read them directly */
        unsigned long numberOfComponents = this->m_NumberOfPoints * this->m_NumberOfPointPixelComponents;
        inputFile.read( reinterpret_cast< char * >( buffer ), numberOfComponents * sizeof( T ) );
        if ( itk::ByteSwapper< T >::SystemIsLittleEndian() )
          {
          itk::ByteSwapper< T >::SwapRangeFromSystemToBigEndian(buffer, numberOfComponents);
          }
        }
      }
    }

  template< typename T >
  void ReadCellDataBufferAsASCII(std::ifstream & inputFile, T *buffer)
    {
    StringType line;

    while ( !inputFile.eof() )
      {
      std::getline(inputFile, line, '\n');
      if ( line.find("CELL_DATA") != std::string::npos )
        {
        if ( !inputFile.eof() )
          {
          std::getline(inputFile, line, '\n');
          }
        else
          {
          itkExceptionMacro("UnExpected end of line while trying to read CELL_DATA");
          }

        /** For scalars we have to read the next line of LOOKUP_TABLE */
        if ( line.find("SCALARS") != std::string::npos && line.find("COLOR_SCALARS") == std::string::npos )
          {
          if ( !inputFile.eof() )
            {
            std::getline(inputFile, line, '\n');
            if ( line.find("LOOKUP_TABLE") == std::string::npos )
              {
              itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
              }
            }
          else
            {
            itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
            }
          }

        /** for VECTORS or NORMALS or TENSORS, we could read them directly */
        unsigned long numberOfComponents = this->m_NumberOfCells * this->m_NumberOfCellPixelComponents;
        for ( unsigned int ii = 0; ii < numberOfComponents; ii++ )
          {
          inputFile >> buffer[ii];
          }
        }
      }
    }

  template< typename T >
  void ReadCellDataBufferAsBINARY(std::ifstream & inputFile, T *buffer)
    {
    StringType line;

    while ( !inputFile.eof() )
      {
      std::getline(inputFile, line, '\n');
      if ( line.find("CELL_DATA") != std::string::npos )
        {
        if ( !inputFile.eof() )
          {
          std::getline(inputFile, line, '\n');
          }
        else
          {
          itkExceptionMacro("UnExpected end of line while trying to read POINT_DATA");
          }

        /** For scalars we have to read the next line of LOOKUP_TABLE */
        if ( line.find("SCALARS") != std::string::npos && line.find("COLOR_SCALARS") == std::string::npos )
          {
          if ( !inputFile.eof() )
            {
            std::getline(inputFile, line, '\n');
            if ( line.find("LOOKUP_TABLE") == std::string::npos )
              {
              itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
              }
            }
          else
            {
            itkExceptionMacro("UnExpected end of line while trying to read LOOKUP_TABLE");
            }
          }
        /** For VECTORS or NORMALS or TENSORS, we could read them directly */
        unsigned long numberOfComponents = this->m_NumberOfCells * this->m_NumberOfCellPixelComponents;
        inputFile.read( reinterpret_cast< char * >( buffer ), numberOfComponents * sizeof( T ) );
        if ( itk::ByteSwapper< T >::SystemIsLittleEndian() )
          {
          itk::ByteSwapper< T >::SwapRangeFromSystemToBigEndian(buffer, numberOfComponents);
          }
        }
      }
    }

  template< typename T >
  void WritePointsBufferAsASCII(std::ofstream & outputFile, T *buffer, const StringType & pointComponentType)
  {
    /** 1. Write number of points */
    outputFile << "POINTS " << this->m_NumberOfPoints;

    outputFile << pointComponentType << '\n';
    for ( unsigned long ii = 0; ii < this->m_NumberOfPoints; ii++ )
      {
      for ( unsigned int jj = 0; jj < this->m_PointDimension - 1; jj++ )
        {
        outputFile << buffer[ii * this->m_PointDimension + jj] << " ";
        }

      outputFile << buffer[ii * this->m_PointDimension + this->m_PointDimension - 1] << '\n';
      }

    return;
  }

  template< typename T >
  void WritePointsBufferAsBINARY(std::ofstream & outputFile, T *buffer, const StringType & pointComponentType)
  {
    /** 1. Write number of points */
    outputFile << "POINTS " << this->m_NumberOfPoints;
    outputFile << pointComponentType << "\n";
    itk::ByteSwapper< T >::SwapWriteRangeFromSystemToBigEndian(buffer, this->m_NumberOfPoints * this->m_PointDimension, &outputFile);
    outputFile << "\n";

    return;
  }

  template< typename T >
  void WriteCellsBufferAsASCII(std::ofstream & outputFile, T *buffer)
    {
    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    unsigned int         numberOfVertices = 0;
    unsigned int         numberOfVertexIndices = 0;
    unsigned int         numberOfLines = 0;
    unsigned int         numberOfLineIndices = 0;
    unsigned int         numberOfPolygons = 0;
    unsigned int         numberOfPolygonIndices = 0;

    /** Write vertices */
    unsigned long index = 0;

    ExposeMetaData< unsigned int >(metaDic, "numberOfVertices", numberOfVertices);
    if ( numberOfVertices )
      {
      ExposeMetaData< unsigned int >(metaDic, "numberOfVertexIndices", numberOfVertexIndices);
      outputFile << "VERTICES " << numberOfVertices << " " << numberOfVertexIndices << '\n';
      for ( unsigned long ii = 0; ii < this->m_NumberOfCells; ii++ )
        {
        MeshIOBase::CellGeometryType cellType = static_cast< MeshIOBase::CellGeometryType >( static_cast< int >( buffer[index++] ) );
        unsigned int                 nn = static_cast< unsigned int >( buffer[index++] );
        if ( cellType == VERTEX_CELL )
          {
          outputFile << nn;
          for ( unsigned int jj = 0; jj < nn; jj++ )
            {
            outputFile << " " << buffer[index++];
            }
          }

        outputFile << '\n';
        }
      }

    /** Write lines */
    index = 0;
    ExposeMetaData< unsigned int >(metaDic, "numberOfLines", numberOfLines);
    if ( numberOfLines )
      {
      ExposeMetaData< unsigned int >(metaDic, "numberOfLineIndices", numberOfLineIndices);
      outputFile << "LINES " << numberOfLines << " " << numberOfLineIndices << '\n';
      for ( unsigned long ii = 0; ii < this->m_NumberOfCells; ii++ )
        {
        MeshIOBase::CellGeometryType cellType = static_cast< MeshIOBase::CellGeometryType >( static_cast< int >( buffer[index++] ) );
        unsigned int  nn = static_cast< unsigned int >( buffer[index++] );
        if ( cellType == POLYLINE_CELL )
          {
          outputFile << nn;
          for ( unsigned int jj = 0; jj < nn; jj++ )
            {
            outputFile << " " << buffer[index++];
            }
          }

        outputFile << '\n';
        }
      }

    /** Write polygons */
    index = 0;
    ExposeMetaData< unsigned int >(metaDic, "numberOfPolygons", numberOfPolygons);
    if ( numberOfPolygons )
      {
      ExposeMetaData< unsigned int >(metaDic, "numberOfPolygonIndices", numberOfPolygonIndices);
      outputFile << "POLYGONS " << numberOfPolygons << " " << numberOfPolygonIndices << '\n';
      for ( unsigned long ii = 0; ii < this->m_NumberOfCells; ii++ )
        {
        MeshIOBase::CellGeometryType cellType = static_cast< MeshIOBase::CellGeometryType >( static_cast< int >( buffer[index++] ) );
        unsigned int                 nn = static_cast< unsigned int >( buffer[index++] );
        if ( cellType == POLYGON_CELL || cellType == TRIANGLE_CELL )
          {
          outputFile << nn;
          for ( unsigned int jj = 0; jj < nn; jj++ )
            {
            outputFile << " " << buffer[index++];
            }
          }

        outputFile << '\n';
        }
      }
    }

  template< typename T >
  void WriteCellsBufferAsBINARY(std::ofstream & outputFile, T *buffer)
    {
    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    unsigned int         numberOfVertices = 0;
    unsigned int         numberOfVertexIndices = 0;
    unsigned int         numberOfLines = 0;
    unsigned int         numberOfLineIndices = 0;
    unsigned int         numberOfPolygons = 0;
    unsigned int         numberOfPolygonIndices = 0;

    /** Write vertices */
    unsigned long index = 0;

    ExposeMetaData< unsigned int >(metaDic, "numberOfVertices", numberOfVertices);
    if ( numberOfVertices )
      {
      ExposeMetaData< unsigned int >(metaDic, "numberOfVertexIndices", numberOfVertexIndices);
      outputFile << "VERTICES " << numberOfVertices << " " << numberOfVertexIndices << '\n';
      unsigned int *data  = new unsigned int[numberOfVertexIndices];
      ReadCellsBuffer(buffer, data);
      itk::ByteSwapper< unsigned int >::SwapWriteRangeFromSystemToBigEndian(data, numberOfVertexIndices, &outputFile);
      outputFile << "\n";
      delete[] data;
      }

    /** Write lines */
    index = 0;
    ExposeMetaData< unsigned int >(metaDic, "numberOfLines", numberOfLines);
    if ( numberOfLines )
      {
      ExposeMetaData< unsigned int >(metaDic, "numberOfLineIndices", numberOfLineIndices);
      outputFile << "LINES " << numberOfLines << " " << numberOfLineIndices << '\n';
      unsigned int *data  = new unsigned int[numberOfLineIndices];
      ReadCellsBuffer(buffer, data);
      itk::ByteSwapper< unsigned int >::SwapWriteRangeFromSystemToBigEndian(data, numberOfLineIndices, &outputFile);
      outputFile << "\n";
      delete[] data;
      }

    /** Write polygons */
    index = 0;
    ExposeMetaData< unsigned int >(metaDic, "numberOfPolygons", numberOfPolygons);
    if ( numberOfPolygons )
      {
      ExposeMetaData< unsigned int >(metaDic, "numberOfPolygonIndices", numberOfPolygonIndices);
      outputFile << "POLYGONS " << numberOfPolygons << " " << numberOfPolygonIndices << '\n';
      unsigned int *data  = new unsigned int[numberOfPolygonIndices];
      ReadCellsBuffer(buffer, data);
      itk::ByteSwapper< unsigned int >::SwapWriteRangeFromSystemToBigEndian(data, numberOfPolygonIndices, &outputFile);
      outputFile << "\n";
      delete[] data;
      }
    }

  template< typename T >
  void WritePointDataBufferAsASCII(std::ofstream & outputFile, T *buffer, const StringType & pointPixelComponentName)
  {
    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    StringType           dataName;

    outputFile << "POINT_DATA " << this->m_NumberOfPoints << '\n';
    switch ( this->m_PointPixelType )
      {
      case SCALAR:
        {
        outputFile << "SCALARS ";
        ExposeMetaData< StringType >(metaDic, "pointScalarDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case OFFSET:
      case POINT:
      case COVARIANTVECTOR:
      case VECTOR:
        {
        outputFile << "VECTORS ";
        ExposeMetaData< StringType >(metaDic, "pointVectorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case SYMMETRICSECONDRANKTENSOR:
      case DIFFUSIONTENSOR3D:
        {
        outputFile << "TENSORS ";
        ExposeMetaData< StringType >(metaDic, "pointTensorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case ARRAY:
      case VARIABLELENGTHVECTOR:
        {
        outputFile << "COLOR_SCALARS ";
        ExposeMetaData< StringType >(metaDic, "pointColorScalarDataName", dataName);
        outputFile << dataName << "  ";
        WriteColorScalarBufferAsASCII(outputFile, buffer, this->m_NumberOfPointPixelComponents, this->m_NumberOfPoints);
        return;
        }
      default:
        {
        itkExceptionMacro(<< "Unknown point pixel type");
        }
      }

    outputFile << pointPixelComponentName << '\n';

    if ( this->m_PointPixelType == SCALAR )
      {
      outputFile << "LOOKUP_TABLE default" << '\n';
      }

    Indent indent(2);
    for ( unsigned long ii = 0; ii < this->m_NumberOfPoints; ii++ )
      {
      for ( unsigned int jj = 0; jj < this->m_NumberOfPointPixelComponents; jj++ )
        {
        outputFile << buffer[ii * this->m_NumberOfPointPixelComponents + jj] << indent;
        }
      outputFile << '\n';
      }

    return;
  }

  template< typename T >
  void WritePointDataBufferAsBINARY(std::ofstream & outputFile, T *buffer, const StringType & pointPixelComponentName)
  {
    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    StringType           dataName;

    outputFile << "POINT_DATA " << this->m_NumberOfPoints << "\n";
    switch ( this->m_PointPixelType )
      {
      case SCALAR:
        {
        outputFile << "SCALARS ";
        ExposeMetaData< StringType >(metaDic, "pointScalarDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case OFFSET:
      case POINT:
      case COVARIANTVECTOR:
      case VECTOR:
        {
        outputFile << "VECTORS ";
        ExposeMetaData< StringType >(metaDic, "pointVectorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case SYMMETRICSECONDRANKTENSOR:
      case DIFFUSIONTENSOR3D:
        {
        outputFile << "TENSORS ";
        ExposeMetaData< StringType >(metaDic, "pointTensorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case ARRAY:
      case VARIABLELENGTHVECTOR:
        {
        outputFile << "COLOR_SCALARS ";
        ExposeMetaData< StringType >(metaDic, "pointColorScalarDataName", dataName);
        outputFile << dataName << "  ";
        WriteColorScalarBufferAsBINARY(outputFile, buffer, this->m_NumberOfPointPixelComponents, this->m_NumberOfPoints);
        return;
        }
      default:
        {
        itkExceptionMacro(<< "Unknown point pixel type");
        }
      }

    outputFile << pointPixelComponentName << "\n";
    if ( this->m_PointPixelType == SCALAR )
      {
      outputFile << "LOOKUP_TABLE default\n";
      }

    itk::ByteSwapper< T >::SwapWriteRangeFromSystemToBigEndian(buffer,
                                                               this->m_NumberOfPoints * this->m_NumberOfPointPixelComponents,
                                                               &outputFile);
    outputFile << "\n";
    return;
  }

  template< typename T >
  void WriteCellDataBufferAsASCII(std::ofstream & outputFile, T *buffer, const StringType & cellPixelComponentName)
  {
    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    StringType           dataName;

    outputFile << "CELL_DATA " << this->m_NumberOfCells << '\n';
    switch ( this->m_CellPixelType )
      {
      case SCALAR:
        {
        outputFile << "SCALARS ";
        ExposeMetaData< StringType >(metaDic, "cellScalarDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case OFFSET:
      case POINT:
      case COVARIANTVECTOR:
      case VECTOR:
        {
        outputFile << "VECTORS ";
        ExposeMetaData< StringType >(metaDic, "cellVectorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case SYMMETRICSECONDRANKTENSOR:
      case DIFFUSIONTENSOR3D:
        {
        outputFile << "TENSORS ";
        ExposeMetaData< StringType >(metaDic, "cellTensorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case ARRAY:
      case VARIABLELENGTHVECTOR:
        {
        outputFile << "COLOR_SCALARS ";
        ExposeMetaData< StringType >(metaDic, "cellColorScalarDataName", dataName);
        outputFile << dataName << "  ";
        WriteColorScalarBufferAsASCII(outputFile, buffer, this->m_NumberOfCellPixelComponents, this->m_NumberOfCells);
        return;
        }
      default:
        {
        itkExceptionMacro(<< "Unknown cell pixel type");
        }
      }

    outputFile << cellPixelComponentName << '\n';
    if ( this->m_CellPixelType == SCALAR )
      {
      outputFile << "LOOKUP_TABLE default" << '\n';
      }

    Indent indent(2);
    for ( unsigned long ii = 0; ii < this->m_NumberOfCells; ii++ )
      {
      for ( unsigned int jj = 0; jj < this->m_NumberOfCellPixelComponents; jj++ )
        {
        outputFile << buffer[ii * this->m_NumberOfCellPixelComponents + jj] << indent;
        }
      outputFile << '\n';
      }

    return;
  }

  template< typename T >
  void WriteCellDataBufferAsBINARY(std::ofstream & outputFile, T *buffer, const StringType & cellPixelComponentName)
  {
    MetaDataDictionary & metaDic = this->GetMetaDataDictionary();
    StringType           dataName;

    outputFile << "CELL_DATA " << this->m_NumberOfCells << "\n";
    switch ( this->m_CellPixelType )
      {
      case SCALAR:
        {
        outputFile << "SCALARS ";
        ExposeMetaData< StringType >(metaDic, "cellScalarDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case OFFSET:
      case POINT:
      case COVARIANTVECTOR:
      case VECTOR:
        {
        outputFile << "VECTORS ";
        ExposeMetaData< StringType >(metaDic, "cellVectorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case SYMMETRICSECONDRANKTENSOR:
      case DIFFUSIONTENSOR3D:
        {
        outputFile << "TENSORS ";
        ExposeMetaData< StringType >(metaDic, "cellTensorDataName", dataName);
        outputFile << dataName << "  ";
        break;
        }
      case ARRAY:
      case VARIABLELENGTHVECTOR:
        {
        outputFile << "COLOR_SCALARS ";
        ExposeMetaData< StringType >(metaDic, "cellColorScalarDataName", dataName);
        outputFile << dataName << "  ";
        WriteColorScalarBufferAsBINARY(outputFile, buffer, this->m_NumberOfCellPixelComponents, this->m_NumberOfCells);
        return;
        }
      default:
        {
        itkExceptionMacro(<< "Unknown cell pixel type");
        }
      }

    outputFile << cellPixelComponentName << "\n";
    if ( this->m_CellPixelType == SCALAR )
      {
      outputFile << "LOOKUP_TABLE default\n";
      }

    itk::ByteSwapper< T >::SwapWriteRangeFromSystemToBigEndian(buffer,
                                                               this->m_NumberOfCells * this->m_NumberOfCellPixelComponents,
                                                               &outputFile);
    outputFile << "\n";
    return;
  }

  template< typename T >
  void WriteColorScalarBufferAsASCII(std::ofstream & outputFile,
                                     T *buffer,
                                     unsigned long numberOfPixelComponents,
                                     unsigned long numberOfPixels)
    {
    outputFile << numberOfPixelComponents << "\n";
    Indent indent(2);
    for ( unsigned long ii = 0; ii < numberOfPixels; ++ii )
      {
      for ( unsigned int jj = 0; jj < numberOfPixelComponents; ++jj )
        {
        outputFile << static_cast< float >( buffer[ii * numberOfPixelComponents + jj] ) << indent;
        }

      outputFile << "\n";
      }

    return;
    }

  template< typename T >
  void WriteColorScalarBufferAsBINARY(std::ofstream & outputFile,
                                      T *buffer,
                                      unsigned long numberOfPixelComponents,
                                      unsigned long numberOfPixels)
    {
    outputFile << numberOfPixelComponents << "\n";
    unsigned long  numberOfElements = numberOfPixelComponents * numberOfPixels;
    unsigned char *data = new unsigned char[numberOfElements];
    for ( unsigned long ii = 0; ii < numberOfElements; ++ii )
      {
      data[ii] = static_cast< unsigned char >( buffer[ii] );
      }

    outputFile.write(reinterpret_cast< char * >( data ), numberOfElements);

    delete[] data;
    outputFile << "\n";
    return;
    }

  /** Convert cells buffer for output cells buffer, it's user's responsibility to make sure
  the input cells don't contain any cell type that coule not be written as polygon cell */
  template< typename TInput, typename TOutput >
  void ReadCellsBuffer(TInput *input, TOutput *output)
    {
    unsigned long inputIndex = 0;
    unsigned long outputIndex = 0;

    if ( input && output )
      {
      for ( unsigned int ii = 0; ii < this->m_NumberOfCells; ii++ )
        {
        inputIndex++;
        unsigned int nn = static_cast< unsigned int >( input[inputIndex++] );
        output[outputIndex++] = nn;
        for ( unsigned int jj = 0; jj < nn; jj++ )
          {
          output[outputIndex++] = static_cast< TOutput >( input[inputIndex++] );
          }
        }
      }
    }

private:
  VTKPolyDataMeshIO(const Self &); // purposely not implemented
  void operator=(const Self &);    // purposely not implemented
};
} // end namespace itk

#endif // __itkVTKPolyDataMeshIO_h
