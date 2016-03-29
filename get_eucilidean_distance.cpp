#include "data_cleaner.hpp"

void normalize(int dim , const vector<float*>* data_block)
{
  float* a = (*data_block)[0];
  float* b = (*data_block)[1];
  float a_length = 0.0f;
  float b_length = 0.0f;
  for(int i = 0 ; i < dim; i++)
  {
    a_length += a[i]*a[i];
    b_length += b[i]*b[i];
  }
  a_length = sqrt(a_length);
  b_length = sqrt(b_length);
  for(int i = 0; i< dim ; i++)
  {
    a[i] /= a_length;
    b[i] /= b_length;
  }
 
}

void normalize_scal(int dim, const vector<float*>* data_block)
{
  float* a = (*data_block)[0];
  float* b = (*data_block)[1];
  float max_a= 0.0f;
  float max_b= 0.0f;
  float min_a = 10000;
  float min_b = 10000;

  for(int i =0; i < dim; i++)
  {
    if(a[i] > max_a)
     max_a = a[i];
    if(a[i] < min_a)
     min_a = a[i]; 
    if(b[i] > max_b)
     max_b = b[i];
    if(b[i] < min_b)
     min_b = b[i]; 
  }

  float c_a = max_a - min_a;
  float c_b = max_b - min_b;

  for(int i = 0; i< dim ; i++)
  {
    a[i] -= min_a;
    b[i] -= min_b;
    a[i] /= c_a;
    b[i] /=c_b; 
  }

}

float DataCleaner::get_Eucilidean_distance(int dim, const vector<float*>* data_block)
{
  float c = 0.0f;
  float distance = 0.0f;


  normalize(dim, data_block);

  float *a = (*data_block)[0];
  float *b = (*data_block)[1];
  for(int i = 0 ; i < dim; i++)
  {
    c =  a[i] - b[i];
    distance += c * c;
  }
  return distance;
}
