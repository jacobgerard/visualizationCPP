#include <iostream>
#include <string>
#include <fstream>
#include "H5Cpp.h"
using namespace H5;

const H5std_string FILE_NAME("out.h5");
const H5std_string PRESSURE("pressure");
const H5std_string X_VELO("x_velo");
const H5std_string Y_VELO("y_velo");
const H5std_string Z_VELO("z_velo");
const H5std_string VELMAG("velmag");
const int N = 27;                     // dataset dimensions
const int RANK = 1;

int writeH5(int*,int*,int*,int*,int*,std::string);
void writeXdmf(int*,std::string);

int main(){
  int data[N];
  for(int i = 0; i < N; i++){
    data[i] = i;
  }
  
  int dims[3] = {3,3,3}; 
  writeH5(data,data,data,data,data,"out.h5");
  writeXdmf(dims,"data.xmf");
}

void writeXdmf(int*dims,std::string filename){
  std::ofstream xdmf;
  xdmf.open("data.xdmf",std::ofstream::out);

  xdmf << "<?xml version=\"1.0\" ?>\n";
  xdmf << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" []>\n";
  xdmf << "<Xdmf xmlns:xi=\"http://www.w3.org/2003/XInclude\" Version=\"2.1\">\n";
  xdmf << "<Domain>\n";

  xdmf << "<Grid Name=\"my_Grid\" GridType=\"Uniform\">\n";
  xdmf << "<Topology TopologyType=\"3DCoRectMesh\" Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\">\n";
  xdmf << "</Topology>\n";

  xdmf << "<Geometry GeometryType=\"Origin_DxDyDz\">\n";
  xdmf << "<DataItem Dimensions=\"3\" NumberType=\"Integer\" Format=\"XML\">\n";
  xdmf << "0 0 0\n"; 
  xdmf << "</DataItem>\n";
  xdmf << "<DataItem Dimensions=\"3\" NumberType=\"Integer\" Format=\"XML\">\n";
  xdmf << "1 1 1\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Geometry>\n";

  xdmf << "<Attribute Name=\"velocity\" AttributeType=\"Vector\" Center=\"Node\">\n";
  xdmf << "<DataItem ItemType=\"Function\" Function=\"JOIN($0, $1, $2)\" Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<" 3\">\n";

  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out.h5:/x_velo\n";
  xdmf << "</DataItem>\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out.h5:/y_velo\n";
  xdmf << "</DataItem>\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out.h5:/z_velo\n";
  xdmf << "</DataItem>\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Attribute>\n";

  xdmf << "<Attribute Name=\"pressure\" AttributeType=\"Scalar\" Center=\"Node\">\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out.h5:/pressure\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Attribute>\n";

  xdmf << "<Attribute Name=\"velocityMagnitude\" AttributeType=\"Scalar\" Center=\"Node\">\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out.h5:/velmag\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Attribute>\n";

  xdmf << "</Grid>\n";
  xdmf << "</Domain>\n";
  xdmf << "</Xdmf>\n";

  xdmf.close();
}

int writeH5(int*p,int*u,int*v,int*w,int*velmag,std::string filename)
{

  std::cout << "please work" << std::endl;
  // Try block to detect exceptions raised by any of the calls inside it
  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();

    // Create a new file using the default property lists. 
    H5File file(FILE_NAME, H5F_ACC_TRUNC);

    // Create the data space for the dataset.
    hsize_t dims[1];               // dataset dimensions
    dims[0] = N;
    DataSpace pressure(RANK, dims);
    DataSpace x_velo(RANK, dims);
    DataSpace y_velo(RANK, dims);
    DataSpace z_velo(RANK, dims);
    DataSpace velmag(RANK, dims);

    // Create the dataset.      
    DataSet p_dataset = file.createDataSet(PRESSURE, PredType::STD_I32BE, pressure);
    DataSet x_dataset = file.createDataSet(X_VELO, PredType::STD_I32BE, x_velo);
    DataSet y_dataset = file.createDataSet(Y_VELO, PredType::STD_I32BE, y_velo);
    DataSet z_dataset = file.createDataSet(Z_VELO, PredType::STD_I32BE, z_velo);
    DataSet v_dataset = file.createDataSet(VELMAG, PredType::STD_I32BE, velmag);

  }  // end of try block
  // catch failure caused by the H5File operations
  catch(FileIException error){
    error.printError();
    return -1;
  }
  // catch failure caused by the DataSet operations
  catch(DataSetIException error){
    error.printError();
    return -1;
  }
  // catch failure caused by the DataSpace operations
  catch(DataSpaceIException error){
    error.printError();
    return -1;
  }


  // Try block to detect exceptions raised by any of the calls inside it
  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();

    // Open an existing file and dataset.
    H5File file(FILE_NAME, H5F_ACC_RDWR);
    DataSet p_dataset = file.openDataSet(PRESSURE);
    DataSet x_dataset = file.openDataSet(X_VELO);
    DataSet y_dataset = file.openDataSet(Y_VELO);
    DataSet z_dataset = file.openDataSet(Z_VELO);
    DataSet v_dataset = file.openDataSet(VELMAG);

    // Write the data to the dataset using default memory space, file
    // space, and transfer properties.
    p_dataset.write(p, PredType::NATIVE_INT);
    x_dataset.write(u, PredType::NATIVE_INT);
    y_dataset.write(v, PredType::NATIVE_INT);
    z_dataset.write(w, PredType::NATIVE_INT);
    v_dataset.write(velmag, PredType::NATIVE_INT);

  }  // end of try block
  // catch failure caused by the H5File operations
  catch(FileIException error){
    error.printError();
    return -1;
  }
  // catch failure caused by the DataSet operations
  catch(DataSetIException error){
    error.printError();
    return -1;
  }

  return 0;  // successfully terminated
}
