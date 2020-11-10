#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/**
* @file dynamic_simulation.c
* @brief dynamic simulation
*/

/**
* @brief parameter definition 
*/

/** total STA number */
int sta = 10; 
/**  number of RU */
int RU = 15;
/**  retry limit */
int Lmax = 5; 
/**  number of RA-RU in each slot for statistic */
float dynamic_RU[80];
/**  Imax */
int slot_time = 50;
/**  number of packets pending to transmit */ 
int packet = 5;
/**  number of RA-RU in each slot */
int packet_RU[80]={0,15};
/**  number of success STA for statistic */
int success_sta, total_success_sta;
/**  number of RA-RU select by STA */
int select_ra_ru; 
/**  for calculating the number of RA-RU in each slot */
int usable_ra_ru;
/**  number of failed STA */
int fail;
/**  OBO counter */
int backoff;
/**  the index of for loop of test number */
int test;
/**  the test number */
int test_number = 100000;
int check_collision = 99;
/**  the index of for loop */
int i;
/**  the index of for loop */ 
int j;
/**  the index of for loop */
int k;
/**  the index of for loop */
int t;
/**  the index of for loop */
int m;
/**  the CDF for statistic */
float total_cdf;
/**  the CDF for statistic */
float test_cdf;
/**  the average access delay for statistic */
float access_delay;
/**  the average access delay for statistic */
float test_access_delay;
/**  the average access delay for statistic */
float first_access_delay;
/**  the average access delay for statistic */
float total_access_delay;
/**  STA[ ][0]:Lmax, STA[ ][1]:OBO, STA[ ][2]:the times of occupying RA-RU */
int STA[101][3], NEW_STA[101][2], RA_RU[15][100], Fm[6], total_Fm[6];  
/**  the number of success STA in each slot */
float M_i_s[80][6];
/**  the number of success STA in each slot */
float M_i_s_for_time[80], s_packet[80];

/**  the number of failed STA in each slot */
float M_i_f[80][6];
/**  the number of failed STA in each slot */
float M_i_f_for_time[80];
/**  the number of contending STA in each slot */
float M_i_for_time[80];
/**  the access success probability for packet for statistic */
float success_packet;
/**  the access success probability for packet for statistic */
float total_success_packet;
/**  the access success probability for packet for statistic */
float test_success_packet;
/**  the access success probability for packet for statistic */
float test_success_packet_probability;
/**  the access success probability for STA for statistic */
float test_success_probability;
/**  the access success probability for STA for statistic */
float success_probability;

int choose_ra_ru(int sta_1, int tt);

