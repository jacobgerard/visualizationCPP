#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <sstream>
#include "H5Cpp.h"
using namespace H5;

// Constants
const H5std_string PRESSURE("pressure");
const H5std_string X_VELO("x_velo");
const H5std_string Y_VELO("y_velo");
const H5std_string Z_VELO("z_velo");
const H5std_string VELMAG("velmag");
const int RANK = 3;

// Prototype for write functions
int writeH5(float*,float*,float*,float*,float*,std::string,int*,int);
void writeXdmf(int*,std::string,int);

int main(int argc, char**argv){
  
  int size, rank;

  int latticeType, Num_ts, ts_rep_freq, warmup_ts, plot_freq;
  double Cs, rho_lbm, u_lbm, omega;
  int Nx, Ny, Nz, restartFlag;
  double Lx_p, Ly_p, Lz_p, t_conv_fact, l_conv_fact, p_conv_fact;
  

  std::ifstream input;
  input.open("params.lbm",std::ifstream::in);
  input >> latticeType >> Num_ts >> ts_rep_freq >> warmup_ts >> plot_freq;
  input >> Cs >> rho_lbm >> u_lbm >> omega;
  input >> Nx >> Ny >> Nz >> restartFlag;
  input >> Lx_p >> Ly_p >> Lz_p >> t_conv_fact >> l_conv_fact >> p_conv_fact;
  input.close();

  std::cout << Nx << " " << Ny << " " << Nz << std::endl;
 
  double u_conv_fact = t_conv_fact / l_conv_fact;
  int nDumps = (Num_ts - warmup_ts)/plot_freq + 1;

  int tot = Nx * Ny * Nz;
  int xdims[3] = {Nz,Ny,Nx}; 

  std::string end = ".b_dat";
  std::stringstream s;
  std::string res;

  float *p_dat = new float[tot];
  float *x_dat = new float[tot];
  float *y_dat = new float[tot];
  float *z_dat = new float[tot];
  float *v_dat = new float[tot];

  for(int d = 0; d < nDumps; d++){

    std::string p = "density";
    s << p << d << end;
    res = s.str();
    std::ifstream pr;
    pr.open(res.c_str(),std::ifstream::in|std::ios::binary);
    pr.read((char*)p_dat,tot*4);

    s.str("");
    s.clear();

    std::string x = "ux";
    s << x << d << end;
    res = s.str();
    std::ifstream xv;
    xv.open(res.c_str(),std::ifstream::in|std::ios::binary);
    xv.read((char*)x_dat,tot*4);
  
    s.str("");
    s.clear();

    std::string y = "uy";
    s << y << d << end;
    res = s.str();
    std::ifstream yv;
    yv.open(res.c_str(),std::ifstream::in|std::ios::binary);
    yv.read((char*)y_dat,tot*4);

    s.str("");
    s.clear();

    std::string z = "uz";
    s << z << d << end;
    res = s.str();
    std::ifstream zv;
    zv.open(res.c_str(),std::ifstream::in|std::ios::binary);
    zv.read((char*)z_dat,tot*4);

    s.str("");
    s.clear();


    for(int i = 0; i < tot; i++){
      v_dat[i] = sqrt((x_dat[i]*x_dat[i])+(y_dat[i]*y_dat[i])+(z_dat[i]*z_dat[i])); 
    }
  
    std::cout << "Processing data dump #" << d << std::endl;
    writeH5(p_dat,x_dat,y_dat,z_dat,v_dat,"out.h5",xdims,d);
    writeXdmf(xdims,"data.xmf",d);
  }
 
  delete [] p_dat;
  delete [] x_dat;
  delete [] y_dat;
  delete [] z_dat;
  delete [] v_dat;

  return 0;
}

void writeXdmf(int*dims,std::string filename,int d){
  std::stringstream name;
  std::string start = "data";
  std::string end = ".xmf";
  name << start << d << end;
  start = name.str();

  std::ofstream xdmf;
  xdmf.open(start.c_str(),std::ofstream::out);

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
  xdmf << "out" << d << ".h5:/x_velo\n";
  xdmf << "</DataItem>\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out" << d << ".h5:/y_velo\n";
  xdmf << "</DataItem>\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out" << d << ".h5:/z_velo\n";
  xdmf << "</DataItem>\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Attribute>\n";

  xdmf << "<Attribute Name=\"pressure\" AttributeType=\"Scalar\" Center=\"Node\">\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out" << d << ".h5:/pressure\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Attribute>\n";

  xdmf << "<Attribute Name=\"velocityMagnitude\" AttributeType=\"Scalar\" Center=\"Node\">\n";
  xdmf << "<DataItem Dimensions=\""<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<"\" NumberType=\"Float\" Format=\"HDF\">\n";
  xdmf << "out" << d << ".h5:/velmag\n";
  xdmf << "</DataItem>\n";
  xdmf << "</Attribute>\n";

  xdmf << "</Grid>\n";
  xdmf << "</Domain>\n";
  xdmf << "</Xdmf>\n";

  xdmf.close();
}

int writeH5(float*p,float*u,float*v,float*w,float*vel,std::string filename,int*setup,int d){

  std::stringstream name;
  std::string start = "out";
  std::string end = ".h5";
  name << start << d << end;
  start = name.str();

  H5std_string FILE_NAME(start.c_str());
  // Try block to detect exceptions raised by any of the calls inside it
  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();

    // Create a new file using the default property lists. 
    H5File file(FILE_NAME, H5F_ACC_TRUNC);

    // Create the data space for the dataset.
    hsize_t dims[3];               // dataset dimensions
    dims[0] = setup[0];
    dims[1] = setup[1];
    dims[2] = setup[2];
    DataSpace pressure(RANK, dims);
    DataSpace x_velo(RANK, dims);
    DataSpace y_velo(RANK, dims);
    DataSpace z_velo(RANK, dims);
    DataSpace velmag(RANK, dims);

    // Create the dataset.      
    DataSet p_dataset = file.createDataSet(PRESSURE, PredType::IEEE_F32BE, pressure);
    DataSet x_dataset = file.createDataSet(X_VELO, PredType::IEEE_F32BE, x_velo);
    DataSet y_dataset = file.createDataSet(Y_VELO, PredType::IEEE_F32BE, y_velo);
    DataSet z_dataset = file.createDataSet(Z_VELO, PredType::IEEE_F32BE, z_velo);
    DataSet v_dataset = file.createDataSet(VELMAG, PredType::IEEE_F32BE, velmag);

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
    p_dataset.write(p, PredType::NATIVE_FLOAT);
    x_dataset.write(u, PredType::NATIVE_FLOAT);
    y_dataset.write(v, PredType::NATIVE_FLOAT);
    z_dataset.write(w, PredType::NATIVE_FLOAT);
    v_dataset.write(vel, PredType::NATIVE_FLOAT);

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
