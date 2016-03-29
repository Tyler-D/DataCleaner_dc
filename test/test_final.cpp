#include "data_cleaner.hpp"

int main()
{
  DataCleaner *dc = new DataCleaner("/home/caffemaker/data_clean/lfw1", 181);
  if (dc->scan_alimages(dc->root_dir(), dc->next_dir_id()) != DC_SUCCESS)
  {
    return 0;
  }
  else
  {
    cout<<"scan images success"<<endl;
  }
  if (dc->scan_alfeatures("/home/caffemaker/data_clean/feature1") == DC_SUCCESS)
  {
    cout<<"scan features success"<<endl;
  }
  else
  {
    return 0;
  }
  dc->verify_images(0, 0);
}
