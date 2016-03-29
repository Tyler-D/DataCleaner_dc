#include "data_cleaner.hpp"

int main(int argc, char** argv)
{
   if(argc < 4)
   {
     cout<<"Usage:./dc [image_path] [feature_path] [feature_dim]"
         <<" [Eucilidean threshold]"
         <<" [main type rate(0~1)]"<<endl;
     return 0;
   }

   string image_path(argv[1]);
   string feature_path(argv[2]);
   int dim = atoi(argv[3]);  
   float eu_thrs = 0;
   float rate  = 0;
   if (argc > 4)
   {
     eu_thrs = atof(argv[4]);
     if (argc > 5 )
        rate = atof(argv[5]);
   }

   DataCleaner *dc = new DataCleaner(image_path.c_str(), dim);

   if (dc->scan_alimages(dc->root_dir(), dc->next_dir_id()) != DC_SUCCESS)
   {
     cout<<"Scanning all images failed "<<endl;
     return 0;
   }
   else 
     cout<<"Scanning all images success "<<endl;

   if (dc->scan_alfeatures(feature_path.c_str()) != DC_SUCCESS)
   {
     cout<<"Scanning all features failed "<<endl;
     return 0;
   } 
   else 
     cout<<"Scanning all features success"<<endl;

   dc->verify_images(eu_thrs, rate);
}
