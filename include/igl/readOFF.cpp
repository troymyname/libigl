#include "readOFF.h"
#include "list_to_matrix.h"
#include <cstdio>

template <typename Scalar, typename Index>
IGL_INLINE bool igl::readOFF(
  const std::string off_file_name, 
  std::vector<std::vector<Scalar > > & V,
  std::vector<std::vector<Index > > & F,
  std::vector<std::vector<Scalar > > & N)
{
  FILE * off_file = fopen(off_file_name.c_str(),"r");                                       
  if(NULL==off_file)
  {
    printf("IOError: %s could not be opened...\n",off_file_name.c_str());
    return false; 
  }
  V.clear();
  F.clear();
  N.clear();
  // First line is always OFF
  char header[1000];
  const std::string OFF("OFF");
  const std::string NOFF("NOFF");
  if(!fscanf(off_file,"%s\n",header)==1
     || !(OFF == header || NOFF == header))
  {
    printf("Error: %s's first line should be OFF or NOFF not %s...",off_file_name.c_str(),header);
    fclose(off_file);
    return false; 
  }
  bool has_normals = NOFF==header;
  // Second line is #vertices #faces #edges
  int number_of_vertices;
  int number_of_faces;
  int number_of_edges;
  char tic_tac_toe;
  char line[1000];
  bool still_comments = true;
  while(still_comments)
  {
    fgets(line,1000,off_file);
    still_comments = line[0] == '#';
  }
  sscanf(line,"%d %d %d",&number_of_vertices,&number_of_faces,&number_of_edges);
  V.resize(number_of_vertices);
  if (has_normals)
    N.resize(number_of_vertices);
  F.resize(number_of_faces);
  //printf("%s %d %d %d\n",(has_normals ? "NOFF" : "OFF"),number_of_vertices,number_of_faces,number_of_edges);
  // Read vertices
  for(int i = 0;i<number_of_vertices;)
  {
    float x,y,z,nx,ny,nz;
    if((has_normals && fscanf(off_file, "%g %g %g %g %g %g\n",&x,&y,&z,&nx,&ny,&nz)==6) || 
       (!has_normals && fscanf(off_file, "%g %g %g\n",&x,&y,&z)==3))
    {
      std::vector<Scalar > vertex;
      vertex.resize(3);
      vertex[0] = x;
      vertex[1] = y;
      vertex[2] = z;
      V[i] = vertex;

      if (has_normals)
      {
        std::vector<Scalar > normal;
        normal.resize(3);
        normal[0] = nx;
        normal[1] = ny;
        normal[2] = nz;
        N[i] = normal;
      }
      i++;
    }else if(
             fscanf(off_file,"%[#]",&tic_tac_toe)==1)
    {
      char comment[1000];
      fscanf(off_file,"%[^\n]",comment);
    }else
    {
      printf("Error: bad line in %s\n",off_file_name.c_str());
      fclose(off_file);
      return false;
    }
  }
  // Read faces
  for(int i = 0;i<number_of_faces;)
  {
    std::vector<Index > face;
    int valence;
    if(fscanf(off_file,"%d",&valence)==1)
    {
      face.resize(valence);
      for(int j = 0;j<valence;j++)
      {
        int index;
        if(j<valence-1)
        {
          fscanf(off_file,"%d",&index);
        }else{
          fscanf(off_file,"%d%*[^\n]",&index);
        }
        
        face[j] = index;
      }
      F[i] = face;
      i++;
    }else if(
             fscanf(off_file,"%[#]",&tic_tac_toe)==1)
    {
      char comment[1000];
      fscanf(off_file,"%[^\n]",comment);
    }else
    {
      printf("Error: bad line in %s\n",off_file_name.c_str());
      fclose(off_file);
      return false;
    }
  }
  fclose(off_file);
  return true;
}


#ifndef IGL_NO_EIGEN
template <typename DerivedV, typename DerivedF>
IGL_INLINE bool igl::readOFF(
  const std::string str,
  Eigen::PlainObjectBase<DerivedV>& V,
  Eigen::PlainObjectBase<DerivedF>& F)
{
  std::vector<std::vector<double> > vV;
  std::vector<std::vector<double> > vN;  
  std::vector<std::vector<int> > vF;
  bool success = igl::readOFF(str,vV,vF,vN);
  if(!success)
  {
    // readOFF(str,vV,vF) should have already printed an error
    // message to stderr
    return false;
  }
  bool V_rect = igl::list_to_matrix(vV,V);
  if(!V_rect)
  {
    // igl::list_to_matrix(vV,V) already printed error message to std err
    return false;
  }
  bool F_rect = igl::list_to_matrix(vF,F);
  if(!F_rect)
  {
    // igl::list_to_matrix(vF,F) already printed error message to std err
    return false;
  }
  return true;
}


template <typename DerivedV, typename DerivedF>
IGL_INLINE bool igl::readOFF(
                             const std::string str,
                             Eigen::PlainObjectBase<DerivedV>& V,
                             Eigen::PlainObjectBase<DerivedF>& F,
                             Eigen::PlainObjectBase<DerivedV>& N)
{
  std::vector<std::vector<double> > vV;
  std::vector<std::vector<double> > vN;  
  std::vector<std::vector<int> > vF;
  bool success = igl::readOFF(str,vV,vF,vN);
  if(!success)
  {
    // readOFF(str,vV,vF) should have already printed an error
    // message to stderr
    return false;
  }
  bool V_rect = igl::list_to_matrix(vV,V);
  if(!V_rect)
  {
    // igl::list_to_matrix(vV,V) already printed error message to std err
    return false;
  }
  bool F_rect = igl::list_to_matrix(vF,F);
  if(!F_rect)
  {
    // igl::list_to_matrix(vF,F) already printed error message to std err
    return false;
  }
  
  if (vN.size())
  {
    bool N_rect = igl::list_to_matrix(vN,N);
    if(!N_rect)
    {
      // igl::list_to_matrix(vN,N) already printed error message to std err
      return false;
    }
  }
  return true;
}
#endif


#ifndef IGL_HEADER_ONLY
// Explicit template specialization
// generated by autoexplicit.sh
template bool igl::readOFF<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);
template bool igl::readOFF<Eigen::Matrix<double, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3> >&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> >&);
#endif