#include"data_cleaner.hpp"

int main()
{
  DataCleaner *dc = new DataCleaner("/home/caffemaker/data_clean/test_k", 1); 
  
  dc->scan_alimages(dc->root_dir(), dc->next_dir_id());
}
