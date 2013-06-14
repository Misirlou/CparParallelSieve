#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
	

void single1bit(long unsigned int n){
	 struct timeval start, end;

    	long dt, seconds, useconds;    
	long unsigned int n2=(n+1)/2;
	unsigned int mallocsize=(n+1)/16;
	char* primos=(char*) calloc( mallocsize,1);

	long unsigned int i,j;
        //printf("%u\n",mallocsize);
	long unsigned int k=sqrt(n)/2+2;
	long unsigned int l;
	gettimeofday(&start, NULL);

	for ( i=1;i<k;i++)
	{
		if (!(primos[i/8]&(1<<(i%8)))) {
			l=i*2+1;
			//printf("%lu\n",i*2+1);
			for (j=l*l;j<n;j+=2) if(j%l==0) primos[j/16]|= 1 << ((j/2)%8);

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



	printf("Single1bits\n\tPrimes: %ld\n\tMFlops: %f\n\tTime: %ld\n",j,flops,dt);
	//printf("%ld\t",dt);
	free(primos);

}

void single2bit(long unsigned int n){
	 struct timeval start, end;

    	long dt, seconds, useconds;    
	long unsigned int n2=(n+1)/2;
	long unsigned int mallocsize=(n+1)/16;
	char* primos=(char*) calloc( mallocsize,1);

	long unsigned int i,j;
        //printf("%u\n",mallocsize);
	long unsigned int k=sqrt(n)/2+2;
	long unsigned int l;
	gettimeofday(&start, NULL);

	for ( i=1;i<k;i++)
	{
		if (!(primos[i/8]&(1<<(i%8)))) {
			l=i*2+1;
			//printf("%lu\n",i*2+1);
			for (j=l*l/2;j<n2;j+=l) primos[j/8]|= 1 << (j%8);

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



	printf("Single2bits\n\tPrimes: %ld\n\tMFlops: %f\n\tTime: %ld\n",j,flops,dt);
	//printf("%ld\t",dt);	
	free(primos);

}

void single3bit(long unsigned int n,long unsigned int blocksize){
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
	gettimeofday(&start, NULL);

	
	for (bi=0;bi<n2;bi+=blocksize)
	{
		bn+=blocksize;
		//printf("%lu\n",bn);
		for ( i=1;i<k&&i<bn;i++)
		{
			if (!(primos[i/8]&(1<<(i%8)))) {
				l=i*2+1;
				l2=(l*l)/2;
				if (l2<bi) l2+=floor((bi-l2)/l)*l;
				//printf("%lu\n",i*2+1);
				for (j=l2;j<n2&&j<bn;j+=l){ primos[j/8]|= 1 << (j%8);
				//printf("\t%lu\n",j*2+1);
				}

			}

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



	printf("Single3bits block:%ld\n\tPrimes: %ld\n\tMFlops: %f\n\tTime: %ld\n",blocksize*2,j,flops,dt);
	//printf("%ld\t",dt);
	free(primos);

}



int main (int argc, char *argv[])
{
	long unsigned int n=4294967396;
	long unsigned int blockSize=4000000;
	if (argv[1][0]=='b')
	{
		printf("batch time!\n");
		n=16777216; //2^24
		int i;
		for (i=24;i<=33;i++)
		{
			printf("%d\n",i);
			if (i<27) single1bit(n);
			else printf("0\t");
			single2bit(n);
			single3bit(n,500000);
			single3bit(n,1000000);
			single3bit(n,2000000);
			single3bit(n,3000000);
			single3bit(n,4000000);
			printf("\n");
			n=n*2;			
		}
	}
	else
	{
		if (argc>1) n=strtol(argv[1], NULL, 0);
		if (argc>2) blockSize=strtol(argv[2], NULL, 0);
		
		single2bit(n);
		single3bit(n,blockSize);
		single1bit(n);
		printf("\n");
	}
}

