#ifndef DATA_CLEANER_HEAD
#define DATA_CLEANER_HEAD
#include<iostream>
#include<vector>
#include<string.h>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>
#include<unistd.h>
#include<utility>
#include<errno.h>
using namespace std;

#define DEFAULT_THRESHOLD 1
#define DEFAULT_RATE 0.5
#define MINIMUM_DIRECTORY_SIZE 10

//cuda things
#include<cublas_v2.h>
#include<cuda_runtime.h>

typedef enum{DC_FAILED, DC_SUCCESS} DCStatus;

class ImgFeature{
  public:
    ImgFeature(int d):dim_(d){
        data_ = (float*)malloc(sizeof(float)*dim_);
    };
    ImgFeature(int d, float* data){
      dim_ = d;
      data_ = data;
    }
    int dim_;
    float* data_;
};

class DataCleaner{
  public:
    explicit DataCleaner(const char* path, int d): root_dir_(path), dim_(d),next_dir_id_(-1)  {
      // cublasStatus_t stat;

      // stat = cublasCreate(&cublas_handle_);
      // if(stat != CUBLAS_STATUS_SUCCESS){
        // cout<<"cublas handle initial failed"<<endl;
       //}
    };
    virtual ~DataCleaner() {};
    //recursively scan all the imags  
    //begining from the root_dir
    DCStatus scan_alimages(const char* path, int dir_id);
    //get images' features and put it into the
    //relative files. the way to get can be based
    //on any features extracting methods   
    virtual void get_image_features() {};
    //to deal with input features
    DCStatus scan_alfeatures(const char* path);
    DCStatus scan_features_binary(const char* path, int dir_id);
    DCStatus scan_features(const char* path, int dir_id);
    //to caculate the eucilidean distance 
    float get_Eucilidean_distance(int dim, const vector<float*>* data_block);
    //verify the images 
    DCStatus clean_files(const vector<bool> files_label, const int dir_id);
    DCStatus verify_images(float threshold,float rate);
    DCStatus pca_verify_images() {};
    DCStatus inter_direct_verification();
    DCStatus merge_directors();
    const char* root_dir() {return this->root_dir_; };
    int next_dir_id(){return this->next_dir_id_; };
    int dim() {return this->dim_;};
  private:
    //the root directory of the images and features
    const char* root_dir_;
    //the feaures' dimension
    int dim_;
    //the number of images directory
    int next_dir_id_;
    //the directory of the single type images
    vector<pair<int,string> > image_dir_;
    vector<vector<string> > images_;
    //the primal features of different types
    vector<vector<ImgFeature*> > image_features_;
    //the directory representative
    vector<vector<int> > image_representative_;
    cublasHandle_t cublas_handle_;
};
#endif
