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
	typedef float PixelType;
	typedef itk::Mesh<PixelType, dimension>            MeshType;
	typedef itk::QuadEdgeMesh<PixelType, dimension>    QuadEdgeMeshType;
	
	typedef itk::MeshFileReader<MeshType>              MeshFileReaderType;
	typedef itk::MeshFileReader<QuadEdgeMeshType>      QuadEdgeMeshFileReaderType;
	typedef itk::MeshFileWriter<MeshType>              MeshFileWriterType;
	typedef itk::MeshFileWriter<QuadEdgeMeshType>      QuadEdgeMeshFileWriterType;

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
	
	for(MeshType::PointsContainerIterator pt = reader->GetOutput()->GetPoints()->Begin(); pt != reader->GetOutput()->GetPoints()->End(); ++pt)
	{
		std::cout<<"pt = "<<pt->Value()<<std::endl;
	}
	
	for(MeshType::CellsContainerIterator ct = reader->GetOutput()->GetCells()->Begin(); ct != reader->GetOutput()->GetCells()->End(); ++ct)
	{
		std::cout<<"cell index = "<<ct->Index()<<std::endl;
		for(MeshType::CellType::PointIdIterator pt = ct->Value()->PointIdsBegin(); pt != ct->Value()->PointIdsEnd(); ++pt)
		{
			std::cout<<"point id = "<<*pt<<std::endl;
		}
	}
	
	return EXIT_SUCCESS;
}

