#include "data_cleaner.hpp"
#include <algorithm>

DCStatus DataCleaner::scan_alimages(const char* path, int dir_id)
{ 
    DIR *root_dp;
    struct dirent *d_info;
    struct stat d_stat;   
    if ((root_dp = opendir(path)) == NULL)
    {
      cout<<"DataCleaner::scan_alimages: Open directory failed"<<endl;
      return DC_FAILED;
    }

    while((d_info = readdir(root_dp)) != 0)
    {
      if (!strcmp(d_info->d_name, ".") || (!strcmp(d_info->d_name, "..")))
        continue;
      
      string temp_path(path);
      temp_path += '/';
      temp_path += d_info->d_name;
  
      if (lstat(temp_path.c_str(), &d_stat) < 0)
      {
        cout<<"DataCleaner::scan_alimages: lstat file failed"<<endl;
        return DC_FAILED;
      }
      else
      {
        if(S_ISREG(d_stat.st_mode))
        {
            if (dir_id == -1)
            {
              cout<<"wrong root dir"<<endl;
              return DC_FAILED;
            } 
            string temp_image(d_info->d_name);
            this->images_[dir_id].push_back(temp_image);
        }
        else if (S_ISDIR(d_stat.st_mode))
        {
          string temp_image_dir(d_info->d_name);
          this->next_dir_id_++;
          this->image_dir_.push_back(make_pair(this->next_dir_id_,temp_image_dir));

          vector<string> new_dir;
          vector<ImgFeature*> new_feature_dir;
          vector<int> new_representative_dir;
          this->images_.push_back(new_dir);
          this->image_features_.push_back(new_feature_dir);
          this->image_representative_.push_back(new_representative_dir);

         if (scan_alimages(temp_path.c_str(),this->next_dir_id_) != DC_SUCCESS)
         {
           return DC_FAILED;
         }
        }
        else
        {
          cout<<"DataCleaner::scan_files: Wrong type file"<<endl;
        }
      }
    }
    closedir(root_dp);
    return DC_SUCCESS; 
}


DCStatus DataCleaner::scan_features_binary(const char* path, int dir_id)
{
  DIR *dp;
  struct dirent *d_info;
  struct stat d_stat;
  int feature_fd;

  if((dp = opendir(path)) == NULL)
  {
    cout<<"DataCleaner::scan_features: Open file failed"<<endl;
    return DC_FAILED;
  }

  while((d_info = readdir(dp)) != 0)
  {
      if (!strcmp(d_info->d_name, ".") || (!strcmp(d_info->d_name, "..")))
          continue;
      
      string  temp_file_path(path);
      temp_file_path +='/';
      temp_file_path += d_info->d_name;

      if (lstat(temp_file_path.c_str(), &d_stat) < 0)
      {
        cout<<"DataCleaner::scan_features: lstat file failed"<<endl;
        return DC_FAILED;
      }

      
      if (S_ISREG(d_stat.st_mode))
      {
        if ((feature_fd = open(temp_file_path.c_str(), O_RDONLY)) == -1)
        {
          cout<<"DataCleaner::scan_features: open file failed"<<endl;
          return DC_FAILED;
        }
        
        size_t data_size = sizeof(float)*this->dim_;
        void* feature_buf = malloc(data_size);
        ssize_t  nread;

        while (data_size > 0)
        {
          nread = read(feature_fd, feature_buf, data_size);
          if (nread < 0)
          {
            cout<<"DataCleaner::scan_features:read file failed"<<endl;
            return DC_FAILED;
          }
          else if (nread == 0)
          {
            if (data_size > 0)
            {
              cout<<"DataCleaner::scan_features:wrong data structure"<<endl;
              return DC_FAILED;
            }
          }
          else
          {
            data_size -= nread;
          }
        } 
        this->image_features_[dir_id].push_back(new ImgFeature(this->dim_, (float*)feature_buf));
        
      }
      else
      {
        cout<<"DataCleaner::scan_features: wrong file type"<<endl;
        return DC_FAILED;
      }  
      
      return DC_SUCCESS;
  }
}

DCStatus DataCleaner::scan_features(const char* path, int dir_id)
{
  int image_num = this->images_[dir_id].size();
  int data_size = this->dim_;
  vector<string> images = this->images_[dir_id];

  for(int i = 0; i < image_num ; i++ )
  {
      
    string  temp_file_path(path);
    temp_file_path +='/';
    temp_file_path += images[i].c_str();
    temp_file_path.erase(temp_file_path.end()-3, temp_file_path.end()); 
    temp_file_path +="txt";

    FILE *feature_fp;
    errno = 0;
    if ((feature_fp = fopen(temp_file_path.c_str(), "r")) == NULL)
    {
      cout<<"DataCleaner::scan_features: open file failed : "<<errno<<endl;
      if (errno == 2)
        cout<<"No file : "<<temp_file_path<<endl;

      return DC_FAILED;
    }
      
    float* feature_buf = (float*)malloc(sizeof(float)*data_size);

    for(int j = 0; j< data_size; j++)
    {
      fscanf(feature_fp, "%f", &feature_buf[j]);
    }

    fclose(feature_fp);
    this->image_features_[dir_id].push_back(new ImgFeature(this->dim_, (float*)feature_buf));
        
  }
  return DC_SUCCESS;
}

