double percentilevalue(double* data,int datasize,double percentile,double minval,double maxval){
   unsigned long int* histogram = new unsigned long int[(unsigned long int)floor(maxval-minval)+1];
   for(unsigned int i=0;i<(unsigned long int)floor(maxval-minval);i++)histogram[i]=0;
   for(int i=0;i<datasize;i++)histogram[(unsigned long int)floor(data[i]-minval)]++;
//   for(int i=0;i<datasize;i++)cout << (data[i]-maxval) << endl;
   int temp=0,index=0;
   do{
      temp+=histogram[index];
      index++;
   }while(temp<(percentile*(double)datasize)/100.0);
   double percval=index+minval;
   delete[] histogram;
   return percval;
}
