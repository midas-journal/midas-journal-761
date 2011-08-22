/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPolylineCell.txx,v $
  Language:  C++
  Date:      $Date: 2009-03-03 15:07:57 $
  Version:   $Revision: 1.36 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPolylineCell_txx
#define __itkPolylineCell_txx
#include "itkPolylineCell.h"


namespace itk
{

/**
 * Standard CellInterface:
 */
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::MakeCopy(CellAutoPointer & cellPointer) const
{
  Self * newPolylineCell = new Self;
  cellPointer.TakeOwnership( newPolylineCell );
  const unsigned long numberOfPoints = this->GetNumberOfPoints();
  if ( numberOfPoints ) 
    {
    newPolylineCell->SetPointIds(0, numberOfPoints, this->GetPointIds());
    }
  else
    {
    newPolylineCell->ClearPoints(); 
    // Make sure the new cell has no points or edges
    }
}


  
/**
 * Standard CellInterface:
 * Get the topological dimension of this cell.
 */
template <typename TCellInterface>
unsigned int
PolylineCell< TCellInterface >
::GetDimension(void) const
{
  return Self::CellDimension;
}


/**
 * Standard CellInterface:
 * Get the number of points required to define the cell.
 */
template <typename TCellInterface>
unsigned int
PolylineCell< TCellInterface >
::GetNumberOfPoints(void) const
{
  return static_cast<unsigned int>( m_PointIds.size() );
}  


/**
 * Standard CellInterface:
 * Get the number of boundary features of the given dimension.
 */
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::CellFeatureCount
PolylineCell< TCellInterface >
::GetNumberOfBoundaryFeatures(int dimension) const
{
  switch (dimension)
    {
    case 0: return this->GetNumberOfVertices();
    case 1: return this->GetNumberOfEdges();
    default: return 0;
    }
}


/**
 * Standard CellInterface:
 * Get the boundary feature of the given dimension specified by the given
 * cell feature Id.
 * The Id can range from 0 to GetNumberOfBoundaryFeatures(dimension)-1.
 */
template <typename TCellInterface>
bool
PolylineCell< TCellInterface >
::GetBoundaryFeature(int dimension, CellFeatureIdentifier featureId,
                     CellAutoPointer& cellPointer )
{
  switch (dimension)
    {
    case 0: 
      {
      VertexAutoPointer vertexPointer;
      if( this->GetVertex(featureId,vertexPointer) )
        {
        TransferAutoPointer(cellPointer,vertexPointer);
        return true;
        }
      else
        {
        cellPointer.Reset();
        return false;
        }
      break;
      }
    case 1: 
      {
      EdgeAutoPointer edgePointer;
      if( this->GetEdge(featureId,edgePointer) )
        {
        TransferAutoPointer(cellPointer,edgePointer);
        return true;
        }
      else
        {
        cellPointer.Reset();
        return false;
        }
      break;
      }
      
    default: 
      {
      cellPointer.Reset();
      return false;
      }
    }
  return false;
}

/**
 * Standard CellInterface:
 * Set the point id list used by the cell.  It is assumed that the given
 * iterator can be incremented and safely de-referenced enough times to 
 * get all the point ids needed by the cell.
 */ 
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::SetPointIds(int itkNotUsed(dummy), int num, PointIdConstIterator first)
{
  PointIdConstIterator ii(first);
  m_PointIds.clear();
  for(int i=0; i < num; ++i)
    {
    m_PointIds.push_back(*ii++);
    }
  this->BuildEdges();
}

/**
 * after input the points in order, generate the edge connections
 */
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::BuildEdges(void)
{
  if( m_PointIds.size() > 0 )
    {
    m_Edges.resize(m_PointIds.size());
    const unsigned int numberOfPoints = static_cast<unsigned int>( m_PointIds.size() );
    for(unsigned int i = 1; i < numberOfPoints; i++)
      {
      m_Edges[i-1][0]=i-1;
      m_Edges[i-1][1]=i;
      }
    }
  else
    {
    m_Edges.clear();
    }
}

/**
 * Standard CellInterface:
 * Set the point id list used by the cell.  It is assumed that the given
 * iterator can be incremented and safely de-referenced enough times to 
 * get all the point ids needed by the cell.
 */ 
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::SetPointIds(PointIdConstIterator first)
{
  PointIdConstIterator ii(first);
  for(unsigned int i=0; i < m_PointIds.size(); ++i)
    {
    m_PointIds[i] = *ii++;
    }
}

/** 
 * Add one points to the points list
 */
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::AddPointId(PointIdentifier ptID)
{
  m_PointIds.push_back(ptID);
}

/** 
 * Remove one points from the points list
 */
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::RemovePointId(PointIdentifier ptID)
{
  typename std::vector<PointIdentifier>::iterator position = 
    std::find(m_PointIds.begin(),m_PointIds.end(),ptID);
  if( position != m_PointIds.end() )
    {
    m_PointIds.erase(position);
    }
}

/**
 * clear all the point and edge informations
 */
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::ClearPoints(void)
{
  m_PointIds.clear();
  m_Edges.clear();
}

/**
 * Standard CellInterface:
 * Set the point id list used by the cell.  It is assumed that the range
 * of iterators [first, last) contains the correct number of points needed to
 * define the cell.  The position *last is NOT referenced, so it can safely
 * be one beyond the end of an array or other container.
 */ 
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::SetPointIds(PointIdConstIterator first, PointIdConstIterator last)
{
  PointIdConstIterator ii(first);
  
  m_PointIds.clear();
  while(ii != last)
    {
    m_PointIds.push_back(*ii++);
    }

  this->BuildEdges();
}

/**
 * Standard CellInterface:
 * Set an individual point identifier in the cell.
 */ 
template <typename TCellInterface>
void
PolylineCell< TCellInterface >
::SetPointId(int localId, PointIdentifier ptId)
{
  if(m_PointIds.size() < (unsigned int)(localId + 1))
    {
    m_PointIds.resize( localId + 1 );
    }
  m_PointIds[localId] = ptId;
}


/**
 * Standard CellInterface:
 * Get a begin iterator to the list of point identifiers used by the cell.
 */
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::PointIdIterator
PolylineCell< TCellInterface >
::PointIdsBegin(void)
{
  if (m_PointIds.size() >0)
    {
    return &*(m_PointIds.begin());
    }
  else
    {
    return NULL;
    }
}


/**
 * Standard CellInterface:
 * Get a const begin iterator to the list of point identifiers used
 * by the cell.
 */ 
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::PointIdConstIterator
PolylineCell< TCellInterface >
::PointIdsBegin(void) const
{
  if (m_PointIds.size() >0)
    {
    return &*(m_PointIds.begin());
    }
  else
    {
    return NULL;
    }
}


/**
 * Standard CellInterface:
 * Get an end iterator to the list of point identifiers used by the cell.
 */ 
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::PointIdIterator
PolylineCell< TCellInterface >
::PointIdsEnd(void)
{
  if (m_PointIds.size() >0)
    {
    return &m_PointIds[m_PointIds.size()-1] + 1;
    }
  else
    {
    return NULL;
    }
}


/**
 * Standard CellInterface:
 * Get a const end iterator to the list of point identifiers used
 * by the cell.
 */ 
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::PointIdConstIterator
PolylineCell< TCellInterface >
::PointIdsEnd(void) const
{
  if (m_PointIds.size() >0)
    {
    return &m_PointIds[m_PointIds.size()-1] + 1;
    }
  else
    {
    return NULL;
    }
}


/**
 * Polyline-specific:
 * Get the number of vertices defining the Polyline.
 */
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::CellFeatureCount
PolylineCell< TCellInterface >
::GetNumberOfVertices(void) const
{
  return static_cast<CellFeatureCount>( m_PointIds.size() );
}

/**
 * Polyline-specific:
 * Get the number of edges defined for the Polyline.
 */
template <typename TCellInterface>
typename PolylineCell< TCellInterface >::CellFeatureCount
PolylineCell< TCellInterface >
::GetNumberOfEdges(void) const
{
  return static_cast<CellFeatureCount>( m_Edges.size() );
}

/**
 * Polyline-specific:
 * Get the vertex specified by the given cell feature Id.
 * The Id can range from 0 to GetNumberOfVertices()-1.
 */ 
template <typename TCellInterface>
bool
PolylineCell< TCellInterface >
::GetVertex(CellFeatureIdentifier vertexId,VertexAutoPointer & vertexPointer )
{
  VertexType * vert = new VertexType;
  vert->SetPointId(0, m_PointIds[vertexId]);
  vertexPointer.TakeOwnership( vert );
  return true;  
}

/**
 * Polyline-specific:
 * Get the edge specified by the given cell feature Id.
 * The Id can range from 0 to GetNumberOfEdges()-1.
 */ 
template <typename TCellInterface>
bool
PolylineCell< TCellInterface >
::GetEdge(CellFeatureIdentifier edgeId, EdgeAutoPointer & edgePointer )
{
  EdgeType * edge = new EdgeType;
  unsigned int max_pointId = this->GetNumberOfPoints() - 1;
  if( edgeId < max_pointId )
    {
    edge->SetPointId(0, m_PointIds[edgeId]);
    edge->SetPointId(1, m_PointIds[edgeId+1]);
    }
  else if( edgeId == max_pointId )
    {
    edge->SetPointId(0, m_PointIds[max_pointId] );
    edge->SetPointId(1, m_PointIds[0] );
    }
  edgePointer.TakeOwnership( edge ); 
  return true;
}

} // end namespace itk

#endif
