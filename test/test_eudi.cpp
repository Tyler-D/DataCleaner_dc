#include"data_cleaner.hpp"
#include<stdlib.h>
#include<time.h>

int main()
{
  int num = 256;

  srand((unsigned)time(NULL));

  float *A = (float*)malloc(sizeof(float)*num);
  float *B = (float*)malloc(sizeof(float)*num);
  float *C = (float*)malloc(sizeof(float)*num);
  float distance_a = 0.0f;
  float answer = 0.0f;

  for(int i = 0; i < num; i++)
  {
    A[i] = 5673.313;
    B[i] = 5684.768;
  }

  for(int i = 0; i< num; i++)
  {
    C[i] = A[i] - B[i];
    answer += C[i]*C[i];
  }
  
  vector<float*> data_pair;
  data_pair.push_back(A);
  data_pair.push_back(B);

  DataCleaner *dc = new DataCleaner("/", num);
  
  dc->get_Eucilidean_distance(num, &data_pair, &distance_a);
  
  if (abs(distance_a - answer) < 0.00001)
    cout<<"right answer! "<<endl;

  
}
