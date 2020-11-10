#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/**
* @file dynamic_analysis.c
* @brief dynamic analysis
*/

/**
* @brief parameter definition 
*/

/**  total STA number */
int sta = 40;
/**  retry limit */
int Lmax = 5;
/**  Imax */
int Imax = 34;
/**  The length of TXOP */
float TXOP = 5.681;
/**  maximum J*/
int Jmax;
/**  minimum J */
int Jmin;
/**  alpha_j_i */
float a_j_i;
/**  RU */ 
int R = 15;
/**  reserved slots */
int K = 10;
/**  delta */
int delta = 0;
/**  number of RA-RU in ith slot */
float Ri[100], total_Ri;
/**  index of for loop */
int i;
/**  index of for loop */
int k;
/**  index of for loop */
int n;
/**  index of for loop */
int m;
/**  index of for loop */
int c;
int Nn,j,Nn_index,a;
/**  temp for calculating Imax */ 
float sum,temp, temp1;
/**  for access delay */
double access_delay;
/**  for access delay */
double counter_access_delay;
/**  for CDF */
double Fm[6];
/**  for CDF */
double total_cdf;
/**  for CDF */
double final_cdf;
/**  OCWn */
int OCW[16];
/**  for success STA, packet */
double success_sta;
/**  for success packet */
double success_packet;
/**  RA-RU in first slot */
float RA_RU = 15.0;
/**  array for M */
double M[100];
/**  array for Mi */
double M_i[100][6];
/**  array for Mis */
double M_i_s[100][6];
/**  array for Mif */
double M_i_f[100][6];
/**  array for MisC*/
double M_i_s_C[100];

/**  access success probability */
double success_probability;

int station_number();

