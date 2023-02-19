#include<cstdio>
#include<pthread.h>
#include<unordered_map>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#include<iostream>
#include<string.h>
#include<stdlib.h>
#include <time.h>
#include<random>
#include<fstream>
using namespace std;


//semaphore to control sleep and wake up


pthread_mutex_t lock_for_kiosks;
pthread_mutex_t lock_for_belts;
pthread_mutex_t lock_for_boarding;
pthread_mutex_t lock_for_special_kiosk;
pthread_mutex_t lock_for_vip_channel;
pthread_mutex_t lock_for_increment;
pthread_mutex_t lock_for_increment_2;
pthread_mutex_t lock_channel;
pthread_mutex_t lock_print;
pthread_mutex_t lock_array;
int m,n,p,w,x,y,z;
int timecount=1;
int prioritized_passengers=0;
int non_prioritized_passengers=0;
unordered_map<string, int> umap;
bool *arr;
sem_t *belts;
sem_t kiosks;
FILE* ptr;

void VIP_left_to_right(string passenger_id);

void initialize_semaphores() {
	sem_init(&kiosks,0,m);
	arr=new bool[m];
	for(int i=0;i<m;i++) {
		arr[i]=true;
	}
	belts=new sem_t[n];
	for(int i=0;i<n;i++) {
		sem_init(&belts[i],0,p);
	}
	pthread_mutex_init(&lock_for_kiosks,NULL);
	pthread_mutex_init(&lock_for_belts,NULL);
	pthread_mutex_init(&lock_for_boarding,NULL);
	pthread_mutex_init(&lock_for_special_kiosk,NULL);
	pthread_mutex_init(&lock_for_vip_channel,NULL);
	pthread_mutex_init(&lock_for_increment,NULL);
	pthread_mutex_init(&lock_for_increment_2,NULL);
	pthread_mutex_init(&lock_channel,NULL);
	pthread_mutex_init(&lock_print,NULL);
	pthread_mutex_init(&lock_array,NULL);
	
}


void special_kiosk(string passenger_id) {
	string id=passenger_id;
	int local_time=umap.at(id);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started waiting at special kiosk at time %d\n",passenger_id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started waiting at special kiosk at time %d\n",passenger_id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	time_t begin, end;
    time(&begin);
	pthread_mutex_lock(&lock_for_special_kiosk);
	time(&end);
    int elapsed = (int) (end - begin);
	local_time=local_time+elapsed;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started self check in at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started self check in at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	sleep(w);
	local_time=local_time+w;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has finished self_check_in at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has finished self_check_in at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_unlock(&lock_for_special_kiosk);
	umap[id]=local_time;
	VIP_left_to_right(id);
}


void VIP_right_to_left(string passenger_id) {
	string id=passenger_id;
	int local_time=umap.at(id);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started waiting to infront of the VIP channel(R-L) at time %d\n",passenger_id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started waiting to infront of the VIP channel(R-L) at time %d\n",passenger_id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	time_t begin, end;
    time(&begin);
	pthread_mutex_lock(&lock_for_vip_channel);
	time(&end);
    int elapsed = (int) (end - begin);
	local_time=local_time+elapsed;
	pthread_mutex_unlock(&lock_for_vip_channel);
	pthread_mutex_lock(&lock_for_increment_2);
	non_prioritized_passengers++;
	if(non_prioritized_passengers==1) pthread_mutex_lock(&lock_channel);
	pthread_mutex_unlock(&lock_for_increment_2);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started walking through the VIP channel(R-L) at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started walking through the VIP channel(R-L) at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	sleep(z);
	local_time=local_time+z;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has finished walking through the VIP channel(R-L) at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has finished walking through the VIP channel(R-L) at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_lock(&lock_for_increment_2);
	non_prioritized_passengers--;
	if(non_prioritized_passengers==0) {
		//cout<<"condition true"<<endl;
		pthread_mutex_unlock(&lock_channel);
	}
	pthread_mutex_unlock(&lock_for_increment_2);
	umap[id]=local_time;
	special_kiosk(id);

}

void boarding(string passenger_id) {
	string id=passenger_id;
	int local_time=umap.at(id);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started waiting to be boarded at time %d\n",passenger_id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started waiting to be boarded at time %d\n",passenger_id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	time_t begin, end;
    time(&begin);
	pthread_mutex_lock(&lock_for_boarding);
	time(&end);
    int elapsed = (int) (end - begin);
	local_time=local_time+elapsed;
	int loss=rand()%2;
	if(loss==0) {
		pthread_mutex_lock(&lock_print);
		printf("Passenger %s has lost boarding pass,sent to VIP channel at time %d\n",id.c_str(),local_time);
		ptr=fopen("output.txt","a+");
		fprintf(ptr,"Passenger %s has lost boarding pass,sent to VIP channel at time %d\n",id.c_str(),local_time);
		fclose(ptr);
		pthread_mutex_unlock(&lock_print);
		pthread_mutex_unlock(&lock_for_boarding);
		umap[id]=local_time;
		VIP_right_to_left(id);
	}
	else 
	{
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started boarding the plane at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started boarding the plane at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	sleep(y);
	local_time=local_time+y;
	umap[id]=local_time;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has boarded the plane at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has boarded the plane at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_unlock(&lock_for_boarding);
	}

}

