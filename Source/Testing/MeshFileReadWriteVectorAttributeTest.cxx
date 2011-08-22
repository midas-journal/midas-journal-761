#include "itkCovariantVector.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "itkMesh.h"
#include "itkQuadEdgeMesh.h"
#include "itksys/SystemTools.hxx"

int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		std::cerr<<"Invalid commands, You need input and output mesh file name "<<std::endl;
		return EXIT_FAILURE;
	}
	const unsigned int dimension = 3;
	typedef itk::CovariantVector<float,dimension>      PixelType;
	typedef itk::Mesh<PixelType, dimension>            MeshType;
	typedef itk::QuadEdgeMesh<PixelType, dimension>    QuadEdgeMeshType;
	
	typedef itk::MeshFileReader<MeshType>              MeshFileReaderType;
	typedef itk::MeshFileReader<QuadEdgeMeshType>      QuadEdgeMeshFileReaderType;
	typedef itk::MeshFileWriter<MeshType>              MeshFileWriterType;
	typedef itk::MeshFileWriter<QuadEdgeMeshType>      QuadEdgeMeshFileWriterType;
	
	const double tol = 1e-8;
	MeshFileReaderType::Pointer reader = MeshFileReaderType::New();
	reader->SetFileName(argv[1]);
	try
	{
		reader->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr<<"Read file "<<argv[1]<<" failed "<<std::endl;
		std::cerr<<err<<std::endl;
		return EXIT_FAILURE;
	}

	MeshFileWriterType::Pointer writer = MeshFileWriterType::New();
	writer->SetFileName(argv[2]);
	writer->SetInput(reader->GetOutput());
	try
	{
		writer->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr<<"Write file "<<argv[2]<<" failed "<<std::endl;
		std::cerr<<err<<std::endl;
		return EXIT_FAILURE;
	}
	
	
	if( itksys::SystemTools::FilesDiffer( argv[1], argv[2] ) )
	{
		MeshFileReaderType::Pointer reader1 = MeshFileReaderType::New();
		reader1->SetFileName(argv[2]);

		try
		{
			reader1->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr<<"Read file "<<argv[2]<<" failed "<<std::endl;
			std::cerr<<err<<std::endl;
			return EXIT_FAILURE;
		}
		
		
		MeshType::PointsContainerPointer points0 = reader->GetOutput()->GetPoints();
		MeshType::PointsContainerPointer points1 = reader1->GetOutput()->GetPoints();
		
		//Test points
		MeshType::PointsContainerConstIterator pt0 = points0->Begin();
		MeshType::PointsContainerConstIterator pt1 = points1->Begin();
		while(pt0 != points0->End())
		{
			//	if(pt0.Value() != pt1.Value())
			if(pt0.Value().SquaredEuclideanDistanceTo(pt1.Value()) > tol)
			{
				std::cerr<<"Input mesh and output mesh are different in points!"<<std::endl;
				std::cerr<<"Input point = "<<pt0.Value()<<std::endl;
				std::cerr<<"Output point = "<<pt1.Value()<<std::endl;
				return EXIT_FAILURE;
			}
			
			pt0++;
			pt1++;
		}
		
		//Test cells
		MeshType::CellsContainerPointer cells0 = reader->GetOutput()->GetCells();
		MeshType::CellsContainerPointer cells1 = reader1->GetOutput()->GetCells();
		if(cells0 && cells1)
		{
			MeshType::CellsContainerConstIterator ceIt0 = cells0->Begin();
			MeshType::CellsContainerConstIterator ceIt1 = cells1->Begin();
			while(ceIt0 != cells0->End())
			{
				if(ceIt0.Value()->GetType() != ceIt1.Value()->GetType())
				{
					std::cerr<<"Input mesh and output mesh are different in cell type!"<<std::endl;
					return EXIT_FAILURE;
				}
				
				MeshType::CellType::PointIdIterator pit0 = ceIt0.Value()->PointIdsBegin();
				MeshType::CellType::PointIdIterator pit1 = ceIt1.Value()->PointIdsBegin();
				while(pit0 != ceIt0.Value()->PointIdsEnd())
				{
					if(*pit0 != *pit1)
					{
						std::cerr<<"Input mesh and output mesh are different in cells!"<<std::endl;
						return EXIT_FAILURE;
					}
					pit0++;
					pit1++;
				}
				
				ceIt0++;
				ceIt1++;
			}
		}

		//Test point data
		MeshType::PointDataContainerPointer pointData0 = reader->GetOutput()->GetPointData();
		MeshType::PointDataContainerPointer pointData1 = reader1->GetOutput()->GetPointData();

		if(pointData0 && pointData1)
		{
			MeshType::PointDataContainerIterator pdIt0 = pointData0->Begin();
			MeshType::PointDataContainerIterator pdIt1 = pointData1->Begin();
			while(pdIt0 != pointData0->End())
			{
				if(pdIt0.Value() != pdIt1.Value())
				{
					std::cerr<<"Input mesh and output mesh are different in point data!"<<std::endl;
					std::cerr<<"Input = "<<pdIt0.Value()<<std::endl;
					std::cerr<<"Output = "<<pdIt1.Value()<<std::endl;
					return EXIT_FAILURE;
				}
				
				pdIt0++;
				pdIt1++;
			}
		}
		else
		{
			if(pointData0.GetPointer() != pointData1.GetPointer())
			{
				std::cerr<<"Input mesh and output mesh are different in point data!"<<std::endl;
				return EXIT_FAILURE;
			}
		}
		
		//Test cell data
		MeshType::PointDataContainerPointer cellData0 = reader->GetOutput()->GetCellData();
		MeshType::PointDataContainerPointer cellData1 = reader1->GetOutput()->GetCellData();
		if(cellData0 && cellData1)
		{
			MeshType::CellDataContainerIterator cdIt0 = cellData0->Begin();
			MeshType::CellDataContainerIterator cdIt1 = cellData1->Begin();
			while(cdIt0 != cellData0->End())
			{
				if(cdIt0.Value() != cdIt1.Value())
				{
					std::cerr<<"Input mesh and output mesh are different in cell data!"<<std::endl;
					std::cerr<<"Input = "<<cdIt0.Value()<<std::endl;
					std::cerr<<"Output = "<<cdIt1.Value()<<std::endl;
					return EXIT_FAILURE;
				}
				
				cdIt0++;
				cdIt1++;
			}
		}
		else
		{
			if(cellData0.GetPointer() != cellData1.GetPointer())
			{
				std::cerr<<"Input mesh and output mesh are different in cell data!"<<std::endl;
				return EXIT_FAILURE;
			}
		}
	}
	
	QuadEdgeMeshFileReaderType::Pointer reader2 = QuadEdgeMeshFileReaderType::New();
	reader2->SetFileName(argv[1]);

	try
	{
		reader2->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr<<"Read file "<<argv[1]<<" failed "<<std::endl;
		std::cerr<<err<<std::endl;
		return EXIT_FAILURE;
	}

	QuadEdgeMeshFileWriterType::Pointer writer2 = QuadEdgeMeshFileWriterType::New();
	writer2->SetFileName(argv[2]);
	writer2->SetInput(reader2->GetOutput());
	//	std::cout<<"number of cells = "<<reader2->GetOutput()->GetNumberOfCells()<<std::endl;
	
	try
	{
		writer2->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr<<"Write file "<<argv[2]<<" failed "<<std::endl;
		std::cerr<<err<<std::endl;
		return EXIT_FAILURE;
	}

	
	if( itksys::SystemTools::FilesDiffer( argv[1], argv[2] ) )
	{
		QuadEdgeMeshFileReaderType::Pointer reader3 = QuadEdgeMeshFileReaderType::New();
		reader3->SetFileName(argv[2]);
		
		try
		{
			reader3->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr<<"Read file "<<argv[2]<<" failed "<<std::endl;
			std::cerr<<err<<std::endl;
			return EXIT_FAILURE;
		}
		
		QuadEdgeMeshType::PointsContainerPointer points2 = reader2->GetOutput()->GetPoints();
		QuadEdgeMeshType::PointsContainerPointer points3 = reader3->GetOutput()->GetPoints();
		
		//Test points
		QuadEdgeMeshType::PointsContainerConstIterator pt2 = points2->Begin();
		QuadEdgeMeshType::PointsContainerConstIterator pt3 = points3->Begin();
		while(pt2 != points2->End())
		{
			//	if(pt2.Value() != pt3.Value())
			if(pt2.Value().SquaredEuclideanDistanceTo(pt3.Value()) > tol)
			{
				std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in points!"<<std::endl;
				return EXIT_FAILURE;
			}

			pt2++;
			pt3++;
		}
		
		//Test cells
		QuadEdgeMeshType::CellsContainerPointer cells2 = reader2->GetOutput()->GetCells();
		QuadEdgeMeshType::CellsContainerPointer cells3 = reader3->GetOutput()->GetCells();
		if(cells2 && cells3)
		{
			QuadEdgeMeshType::CellsContainerConstIterator ceIt2 = cells2->Begin();
			QuadEdgeMeshType::CellsContainerConstIterator ceIt3 = cells3->Begin();
			while(ceIt2 != cells2->End())
			{
				QuadEdgeMeshType::PolygonCellType * pe2 = dynamic_cast<QuadEdgeMeshType::PolygonCellType *>(ceIt2.Value());
				QuadEdgeMeshType::PolygonCellType * pe3 = dynamic_cast<QuadEdgeMeshType::PolygonCellType *>(ceIt3.Value());
				
				if(ceIt2.Value()->GetType() != ceIt3.Value()->GetType())
				{
					std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in cell type!"<<std::endl;
					return EXIT_FAILURE;
				}
				
				if(pe2 && pe3)
				{
					QuadEdgeMeshType::CellTraits::PointIdInternalIterator ptId2 = pe2->InternalPointIdsBegin();
					QuadEdgeMeshType::CellTraits::PointIdInternalIterator ptId3 = pe3->InternalPointIdsBegin();
					while(ptId2 != pe2->InternalPointIdsEnd())
					{
						if(*ptId2 != *ptId3)
						{
							std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in cells!"<<std::endl;
							return EXIT_FAILURE;
						}
						ptId2++;
						ptId3++;
					}
				}
				else
				{
					if(pe2 != pe3)
					{
						std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in cells!"<<std::endl;
						return EXIT_FAILURE;
					}
				}
				++ceIt2;
				++ceIt3;
			}
		}

		
		//Test point data
		QuadEdgeMeshType::PointDataContainerPointer pointData2 = reader2->GetOutput()->GetPointData();
		QuadEdgeMeshType::PointDataContainerPointer pointData3 = reader3->GetOutput()->GetPointData();
		if(pointData2 && pointData3)
		{
			QuadEdgeMeshType::PointDataContainerIterator pdIt2 = pointData2->Begin();
			QuadEdgeMeshType::PointDataContainerIterator pdIt3 = pointData3->Begin();
			while(pdIt2 != pointData2->End())
			{
				if(pdIt2.Value() != pdIt3.Value())
				{
					std::cerr<<"Input QaudEdge mesh and output QuadEdge mesh are different in point data!"<<std::endl;
					std::cerr<<"Input = "<<pdIt2.Value()<<std::endl;
					std::cerr<<"Output = "<<pdIt3.Value()<<std::endl;
					return EXIT_FAILURE;
				}
				
				pdIt2++;
				pdIt3++;
			}
		}
		else
		{
			if(pointData2.GetPointer() != pointData3.GetPointer())
			{
				std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in point data!"<<std::endl;
				return EXIT_FAILURE;
			}
		}
		
		//Test cell data
		QuadEdgeMeshType::PointDataContainerPointer cellData2 = reader2->GetOutput()->GetCellData();
		QuadEdgeMeshType::PointDataContainerPointer cellData3 = reader3->GetOutput()->GetCellData();
		if(cellData2 && cellData3)
		{
			QuadEdgeMeshType::CellDataContainerIterator cdIt2 = cellData2->Begin();
			QuadEdgeMeshType::CellDataContainerIterator cdIt3 = cellData3->Begin();
			while(cdIt2 != cellData2->End())
			{
				if(cdIt2.Value() != cdIt3.Value())
				{
					std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in cell data!"<<std::endl;
					std::cerr<<"Input = "<<cdIt2.Value()<<std::endl;
					std::cerr<<"Output = "<<cdIt3.Value()<<std::endl;
					return EXIT_FAILURE;
				}
				
				cdIt2++;
				cdIt3++;
			}
		}
		else
		{
			if(cellData2.GetPointer() != cellData3.GetPointer())
			{
				std::cerr<<"Input QuadEdge mesh and output QuadEdge mesh are different in cell data!"<<std::endl;
				return EXIT_FAILURE;
			}
		}

	}
	
	return EXIT_SUCCESS;
}