/**
* @brief for performimg backoff, select RA-RU, and check collision
*/
int choose_ra_ru(int sta_1, int tt){
	/** \brief dynamic RU */
	for(m=0;m<15;m++){
		if(RA_RU[m][0] == 0){
			usable_ra_ru += 1; 
			packet_RU[tt] = usable_ra_ru;
		}
	}
	for(i=1;i<=sta_1;i++){		
		//printf("\n目前可用的 RU = %d 個\n",packet_RU[tt]);
		/** \brief pre-backoff */
		if(STA[i][0]==0){
			/** \brief OCWmin */
			backoff = (rand()%8); 
			STA[i][1] = backoff;
		}
		else if(STA[i][0]==1 && STA[i][1]>0 && NEW_STA[i][1]<=0){
			backoff = (rand()%16); 
			STA[i][1] = backoff;
		} 
		else if(STA[i][0]>=2 && STA[i][0]<Lmax && STA[i][1]>0 && NEW_STA[i][1]<=0){
			backoff = (rand()%32); 
			STA[i][1] = backoff;
		}		
		//printf("backoff STA[%d]=%d\n",i,STA[i][1]);
		/** \brief backoff number - RU */
		NEW_STA[i][1] = STA[i][1] - packet_RU[tt];	
		//printf("NEW_STA[%d]=%d\n",i,NEW_STA[i][1]);		
	
		if(NEW_STA[i][1] <= 0 && NEW_STA[i][1] > -31 && NEW_STA[i][1] > -(packet_RU[tt])){
			do {	
				/** \brief select a RU randomly */			
				select_ra_ru = (rand()%packet_RU[1]);  
				//printf("select_ra_ru=%d\n",select_ra_ru);
			} while( RA_RU[select_ra_ru][0] != 0 );
			
			/** \brief contention time+1 */
			STA[i][0] += 1; 
			//printf("Transmission times of STA[%d][0] is %d\n",i,STA[i][0]);
			
			/** \brief contention times>Lmax, leave this loop */
			if(STA[i][0] > Lmax) 
			continue;
			
			/** \brief fill the STA in the array of RA-RU */ 
			if(RA_RU[select_ra_ru][1] == 0){
				RA_RU[select_ra_ru][1] = i;
				//printf("RA_RU[%d][1]=%d\n\n",select_ra_ru,RA_RU[select_ra_ru][1]);
			}
			else{
				for(j=check_collision;j>=1;j--){ 
					RA_RU[select_ra_ru][j] = RA_RU[select_ra_ru][j-1];
				}
				RA_RU[select_ra_ru][1] = i;
				//printf("RA_RU[%d][1]=%d\n",select_ra_ru,RA_RU[select_ra_ru][1]);
				//printf("RA_RU[%d][2]=%d\n",select_ra_ru,RA_RU[select_ra_ru][2]);
				//printf("RA_RU[%d][3]=%d\n\n",select_ra_ru,RA_RU[select_ra_ru][3]);								
			}
		}
		else{
			/** \brief keep the backoff number */
			STA[i][1] = NEW_STA [i][1]; 
		}	
	}			
	/** \brief check collision of RA-RU */
	for(i=0;i<packet_RU[1];i++){						
		if(RA_RU[i][2] == 0 && RA_RU[i][1] != 0){ 
			//M_i_for_time[tt] += 1;
			/** \brief the times of occupying the RA-RU */
			STA[RA_RU[i][1]][2] += 1;
			/** \brief success packet */ 
			success_packet += 1; 
			////s_packet[tt] += 1; 
			
			/** \brief the occupying RU=1, the non-occupying RU=0 */
			RA_RU[i][0] = 1; 

			/** \brief calculate the number of success sta and CDF */
			if(STA[RA_RU[i][1]][2] == packet){ 
				success_sta += 1;
				/** \brief CDF */
				for(j=1;j<=Lmax;j++){
					if(STA[RA_RU[i][1]][0]==j){
						Fm[j] += 1;
						M_i_s[tt][j] += 1;
					}
				}
			}
			/** \brief remove the backoff number from the success STA */
			STA[RA_RU[i][1]][1] = 0; 
			
			/** \brief when counter > the number of packet, release this RU */
			if(STA[RA_RU[i][1]][2] >= packet){ 
				/** \brief remove the STA index from the success STA */
				RA_RU[i][1] = 0; 
				/** \brief release the occupying RA-RU */
				RA_RU[i][0] = 0; 
			}
		}	
		else{			
			for(k=1;k<check_collision;k++){ 			 
				if(RA_RU[i][k] != 0){
					fail+=1;
					M_i_f_for_time[tt] += 1;
					for(j=1;j<=Lmax;j++){
						if(STA[RA_RU[i][1]][0]==j){
							M_i_f[tt][j] += 1;
						}
					}
					/** \brief the occupying RU=1, the non-occupying RU=0 */
					RA_RU[i][0] = 0; 
					RA_RU[i][k] = 0;  
				}				
			}
		}
	}
	//printf("------------------re-transmiision sta is %d--------------\n\n",fail);
	//printf("------------------success sta is %d, at %d slot------------------\n\n",success_sta,tt);
	return 0;
}
/**
* @brief main function and calculate the performance metrics
*/
int main(){		
	for (test=1;test<=test_number;test++){
		srand((unsigned)time(NULL));		
		for (t=1;t<=slot_time;t++){
			fail = 0;
			success_sta = 0;
			usable_ra_ru = 0;
			success_packet = 0;
					
			choose_ra_ru(sta, t);
			/** \brief success sta */		
			total_success_sta += success_sta;
			//printf("******************total success sta is %d, at %d slot*****************\n\n",total_success_sta,t);
			//printf("==================fail sta is %d====================\n\n",fail);
			/** \brief success packet */
			total_success_packet += success_packet;		
						
			/** \brief access delay */
			first_access_delay += (float)success_sta*t; 
			access_delay = (float)first_access_delay/total_success_sta; 		
	
			/** \brief Ri */
			dynamic_RU[t] += usable_ra_ru;
			//printf("total_success_sta=%d, access delay=%f, at %d slot.\n\n",total_success_sta,access_delay,t);			
			//printf("================================================================================\n");
		} 	
		for(i=1;i<=sta;i++){
			STA[i][2] = 0;
			STA[i][1] = 0;
			STA[i][0] = 0;
			NEW_STA[i][1] = 0;
		}
		for(i=0;i<packet_RU[1];i++){
			RA_RU[i][0] = 0;
		}
		total_access_delay += access_delay;
	}
	
	/** \brief average access delay */
    test_access_delay=(float)(5.681*total_access_delay)/test_number; 
	/** \brief access success probability */
	test_success_probability=(float)total_success_sta/(sta*test_number); 
	/** \brief success packet */
	test_success_packet = total_success_packet/test_number; 
	/** \brief success packet probability */	
	test_success_packet_probability = total_success_packet/(sta*packet*test_number); 
	/** \brief CDF */
	for(m=1;m<=Lmax;m++){
		total_cdf += Fm[m];
		test_cdf = (float)total_cdf/(total_success_sta);
		//printf("CDF : Fm[%d]=%f\n\n",m,test_cdf);	
	} 
	/** \brief i v.s. M_i,s[n] */
	for(t=1;t<=slot_time;t++){
		dynamic_RU[t] = (float)dynamic_RU[t]/test_number; 		
		for(m=1;m<=Lmax;m++){
			M_i_s[t][m] = (float)M_i_s[t][m]/test_number; 
			M_i_f[t][m] = (float)M_i_f[t][m]/test_number; 	
			M_i_s_for_time[t] += M_i_s[t][m]; 
			 
			//printf("M_i_s[t][m] : t=%d , M_i,s[%d] = %f\n\n",t,m,M_i_s[t][m]);	
			//printf("M_i_f[t][m] : t=%d , M_i,f[%d] = %f\n\n",t,m,M_i_f[t][m]);
		}
		M_i_f_for_time[t] = (float)M_i_f_for_time[t]/test_number;
		//M_i_for_time[t] = (float)(M_i_for_time[t]/test_number);
		////s_packet[t] = (float)s_packet[t]/test_number;
		//printf("第 %d 秒, 可用RU=%f\n\n",t,dynamic_RU[t]);  
		//printf("M_i_s[t] : %f\n",M_i_s_for_time[t]);
		//printf("M_i_f[%d] : %f\n\n",t,M_i_f_for_time[t]);
		//printf("M_i[%d] : %f\n\n",t,M_i_for_time[t]);
		//printf("s_packet[t] : %f\n\n",s_packet[t]);		
		//printf("====================================================\n");
	}

	printf("STA Average access delay=%f\n\n",test_access_delay);
	printf("total success packet probability=%f\n\n",test_success_packet_probability);
	printf("total success packet=%f\n\n",test_success_packet);
	system("pause"); 
	return 0;
}