void VIP_left_to_right(string passenger_id) {
	string id=passenger_id;
	int local_time=umap.at(id);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started waiting to infront of the VIP channel(L-R) at time %d\n",passenger_id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started waiting to infront of the VIP channel(L-R) at time %d\n",passenger_id.c_str(),local_time);
	fclose(ptr);
    pthread_mutex_unlock(&lock_print);
	
	pthread_mutex_lock(&lock_for_increment);
	prioritized_passengers++;
	if(prioritized_passengers==1) pthread_mutex_lock(&lock_for_vip_channel);
	pthread_mutex_unlock(&lock_for_increment);
	time_t begin, end;
    time(&begin);
	if(non_prioritized_passengers>0) {
		pthread_mutex_lock(&lock_channel);
		pthread_mutex_unlock(&lock_channel);
	}
	time(&end);
    int elapsed = (int) (end - begin);
	local_time=local_time+elapsed;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started walking through the VIP channel(L-R) at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started walking through the VIP channel(L-R) at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	sleep(z);
	local_time=local_time+z;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has finished walking through the VIP channel(L-R) at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has finished walking through the VIP channel(L-R) at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_lock(&lock_for_increment);
	prioritized_passengers--;
	if(prioritized_passengers==0) pthread_mutex_unlock(&lock_for_vip_channel);
	pthread_mutex_unlock(&lock_for_increment);
	umap[id]=local_time;
	boarding(id);
}

void security_check(string passenger_id) {
	string id=passenger_id;
	int local_time=umap.at(id);	
	int rand_no=rand()%n;
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started waiting for security check at belt %d at time %d\n",passenger_id.c_str(),rand_no+1,local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started waiting for security check at belt %d at time %d\n",passenger_id.c_str(),rand_no+1,local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	time_t begin, end;
    time(&begin);
	sem_wait(&belts[rand_no]);
	time(&end);
    int elapsed = (int) (end - begin);
	local_time=local_time+elapsed;
	pthread_mutex_lock(&lock_for_belts);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started the security check at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started the security check at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_unlock(&lock_for_belts);
	sleep(x);
	local_time=local_time+x;
	pthread_mutex_lock(&lock_for_belts);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has crossed the security check at time %d\n",id.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has crossed the security check at time %d\n",id.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_unlock(&lock_for_belts);
	sem_post(&belts[rand_no]);
	umap[id]=local_time;
	boarding(id);
}

void * self_check_in(void * arg)
{	
	int id=*((int*) arg);
	int val;
	int local_time=timecount;
	int vip=rand()%3;
	string str;
	str= to_string(id);
	if(vip==0) {
		pthread_mutex_lock(&lock_print);
		printf("Passenger %d(VIP) has arrived at the airport at time %d\n",id,timecount);
		ptr=fopen("output.txt","a+");
		fprintf(ptr,"Passenger %d(VIP) has arrived at the airport at time %d\n",id,timecount);
		fclose(ptr);
		pthread_mutex_unlock(&lock_print);
		str=str+"(VIP)";
	}
	else  {
		pthread_mutex_lock(&lock_print);
		printf("Passenger %d has arrived at the airport at time %d\n",id,timecount);
		ptr=fopen("output.txt","a+");
		fprintf(ptr,"Passenger %d has arrived at the airport at time %d\n",id,timecount);
		fclose(ptr);
		pthread_mutex_unlock(&lock_print);
	}
	
	time_t begin, end;
    time(&begin);
	sem_wait(&kiosks);
	time(&end);
    int elapsed = (int) (end - begin);
	local_time=local_time+elapsed;
	sem_getvalue(&kiosks,&val);
	pthread_mutex_lock(&lock_for_kiosks);
	int idx=-1;
	pthread_mutex_lock(&lock_array);
	for(int i=0;i<m;i++) {
		if(arr[i]==true) {
			idx=i;
			break;
		}
	}
	arr[idx]=false;
	pthread_mutex_unlock(&lock_array);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has started self check-in in kiosk %d at time %d\n",str.c_str(),idx+1,local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has started self check-in in kiosk %d at time %d\n",str.c_str(),idx+1,local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_unlock(&lock_for_kiosks);
	sleep(w);
	local_time=local_time+w;
	pthread_mutex_lock(&lock_for_kiosks);
	pthread_mutex_lock(&lock_print);
	printf("Passenger %s has finished self check-in at time %d\n",str.c_str(),local_time);
	ptr=fopen("output.txt","a+");
	fprintf(ptr,"Passenger %s has finished self check-in at time %d\n",str.c_str(),local_time);
	fclose(ptr);
	pthread_mutex_unlock(&lock_print);
	pthread_mutex_lock(&lock_array);
	arr[idx]=true;
	pthread_mutex_unlock(&lock_array);
	pthread_mutex_unlock(&lock_for_kiosks);
	sem_post(&kiosks);
	umap[str]=local_time;
	//initialize_semaphores();
	if(vip!=0)security_check(str);
	else VIP_left_to_right(str);
	return nullptr;
}


int main(void)
{	
	
	FILE* file=fopen ("input.txt", "r");
	ptr=fopen("output.txt","w");

	// std::default_random_engine generator;
    // std::poisson_distribution<int> distribution(5);
	std::random_device rd; 
 	std::mt19937 range (rd ()); 

 	double average = 5;
 	double lamda = 1 / average;
 	std::exponential_distribution<double> exp (lamda);
	int num_of_passenger;
	srand(time(0));
	fscanf(file,"%d %d %d %d %d %d %d",&m,&n, &p, &w, &x, &y, &z); 	
	int indexes[1000000];
	initialize_semaphores();
	int i=0;
	while(true) {
		//if(i==20) break;
		//string str= to_string(i+1); 
		pthread_t thread1;
		int *p;
		indexes[i+1]=i+1;
		p=&indexes[i+1];
		pthread_create(&thread1,NULL,self_check_in,(void*)p);
		//int number = distribution(generator);
		int number=exp.operator() (range);
		sleep(number);
		//cout<<number<<endl;
		timecount=timecount+number;
		i++;
	}
	while(1);
	return 0;
}
