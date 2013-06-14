#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>



void parallelMPI(int proci,int numprocs,long unsigned int n,long unsigned int blocksize){
	 struct timeval start, end;

    	long dt, seconds, useconds;    
	long unsigned int n2=(n+1)/2;
	long unsigned int mallocsize=(n+1)/16;
	char* primos=(char*) calloc( mallocsize,1);

	long unsigned int i,j;
        //printf("%u\n",mallocsize);
	blocksize=blocksize/2;
	blocksize=blocksize+(blocksize%8);
	long unsigned int k=sqrt(n)/2+2;
	long unsigned int l,l2;
	long unsigned int bi=blocksize*proci,bn=0,bii;
	long unsigned int nblocks=floor(k/8.0/numprocs);
	

	gettimeofday(&start, NULL);
	
	if (proci==0) {
	//enviar o primeiro bloco de seeds	
		bn=blocksize;
		bi=0;
		for ( i=1;i<k&&i<bn;i++)
		{
			if (!(primos[i/8]&(1<<(i%8)))) {
				l=i*2+1;
				l2=(l*l)/2;
				if (l2<bi) l2+=floor((bi-l2)/l)*l;
				//printf("%lu\n",i*2+1);
				for (j=l2;j<n2&&j<bn;j+=l){ 
					primos[j/8]|= 1 << (j%8);
				//printf("%lu\n",j*2+1);
				}

			}

		}
		MPI_Bcast(primos, blocksize/8 , MPI_CHAR, 0, MPI_COMM_WORLD);
		//printf("broadcast %d to %ld\n",0,blocksize/8);
		bi+=blocksize;
	}
	else{
		//wait for master seeds
		char *data;
		MPI_Bcast(primos, blocksize/8 , MPI_CHAR, 0, MPI_COMM_WORLD);
		bi=blocksize*(proci+1);
	}
	bii=blocksize;
	for (;bi<n2;bi+=blocksize*numprocs)
	{
		bn=bi+blocksize;
		for ( i=1;i<k&&i<bn;i++)
		{
			if (!(primos[i/8]&(1<<(i%8)))) {
				l=i*2+1;
				//printf("%d found %ld \n",proci,l);
				l2=(l*l)/2;
				if (l2<bi) l2+=floor((bi-l2)/l)*l;
				//printf("%lu\n",i*2+1);
				for (j=l2;j<n2&&j<bn;j+=l){ 
					primos[j/8]|= 1 << (j%8);
				//printf("%lu\n",j*2+1);
				}

			}

		}
		//printf("%lu\n",bn);

		//enviar as seeds
		for (i=0;i<numprocs;i++)
		{
			MPI_Bcast(primos+(bii/8), (blocksize)/8 , MPI_CHAR, i, MPI_COMM_WORLD);
			//if (proci==0) printf("broadcast %ld to %ld\n",bii/8,(bii+blocksize)/8);
			bii+=blocksize;
		}
	}

	/*if (proci==0)
	{
		//tem um broadcast em atraso para receber dos outros todos
		for (i=1;i<numprocs;i++)
		{
			MPI_Bcast(primos+(bii/8), (blocksize+bii)/8 , MPI_CHAR, i, MPI_COMM_WORLD);
			bii+=blocksize;	
		}
	}*/


	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
    	useconds = end.tv_usec - start.tv_usec;

    	dt = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	
	if (proci==0){
		j=1; //1 para o número 2 que não é contabilizado
		for (i=1;i<n2;i++) 
			if (!(primos[i/8]&(1<<(i)%8))) 
			{
				//printf("%lu\n",i*2+1);
				j++;	
			}




		double flops= n*log(log(n))/dt/1000;



		//printf("MPI block:%ld\n\tPrimes: %ld\n\tMFlops: %f\n\tTime: %ld\n",blocksize,j,flops,dt);
		printf("%ld\t",dt);
	}	
	free(primos);

}


int main (int argc, char *argv[])
{
	int numprocs,proci;
	MPI_Init(&argc, &argv);	
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&proci);

	
	long unsigned int n=4294967396;
	long unsigned int blockSize=4000000;
	//printf("%d/%d",proci,numprocs);

	if (argv[1][0]=='b')
	{
		printf("batch time!\n");
		n=16777216; //2^24
		int i;
		for (i= 24;i<=33;i++)
		{
			parallelMPI(proci,numprocs,n,1000000);
			parallelMPI(proci,numprocs,n,2000000);
			parallelMPI(proci,numprocs,n,4000000);
			parallelMPI(proci,numprocs,n,6000000);
			printf("\n");
			n=n*2;
		}
	}
	else
	{
		if (argc>1) n=strtol(argv[1], NULL, 0);
		if (argc>2) blockSize=strtol(argv[2], NULL, 0);
		parallelMPI(proci,numprocs,n,blockSize);
	}
	MPI_Finalize();
}
