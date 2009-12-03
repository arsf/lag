//Returns the value of the given percentile of a dataset. Makes a histogram of the dataset and goes along it until it gets a total count corresponding with the percentile.
double percentilevalue(double* data,int datasize,double percentile,double minval,double maxval){
   unsigned long int* histogram = new unsigned long int[(unsigned long int)floor(maxval-minval)+1];
   for(unsigned int i=0;i<(unsigned long int)floor(maxval-minval);i++)histogram[i]=0;//Fill histogram with zeroes.
   for(int i=0;i<datasize;i++)histogram[(unsigned long int)floor(data[i]-minval)]++;//Increment histogram indeices to give it "shape".
   int temp=0,index=0;
   do{//Determine value of percentile.
      temp+=histogram[index];
      index++;
   }while(temp<(percentile*(double)datasize)/100.0);
   double percval=index+minval;
   delete[] histogram;
   return percval;
}
