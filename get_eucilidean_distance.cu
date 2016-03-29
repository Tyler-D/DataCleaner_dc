#include "data_cleaner.hpp"

int cuda_check(cudaError_t error){
  if (error != cudaSuccess)
  {
    cout<<"CUDA error : "<<error<<endl;
    exit(0); 
  }
  else 
    return 0; 
}

int cublas_check(cublasStatus_t stat){
  if (stat != CUBLAS_STATUS_SUCCESS)
  {
    cout<<"CUBLAS error: "<<stat<<endl;
    exit(0);
  }
  else
    return 0;
}

__global__ void sub_kernel(int num, const float* A, const float* B, float* C)
{
  for(int index = 0; index < num; index++)
  {
    C[index] = A[index] - B[index];
  }
}

/*
__global__ void dot_kernel(int num, const float* A , const float* B, float* C)
{
    for(int index = 0; index < num; index++)
    {
        (*C)+=A[index]*B[index];
    }
}
*/

DCStatus DataCleaner:: get_Eucilidean_distance(int dim, const vector<float*>* data_pairs, float* distance){
  cudaError_t error;
  cublasStatus_t cublas_stat;
  size_t data_size = sizeof(float)*dim;

  float* dev_ptrA;
  float* dev_ptrB;
  float* dev_ptrC;
  float* dev_result;
  //set the device memory
  error = cudaMalloc((void**)&dev_ptrA,data_size);
  cuda_check(error);
  error = cudaMalloc((void**)&dev_ptrB,data_size);
  cuda_check(error);
  error = cudaMalloc((void**)&dev_ptrC,data_size);
  cuda_check(error);
  error = cudaMalloc((void**)&dev_result,sizeof(float));
  cuda_check(error);
  
  //copy the vector from host to vector
  cublas_stat = cublasSetVector(dim, sizeof(float), (*data_pairs)[0], 1, dev_ptrA, 1);
  cublas_check(cublas_stat);
  cublas_stat = cublasSetVector(dim, sizeof(float), (*data_pairs)[1], 1, dev_ptrB, 1);
  cublas_check(cublas_stat); 

  int threads_per_block = 256;
  int blocks_per_grid = (dim + threads_per_block - 1)/threads_per_block;
  sub_kernel<<<blocks_per_grid, threads_per_block>>>(dim, dev_ptrA,dev_ptrB,dev_ptrC);

  //dot_kernel<<<1024, threads_per_block>>>(dim, dev_ptrC, dev_ptrC, dev_result)
  cublas_stat = cublasSdot(this->cublas_handle_, dim, dev_ptrC, 1, dev_ptrC, 1, distance); 
  cublas_check(cublas_stat);
  
  //cublas_stat = cublasGetVector(dim, sizeof(float), dev_result, 1, distance, 1); 
  //cublas_check(cublas_stat);
  
  cudaFree(dev_ptrA);
  cudaFree(dev_ptrB);
  cudaFree(dev_ptrC);


  return DC_SUCCESS;
}
