#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int k, i, j;
	int m, n;
	double **r, **q, **a, mysum;

	m = 5;
	n = 5;

	a = malloc(sizeof(double *) * m);
	q = malloc(sizeof(double *) * m);
	r = malloc(sizeof(double *) * n);

	for (i = 0; i < m; i++) {
		a[i] = malloc(sizeof(double) * n);
		q[i] = malloc(sizeof(double) * n);
		for (j = 0; j < n; j++)
			a[i][j] = (i+1)*(j+1) + 10;
	}
	for (i = 0; i < n; i++) {
		r[i] = malloc(sizeof(double) * n);
	}
	for (k = 0; k < n; k++){
		r[k][k] = 0;
		mysum = 0;
		#pragma omp parallel for private(i) reduction(+:mysum) schedule(static) 
		for (i = 0; i < m; i++)
			mysum += a[i][k] * a[i][k];
		r[k][k] = mysum;
		r[k][k] = sqrt(r[k][k]);
		#pragma omp parallel for private(i) shared(k, r) schedule(static) 
		for (i = 0; i < m; i++)
			q[i][k] = a[i][k] / r[k][k];
	 	#pragma omp parallel for private(j, mysum) shared(k, r) 
		for(j = k + 1; j < n; j++) {
			mysum = 0;
			#pragma omp parallel for private(i) shared(k, j) reduction(+:mysum) schedule(static)
			for(i = 0; i < m; i++)
				mysum += q[i][k] * a[i][j];
			r[k][j] = mysum;
			#pragma omp parallel for private(i) shared(k, j) schedule(static)
			for (i = 0; i < m; i++)
				a[i][j] = a[i][j] - r[k][j] * q[i][k];
		}
	}

/*

	printf("a = \n");

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%lf ", a[i][j]);
		}
		printf("\n");
	}

	printf("\n\n");

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%lf ", q[i][j]);
		}
		printf("\n");
	}

	printf("\n\n");

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%lf ", r[i][j]);
		}
		printf("\n");
	}
		
	printf("\n\n");

*/
	for (i = 0; i < m; i++) {
		free(a[i]);
		free(q[i]);
	}
	for (i = 0; i < n; i++) {
		free(r[i]);
	}
	free(a);
	free(q);
	free(r);
	return 0;
}