/**
* @brief for calculating the number of success STA, and performance metrics 
*/
int station_number(){
	
	for(i=1;i<=Imax;i++){
		/** \brief Ri = Ri-1 - Mi-1,s + Mi-K,s */
		if(i==1){
			Ri[i] = RA_RU;
		}
		else if(i>1 && i<=K){ 
			Ri[i] = Ri[i-1]-M_i_s_C[i-1]; 
		}				
		else if(i>=(K+1)){
			Ri[i] = Ri[i-1]-M_i_s_C[i-1]+M_i_s_C[i-K]; 
		}
		
		/** \brief Ri for delta */
		if(i<=(delta+1)){
			Ri[i] = RA_RU;
		}
		else if(i>(delta+1) && i<(1+K*(delta+1))){ 
			Ri[i] = Ri[i-delta-1]-M_i_s_C[i-delta-1]; 
		}				
		else if(i>=(1+K*(delta+1))){
			Ri[i] = Ri[i-delta-1]-M_i_s_C[i-delta-1]+M_i_s_C[i-K-K*delta]; 
		}
		//printf("時間=%d, 可用RU數量=%f, 第 %d 次傳輸\n\n",i,Ri[i],n);
		
		for(n=1;n<=Lmax;n++){	
			/** \brief min(2*OCW+1,OCWmax) */ 			
			if(n == 1){
				OCW[n] = 7;
			}
			else if(n == 2){
				OCW[n] = 15;
			}
			else if(n >= 3){
				OCW[n] = 31;
			}
			
			/** \brief initial condition */ 
			if(n==1){				 
				if(i==1 && Ri[i]>=OCW[n]){
					a_j_i = 1;
				}
				else if (i==1 && Ri[i]<OCW[n]){
					a_j_i = (float)((Ri[i]+1)/(OCW[n]+1));
				}
				else if (i>1){
					temp = 0;
					for(c=1;c<=i-1;c++){
						temp += Ri[c];
					}
					if(OCW[n]-temp>=Ri[i]){
						a_j_i = (float)(Ri[i]/(OCW[n]+1));
					}
					else if(OCW[n]-temp<=0){
						a_j_i = 0;
					}
					else{
						a_j_i = (float)((OCW[n]-temp)/(OCW[n]+1));
					}
					
				}				
				M_i[i][n] = a_j_i*sta;
			}		 	
			else if(n<=i){	
				/** \brief Mi[n]=sigma (a_j_i*Mj,F[n-1]) */ 
				for(k=1;k<=(i-1);k++){
					if(k==i-1 && Ri[i]<OCW[n]){
						a_j_i = (float)((Ri[i]+1)/(OCW[n]+1));
					}
					else if (i==1 && Ri[i]>=OCW[n]){
						a_j_i = 1;
					}
					else if(k<i-1){
						temp1 = 0;
						for (c=k+1;c<=i-1;c++){
							temp1 += Ri[c];
						}
						if(OCW[n]-temp1>=Ri[i]){
							a_j_i = (float)(Ri[i]/(OCW[n]+1));
						}
						else if (OCW[n]-temp1<=0){
							a_j_i = 0;
						}
						else{
							a_j_i = (float)((OCW[n]-temp1)/(OCW[n]+1));
						}
					}
					M_i[i][n] += a_j_i * M_i_f[k][n-1];
				}
			}	
			
			M[i] += M_i[i][n];
		}
		/** \brief statistics */  
		for(n=1;n<=Lmax;n++){
			M_i_s[i][n] = (double)M_i[i][n] * exp(-M[i]/Ri[i]);	
			M_i_f[i][n] = 	(double)M_i[i][n] - M_i_s[i][n];
			M_i_s_C[i] += M_i_s[i][n];				

			//printf("final_[%d slot][%d times] success station number=%lf\n",i,n,M_i_s_C[i]);
			//printf("final_[%d slot][%d times] fail station number=%lf\n\n",i,n,M_i_f[i][n]);
			//printf("-----------------------------------------\n");
		}
		/** \brief performance metric */  
		/** \brief success probability */ 
		success_packet += M_i_s_C[i]*K;
		success_sta += M_i_s_C[i];
		success_probability=(double)success_packet/(sta*K);
		//printf("The success probability is %lf, and the total success number of sta is %lf\n\n", success_probability, success_sta);
		//printf("The total success packet is %lf\n\n",success_packet);
		/** \brief access delay */
		//counter_access_delay += M_i_s_C[i]*(i+K-1);
		access_delay=(double)(TXOP*counter_access_delay)/success_sta;
		//printf("The access delay is %lf\n\n",access_delay);
		/** \brief CDF */
		for(m=1;m<=Lmax;m++){
			Fm[m] += M_i_s[i][m];
			if (i==Imax){
				total_cdf += Fm[m];
				final_cdf = (double)total_cdf/success_sta;
				//printf("CDF : Fm[%d]=%lf\n\n",m,final_cdf);
			}			
		}
		/** \brief i v.s. M_i,s[n] */ 
		/* 
		for(n=1;n<=Lmax;n++){
			//printf("M_i_s[i][n] : M_i_s[%d][%d]=%lf\n\n",i,n,M_i_s[i][n]);
			//printf("M_i_f[i][n] : M_i_F[%d][%d]=%lf\n\n",i,n,M_i_f[i][n]);
		}
		*/
		//printf("=============================================================================================\n");
		
	}	
	/** \brief Imax */
	Nn_index = 0;
	Imax = 0;
	for(n=1;n<=Lmax;n++){		
		sum = 0;
		Nn = 0;
		for(c=(Nn_index+1);c<=60;c++){
			if(sum<=OCW[n]){
				Nn++;
				Nn_index++;
				sum += Ri[c];				
			}
			//printf("Nn_index=%d, Nn=%d, sum=%f\n\n",Nn_index,Nn,sum);				
		}
		Imax += Nn;		
	}
	Imax = Imax + (K-1);
	//printf("Imax=%d\n\n\n",Imax);
	return 0;
}

int main(){	

	station_number();
	
	system("pause"); 
	return 0;
}


