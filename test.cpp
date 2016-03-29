#include"data_cleaner.hpp"
#include<math.h>
#include<sys/time.h>

int main()
{
  FILE *fp;
  int num = 256;
  
  fp = fopen("/home/caffemaker/data_clean/asian_train_features.txt","r");


  vector<float*> feature_buf;
  for(int i = 0; i < 19583 ; i++)
  {
    float* feature = (float*)malloc(sizeof(float)*num);
    for(int j = 0; j < num; j++)
    {
      fscanf(fp, "%f", &feature[j]);
    }
    feature_buf.push_back(feature);
  }
  
  //FILE *fp; 
  //fp = fopen("/home/caffemaker/data_clean/a.txt", "w");
  /*
  for(int i = 0; i < 100 ; i++)
  {
    for(int j = 0; j < num ; j++)
    {
      cout<<feature_buf[i][j]<<" ";
    }
    cout<<endl;
  }
  */
  /*  
  for(int i = 0 ; i< 5; i++)
  {
    for(int j = 245; j <256 ; j++)
      cout<<feature_buf[i][j]<<" ";
    cout<<endl;
  }
 */

  DataCleaner *dc = new DataCleaner("/", num);
  vector<float*> data_pair;
  data_pair.resize(2);
  data_pair[0] = feature_buf[0];
  int same = 0;
  int top[3] = {0,0,0};
  float val[3] = {10000,10000,10000};
    

  timeval s,e;
  gettimeofday(&s, NULL); 
  for(int i = 1; i< 19583 ; i++)
  { 
    float distance = 0.00f;
    data_pair[1] = feature_buf[i];
//    for(int j = 0; j< 10; j++)
  //  {
    //  cout<<data_pair[1][j]<<" "; 
   // }
   // cout<<endl;
  //  float di[256]; 
    // for(int j = 0; j < 256; j++)
     //{
       // di[j] = feature_buf[0][j] - feature_buf[i][j];
       // distance += di[j];
    // }


   distance = dc->get_Eucilidean_distance(num, &data_pair);
  
   distance = sqrt(distance);
   cout<<"distance : "<<distance <<endl;
   // for(int j = 0; j < 3 ; j++)
   // {
    
      if(distance <= val[0])
      {
        top[0] = i;
        val[0] = distance;
        continue;
      }
      else if (distance <= val[1])
      {
        top[1] = i;
        val[1] = distance;
        continue;

      }
      else if (distance <= val[2])
      {
      
        top[2] = i;
        val[2] = distance;
        continue;
      }
   // }
   
    //if (distance > 0.5)
    //{
      //cout << "Different Persons "<<endl;
      //return 0;
    //}
    //else
    //{
  //    cout<< "Same Person !"<<distance<<endl;
  //  }

  }
  gettimeofday(&e, NULL);
  double timeuse = 1000000*(e.tv_sec - s.tv_sec) + e.tv_usec - s.tv_usec;
  timeuse /= 1000;
  timeuse /= 19583;
  cout<<"Average Time Use : "<<timeuse<<endl;
 
  for(int i = 0; i < 3 ;i ++)
  {
    cout<<top[i]<<" "<< val[i] <<endl;
  }
 
 
}
  