DCStatus DataCleaner::scan_alfeatures(const char* path)
{
    int dir_num = this->image_dir_.size();
    cout<< dir_num <<endl;
    for(int i = 0; i < dir_num ; i++)
    {
      string temp_path(path);
      temp_path += '/';
      temp_path += (this->image_dir_[i].second).c_str();
      if (scan_features(temp_path.c_str(), this->image_dir_[i].first) != DC_SUCCESS)
      {
        cout<<"Finished " << i <<" "<< this->image_dir_[i].first<<" features scanning"<<endl;
        return DC_FAILED;
      }
    }
    return DC_SUCCESS;
}

// v0.1 clean_files: move images to deprecated 
DCStatus DataCleaner::clean_files(const vector<bool> files_label, const int dir_id)
{
  int files_num = files_label.size();
  int erro;
  string temp_path(this->root_dir_);
  temp_path +='/';
  temp_path += (this->image_dir_[dir_id].second).c_str();
  temp_path += '/';
  string temp_path_old(temp_path.c_str());
  temp_path += "deprecated";
  if ((erro = mkdir(temp_path.c_str(),0777)) < 0)
  {
    cout<<"DataCleaner::clean_files: mkdir failed"<<endl;
    exit(0);
  }
  temp_path += '/';
  string temp_path_new(temp_path.c_str());
  for(int i = 0; i < files_num ; i++)
  {
    if (!files_label[i])
    {
      string file_path_old(temp_path_old.c_str());
      file_path_old += this->images_[dir_id][i].c_str();
      string file_path_new(temp_path_new.c_str());
      file_path_new += this->images_[dir_id][i].c_str();
      if((erro = rename(file_path_old.c_str(), file_path_new.c_str())) < 0)
      {
        cout<<"DataCleaner::clean_files: rename failed"<<endl;
      }
    }
  }
}

//v0.2 

DCStatus DataCleaner::verify_images(float threshold, float rate){
  //verify images in each image directory
  float eu_threshold = 0.0f;
  float main_rate = 0.0f;
  if ((rate > 0)&&( rate <= 1))
  {
    main_rate = rate;
  }
  else
  {
    main_rate = DEFAULT_RATE;
  }

  if (threshold > 0)
  {
    eu_threshold = threshold;
  } 
  else
  {
    eu_threshold = DEFAULT_THRESHOLD;
  }

  int dir_num = image_dir_.size(); 
  int total_num = 0; 
  int deprecated_num = 0;
  for(int image_dir_id = 0; image_dir_id < dir_num; image_dir_id++)
  {
    /*compare the similarity between each pair of images through 
    Eucilidean distance*/
    int image_num = this->image_features_[image_dir_id].size();
    vector<int> image_simival(image_num, 0); 
    total_num += image_num;
    for(int i = 0; i < image_num; i++)
    {
      float* base = image_features_[image_dir_id][i]->data_;
      for(int j = i+1; j < image_num; j++)
      {
        float distance = 0.0f;
        float* correspond = image_features_[image_dir_id][j]->data_; 
        vector<float*> compute_data;
        compute_data.push_back(base);
        compute_data.push_back(correspond); 
        distance = get_Eucilidean_distance(this->dim_, &compute_data);
        cout<<this->images_[image_dir_id][i]<<" and "<<this->images_[image_dir_id][j]<< " distance :"<<distance<<endl;
        if (distance >= eu_threshold)
        {
           continue;
        }
        else
        {
          image_simival[i]++; 
          image_simival[j]++;
        }

      }
    }

    //for(int i = 0; i< image_num ; i++)
    //{
      //cout<<this->images_[image_dir_id][i]<<": "<<image_simival[i]<<endl;
    //}

    vector<bool> image_labels(image_num, false);
    //Distinguish by the simival
    int simi_threshold = image_num * main_rate;
    int temp_del = image_num;
    for(int i = 0; i < image_num ; i++)
    {
      if (image_simival[i] >= simi_threshold)
      {
        image_labels[i] = true;
        temp_del --;
      }
    }
    deprecated_num += temp_del; 
    // clean the directory
    this->clean_files(image_labels, image_dir_id);

    //figure out the representative 
    vector<int> image_simi_temp(image_simival);
    for(int i = 0 ; i < MINIMUM_DIRECTORY_SIZE; i++)
    {
      this->image_representative_[image_dir_id].push_back(
      std::distance(image_simi_temp.begin(), std::max_element(
         image_simi_temp.begin(), image_simi_temp.end())));
    }
    
  }//end "for" search each image directory 
  cout<<"Total number of images : "<<total_num<<endl;
  cout<<"Deprecated images : "<<deprecated_num<<endl;
}

DCStatus DataCleaner::merge_directors()
{

}

DCStatus DataCleaner::inter_direct_verification()
{
  

} 
