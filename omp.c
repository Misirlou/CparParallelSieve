#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <omp.h>
	


void parallel(int cores,long unsigned int n,long unsigned int blocksize){
	 struct timeval start, end;

    	long dt, seconds, useconds;    
	long unsigned int n2=(n+1)/2;
	long unsigned int mallocsize=(n+1)/16;
	char* primos=(char*) calloc( mallocsize,1);

	long unsigned int i,j;
        //printf("%u\n",mallocsize);
	long unsigned int k=sqrt(n)/2+2;
	long unsigned int l,l2;
	blocksize=blocksize/2;
	long unsigned int bi,bn=0;

	omp_set_num_threads(cores);
	int tid,num;

	gettimeofday(&start, NULL);
	

	#pragma omp parallel private (bi,bn,i,l,l2,j)
	{
	
		#pragma omp for
		for (bi=0;bi<n2;bi+=blocksize)
		{
			bn=bi+blocksize;
			for ( i=1;i<k&&i<bn;i++)
			{
				if (!(primos[i/8]&(1<<(i%8)))) {
					l=i*2+1;
					l2=(l*l)/2;
					if (l2<bi) l2+=floor((bi-l2)/l)*l;
					//printf("%lu\n",i*2+1);
					for (j=l2;j<n2&&j<bn;j+=l){ primos[j/8]|= 1 << (j%8);
					//printf("%lu\n",j*2+1);
					}

				}

			}
			//printf("%lu\n",bn);
		}
	}


	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
    	useconds = end.tv_usec - start.tv_usec;

    	dt = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	

	j=1; //1 para o número 2 que não é contabilizado
	for (i=1;i<n2;i++) 
		if (!(primos[i/8]&(1<<(i)%8))) 
		{
			//printf("%lu\n",i*2+1);
			j++;	
		}




	double flops= n*log(log(n))/dt/1000;



	printf("Parallel cores:%d block:%ld\n\tPrimes: %ld\n\tMFlops: %f\n\tTime: %ld\n",cores,blocksize,j,flops,dt);
	//printf("%ld\t",dt);
	free(primos);

}


int main (int argc, char *argv[])
{
	long unsigned int n=4294967396;
	long unsigned int blockSize=4000000;
	int cores=omp_get_max_threads();
	if (argv[1][0]=='b')
	{
		printf("batch time!\n");
		n=16777216; //2^24
		int i;
		for (i=24;i<=33;i++)
		{
			printf("%d\n",i);
			if (cores>=2){
				parallel(2,n,1000000);
				parallel(2,n,2000000);
				parallel(2,n,4000000);
			}else
			{
				parallel(1,n,1000000);
				parallel(1,n,2000000);
				parallel(1,n,4000000);
			}
			if (cores>=4){
				parallel(4,n,1000000);
				parallel(4,n,2000000);
				parallel(4,n,4000000);
			}
			if (cores>=6){
				parallel(6,n,1000000);
				parallel(6,n,2000000);
				parallel(6,n,4000000);
			}
			if (cores>=8){
				parallel(8,n,1000000);
				parallel(8,n,2000000);
				parallel(8,n,4000000);
			}
			n=n*2;		
			printf("\n");	
		}
	}
	else
	{
		if (argc>1) cores=atoi(argv[1]);
		if (argc>2) n=strtol(argv[2], NULL, 0);
		if (argc>3) blockSize=strtol(argv[3], NULL, 0);
		parallel(cores,n,blockSize);
	}
	
}

