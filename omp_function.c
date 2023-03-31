void omp_function(int length, int nthreads){
	int i,j, chunk;
	chunk = ceil(length/nthreads);
	for (i=0; i<=length; i++) {
		S[i] = B[i];

		if (0 != S[i])
			D[i] = 1;
		else
			D[i] = 0;
	}
	for (j=0; j<=log2(length); j++) {	

		#pragma omp parallel num_threads(nthreads) shared(S,D,newS,newD) private(i)
		{
			#pragma omp for schedule(dynamic,chunk)
			for (i=1; i<=length; i++) {
				if (S[i] != S[S[i]]) {
					newD[i] = D[i] + D[S[i]];
					newS[i] = S[S[i]];
				} else {
					newD[i] = D[i];
					newS[i] = S[i];
				}
			}
			#pragma omp for schedule(dynamic,chunk)
			for (i=1; i<=length; i++) {
				S[i] = newS[i];
				D[i] = newD[i];
			}	
		}
	}
	
}