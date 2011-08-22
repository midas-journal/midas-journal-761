/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPolylineCell.h,v $
  Language:  C++
  Date:      $Date: 2009-03-03 15:07:57 $
  Version:   $Revision: 1.35 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPolylineCell_h
#define __itkPolylineCell_h

#include "itkCellInterface.h"
#include "itkLineCell.h"
#include "itkPoint.h"
#include <vector>
#include <deque>

namespace itk
{

/** \class PolylineCell
 * PolylineCell represents a polyline for a Mesh.
 *  the points of the polyline can be dynamically changed.
 *
 * Template parameters for PolylineCell:
 *
 * TPixelType =
 *     The type associated with a point, cell, or boundary for use in storing
 *     its data.
 *
 * TCellTraits =
 *     Type information of mesh containing cell.
 * \ingroup MeshObjects
 */
template < typename TCellInterface >
class ITK_EXPORT PolylineCell: public TCellInterface
{
public:
  /** Standard class typedefs. */
  itkCellCommonTypedefs(PolylineCell);
  itkCellInheritedTypedefs(TCellInterface);
   
  /** Standard part of every itk Object. */
  itkTypeMacro(PolylineCell, CellInterface);

  /** Save some template parameter information. */
  itkStaticConstMacro(CellDimension, unsigned int, 2);

  /** The type of boundary for this triangle's vertices. */
  typedef VertexCell< TCellInterface >            VertexType;
  typedef typename VertexType::SelfAutoPointer    VertexAutoPointer;
  
  /** The type of boundary for this triangle's edges. */
  typedef LineCell< TCellInterface >              EdgeType;
  typedef typename EdgeType::SelfAutoPointer      EdgeAutoPointer;
    
  typedef FixedArray<int,2>    EdgeInfo;
  typedef std::deque<EdgeInfo> EdgeInfoDQ;
  
 /** Need to add POLYGON_CELL into CellInterface. */
  itkCellVisitMacro(Superclass::LINE_CELL);
  //  itkCellVisitMacro(Superclass::POLYLINE_CELL);

  /** Implement the standard CellInterface. */
  virtual CellGeometry GetType(void) const 
    {return Superclass::LINE_CELL;}
  //{return Superclass::POLYLINE_CELL;}
  virtual void MakeCopy( CellAutoPointer & ) const;
  virtual unsigned int GetDimension(void) const;
  virtual unsigned int GetNumberOfPoints(void) const;
  virtual CellFeatureCount GetNumberOfBoundaryFeatures(int dimension) const;
  virtual bool GetBoundaryFeature(int dimension, CellFeatureIdentifier,CellAutoPointer &);
  
  virtual void SetPointIds(PointIdConstIterator first); 
  virtual void SetPointIds(PointIdConstIterator first,
                           PointIdConstIterator last);

  void AddPointId(PointIdentifier);
  void RemovePointId(PointIdentifier);
  void SetPointIds(int dummy, int num, PointIdConstIterator first);
  void BuildEdges(void);
  void ClearPoints(void);
  
  virtual void SetPointId(int localId, PointIdentifier);
  virtual PointIdIterator      PointIdsBegin(void);
  virtual PointIdConstIterator PointIdsBegin(void) const;
  virtual PointIdIterator      PointIdsEnd(void);
  virtual PointIdConstIterator PointIdsEnd(void) const; 
  
  /** Polygon-specific interface. */
  virtual CellFeatureCount GetNumberOfVertices(void) const;
  virtual CellFeatureCount GetNumberOfEdges(void) const;
  virtual bool GetVertex(CellFeatureIdentifier, VertexAutoPointer &);
  virtual bool GetEdge(CellFeatureIdentifier, EdgeAutoPointer &);
  

  /** Constructor and destructor */
  PolylineCell() {}
  PolylineCell(unsigned int NumberOfPoints)
    {
    for (unsigned int i = 0; i < NumberOfPoints; i++)
      {
      m_PointIds.push_back(NumericTraits<unsigned long>::max());
      }
    this->BuildEdges();
    }
  ~PolylineCell() {}

protected:
  std::vector<EdgeInfo>        m_Edges;
  std::vector<PointIdentifier> m_PointIds;

private:
  PolylineCell(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} //end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPolylineCell.txx"
#endif

#endif
