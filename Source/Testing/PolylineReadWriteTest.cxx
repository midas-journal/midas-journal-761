#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "itkMesh.h"
#include "itkArray.h"
#include "itkVariableLengthVector.h"

#include "itksys/SystemTools.hxx"

int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		std::cerr<<"Invalid commands, You need input and output mesh file name "<<std::endl;
		return EXIT_FAILURE;
	}
	const unsigned int dimension = 3;
	//	typedef float PixelType;
	typedef itk::VariableLengthVector<float>           PixelType;
	//	typedef itk::Array<float>                          PixelType;
	typedef itk::Mesh<PixelType, dimension>            MeshType;
	
	typedef itk::MeshFileReader<MeshType>              MeshFileReaderType;
	typedef itk::MeshFileWriter<MeshType>              MeshFileWriterType;
	
	const double tol = 1e-6;
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
	
	if(argc > 3)
	{
		writer->SetFileTypeAsBINARY();
	}
	
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
	
	return EXIT_SUCCESS;
}

