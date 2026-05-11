#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SEATS 80 
#define TICKET_FILE "tickets.txt"
#define CYAN "\033[1;36m"
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define PINK "\033[1;35m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

struct Train{
    int id;
    char name[50];
    char from[30];
    char to[30];
    int total_seats;
    float fare;
    char departure[10];
    int platform;
};

struct Ticket{
    int booking_id;
    int pnr;
    int train_id;
    char passenger_name[50];
    int age;
    char gender;
    int seat_number;
    char status[15];
    char emergency_contact[15];
};

char seat_map[MAX_SEATS+1];

void line();
void smallLine();
void pauseMsg();
void loader(char msg[]);
void viewTrains();
void bookTicket();
void viewBookings();
void cancelTicket();
void searchTrain();
void showSeatMap();
void showTicketByID();
void loadSeatMap(int train_id);
void getCoachName(int seat,char coach[]);
void printTicket(struct Ticket t,struct Train *tr);
void bookingProcess(int train_id);

int generateBookingID();
int generatePNR();
int isDuplicateBooking(char name[],int train_id);
int isWindowSeat(int seat);
int isSandwiched(int seat);
int allocateSeat(char gender,int age,int want_window);
int getDiscountPercent(int age);
int countBookedSeats(int train_id);

float calculateFare(float baseFare,int age);
struct Train* findTrain(int id);

struct Train trains[5]={
    {101,"Rajdhani Express","Delhi","Mumbai",120,850.0,"06:00",3},
    {102,"Shatabdi Express","Chennai","Bangalore",120,450.0,"07:30",1},
    {103,"Duronto Express","Kolkata","Delhi",120,700.0,"22:00",5},
    {104,"Garib Rath","Mumbai","Pune",120,120.0,"09:15",2},
    {105,"Jan Shatabdi","Delhi","Agra",120,200.0,"08:00",4}
};

int trainCount=5;

int main(){
    int choice;
    do{
        printf(CYAN "\n+--------------------------------------+\n" RESET);
        printf(CYAN "|   TRAIN SEAT ALLOCATION SYSTEM       |\n" RESET);
        printf(CYAN "+--------------------------------------+\n" RESET);
        printf("1. View All Trains\n");
        printf("2. Search Train\n");
        printf("3. Book Ticket\n");
        printf("4. View Bookings\n");
        printf("5. Cancel Ticket\n");
        printf("6. View Seat Map\n");
        printf("7. Show Ticket\n");
        printf("8. Exit\n");
        printf("\nEnter choice: ");
        while(scanf("%d",&choice) != 1 || (choice < 1 || choice > 8)){
            while(getchar() != '\n');
            printf(RED "[!] Invalid input. Enter 1-8.\n" RESET);
            printf("Enter choice: ");
        }
        getchar();
        switch(choice){
            case 1:viewTrains();pauseMsg();break;
            case 2:searchTrain();pauseMsg();break;
            case 3:bookTicket();pauseMsg();break;
            case 4:viewBookings();pauseMsg();break;
            case 5:cancelTicket();pauseMsg();break;
            case 6:showSeatMap();pauseMsg();break;
            case 7:showTicketByID();pauseMsg();break;
            case 8:printf(GREEN "\nThank you.\n" RESET);break;
            default:printf(RED "[!] Invalid choice.\n" RESET);pauseMsg();
        }
    }while(choice!=8);
    return 0;
}

void line(){
    printf(CYAN "\n----------------------------------------\n" RESET);
}

void smallLine(){
    printf("----------------------------------------\n");
}

void pauseMsg(){
    printf(CYAN "\n----------------------------------------\n" RESET);
    printf("Press Enter to return to menu...");
    fflush(stdout);
    getchar();
}

void loader(char msg[]){
    printf("%s.\n",msg);
    fflush(stdout);
}

void loadSeatMap(int train_id){
    int i;
    struct Ticket t;
    FILE *fp;
    for(i=1;i<=MAX_SEATS;i++) seat_map[i]='0';
    fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) return;
    while(fscanf(fp,"%d %d %d %s %d %c %d %s %s",
        &t.booking_id,&t.pnr,&t.train_id,t.passenger_name,
        &t.age,&t.gender,&t.seat_number,t.status,t.emergency_contact)==9){
        if(t.train_id==train_id && strcmp(t.status,"CONFIRMED")==0 &&
           t.seat_number>=1 && t.seat_number<=MAX_SEATS){
            seat_map[t.seat_number]=t.gender;
        }
    }
    fclose(fp);
}

int generateBookingID(){
    int count=1001;
    char line[200];
    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) return count;
    while(fgets(line,sizeof(line),fp)) count++;
    fclose(fp);
    return count;
}

int generatePNR(){
    int count=100000;
    char line[200];
    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) return count;
    while(fgets(line,sizeof(line),fp)) count++;
    fclose(fp);
     printf("Processing.....\n");
    return count;
}

int isDuplicateBooking(char name[],int train_id){
    FILE *fp=fopen(TICKET_FILE,"r");
    struct Ticket t;
    if(fp==NULL) return 0;
    while(fscanf(fp,"%d %d %d %s %d %c %d %s %s",
        &t.booking_id,&t.pnr,&t.train_id,t.passenger_name,
        &t.age,&t.gender,&t.seat_number,t.status,t.emergency_contact)==9){
        if(strcmp(t.passenger_name,name)==0 &&
           t.train_id==train_id &&
           strcmp(t.status,"CONFIRMED")==0){
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int getDiscountPercent(int age){
    if(age<12) return 50;
    else if(age>60) return 20;
    else return 0;
}

float calculateFare(float baseFare,int age){
    int discount=getDiscountPercent(age);
    return baseFare-(baseFare*discount/100);
}

void getCoachName(int seat,char coach[]){
    if(seat<=25) strcpy(coach,"S1");
    else strcpy(coach,"S2");
}

int isWindowSeat(int seat){
    return seat%3==1;
}

int isSandwiched(int seat){
    char left,right;
    if(seat<=1 || seat>=MAX_SEATS) return 0;
    left=seat_map[seat-1];
    right=seat_map[seat+1];
    if(left!='0' && right!='0') return 1;
    return 0;
}

int allocateSeat(char gender,int age,int want_window){
    int seat;
    if(age<12){
        for(seat=1;seat<=10 && seat<=MAX_SEATS;seat++){
            if(seat_map[seat]=='0') return seat;
        }
    }
    if(age>60){
        for(seat=1;seat<=MAX_SEATS;seat++){
            if(seat_map[seat]=='0') return seat;
        }
        return -1;
    }
    if(want_window==1){
        for(seat=1;seat<=MAX_SEATS;seat++){
            if(seat_map[seat]=='0' && isWindowSeat(seat)) return seat;
        }
    }
    if(gender=='M'){
        for(seat=1;seat<=MAX_SEATS;seat++){
            if(seat_map[seat]=='0' && !isSandwiched(seat)) return seat;
        }
    }
    for(seat=1;seat<=MAX_SEATS;seat++){
        if(seat_map[seat]=='0') return seat;
    }
    return -1;
}

int countBookedSeats(int train_id){
    int booked=0,i;
    loadSeatMap(train_id);
    for(i=1;i<=MAX_SEATS;i++){
        if(seat_map[i]!='0') booked++;
    }
    return booked;
}

void viewTrains(){
    int i,booked,available;
    line();
    printf(CYAN "AVAILABLE TRAINS\n" RESET);
    smallLine();
    printf("ID   Name                 From       To         Fare  Dep    Plt  Avl\n");
    for(i=0;i<trainCount;i++){
        booked=countBookedSeats(trains[i].id);
        available=trains[i].total_seats-booked;
        printf("%-4d %-20s %-10s %-10s %-5.0f %-6s %-4d %-4d\n",
            trains[i].id,trains[i].name,trains[i].from,trains[i].to,
            trains[i].fare,trains[i].departure,trains[i].platform,available);
    }
    smallLine();
}

struct Train* findTrain(int id){
    int i;
    for(i=0;i<trainCount;i++){
        if(trains[i].id==id) return &trains[i];
    }
    return NULL;
}

void printTicket(struct Ticket t,struct Train *tr){
    char coach[5];
    float finalFare;
    int discount;
    getCoachName(t.seat_number,coach);
    finalFare=calculateFare(tr->fare,t.age);
    discount=getDiscountPercent(t.age);
    line();
    printf(CYAN "TRAIN TICKET\n" RESET);
    smallLine();
    printf("Booking ID : %d\n",t.booking_id);
    printf("PNR        : %d\n",t.pnr);
    printf("Passenger  : %s\n",t.passenger_name);
    printf("Age        : %d\n",t.age);
    printf("Gender     : ");
    if(t.gender=='M') printf(BLUE "%c\n" RESET,t.gender);
    else if(t.gender=='F') printf(PINK "%c\n" RESET,t.gender);
    else if(t.gender=='C') printf(YELLOW "%c\n" RESET,t.gender);
    else printf("%c\n",t.gender);
    printf("Train      : %s\n",tr->name);
    printf("Route      : %s to %s\n",tr->from,tr->to);
    printf("Coach      : %s\n",coach);
    printf("Seat       : %s-%d\n",coach,t.seat_number);
    printf("Platform   : %d\n",tr->platform);
    printf("Base Fare  : %.0f\n",tr->fare);
    printf("Discount   : %d%%\n",discount);
    printf("Final Fare : %.0f\n",finalFare);
    if(strcmp(t.status,"CONFIRMED")==0)
        printf("Status     : " GREEN "%s\n" RESET,t.status);
    else
        printf("Status     : " RED "%s\n" RESET,t.status);
    smallLine();
}

void bookingProcess(int train_id){
    int passengers,i,want_window;
    struct Ticket t;
    struct Train *tr=findTrain(train_id);
    if(tr==NULL){
        printf(RED "[!] Train not found.\n" RESET);
        return;
    }
    printf("Enter number of passengers: ");
    while(scanf("%d",&passengers) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Enter number of passengers: ");
    }
    getchar();
    if(passengers<=0){
        printf(RED "[!] Invalid number of passengers.\n" RESET);
        return;
    }
    for(i=1;i<=passengers;i++){
        line();
        printf(CYAN "Passenger %d Details\n" RESET,i);
        t.train_id=train_id;
        t.booking_id=generateBookingID();
        t.pnr=generatePNR();
        strcpy(t.status,"CONFIRMED");
        do{
            printf("Passenger Name: ");
            fgets(t.passenger_name,sizeof(t.passenger_name),stdin);
            t.passenger_name[strcspn(t.passenger_name,"\n")]=0;
            if(strlen(t.passenger_name)==0){
                printf(RED "[!] Name cannot be empty.\n" RESET);
                continue;
            }
            break;
        }while(1);
        if(isDuplicateBooking(t.passenger_name,train_id)){
            printf(RED "[!] Duplicate booking found for this passenger.\n" RESET);
            continue;
        }
        do{
            printf("Age: ");
            while(scanf("%d",&t.age) != 1){
                while(getchar() != '\n');
                printf(RED "[!] Invalid input.\n" RESET);
                printf("Age: ");
            }
            getchar();
            if(t.age<=0 || t.age>120){
                printf(RED "[!] Age must be between 1 and 120.\n" RESET);
                continue;
            }
            break;
        }while(1);
        if(t.age<12) t.gender='C';
        else{
            do{
                printf("Gender (M/F): ");
                scanf("%c",&t.gender);
                getchar();
                if(t.gender != 'M' && t.gender != 'F'){
                    printf(RED "[!] Please enter M or F.\n" RESET);
                }
            }while(t.gender != 'M' && t.gender != 'F');
        }
        printf("Emergency Contact: ");
        do{
            fgets(t.emergency_contact,sizeof(t.emergency_contact),stdin);
            t.emergency_contact[strcspn(t.emergency_contact,"\n")]=0;
            if(strlen(t.emergency_contact)==0){
                printf(RED "[!] Contact cannot be empty.\n" RESET);
                printf("Emergency Contact: ");
                continue;
            }
            break;
        }while(1);
        printf("Window seat required (1/0): ");
        while(scanf("%d",&want_window) != 1 || (want_window != 0 && want_window != 1)){
            while(getchar() != '\n');
            printf(RED "[!] Invalid input. Enter 0 or 1.\n" RESET);
            printf("Window seat required (1/0): ");
        }
        getchar();
        loadSeatMap(train_id);
        loader("Checking seat availability");
        t.seat_number=allocateSeat(t.gender,t.age,want_window);
        loader("Allocating seat");
        if(t.seat_number==-1){
            printf(RED "[!] No seats available.\n" RESET);
            continue;
        }
        loader("Saving booking");
        FILE *fp=fopen(TICKET_FILE,"a");
        if(fp==NULL){
            printf(RED "[!] File error.\n" RESET);
            return;
        }
        fprintf(fp,"%d %d %d %s %d %c %d %s %s\n",
            t.booking_id,t.pnr,t.train_id,t.passenger_name,
            t.age,t.gender,t.seat_number,t.status,t.emergency_contact);
        fflush(fp);
        fclose(fp);
        loader("Generating ticket");
        printf(GREEN "[+] Ticket booked successfully.\n" RESET);
        printTicket(t,tr);
    }
}

void bookTicket(){
    int train_id;
    viewTrains();
    printf("\nEnter Train ID: ");
    while(scanf("%d",&train_id) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Enter Train ID: ");
    }
    getchar();
    bookingProcess(train_id);
}

void viewBookings(){
    FILE *fp=fopen(TICKET_FILE,"r");
    struct Ticket t;
    int count=0;
    if(fp==NULL){
        printf(RED "[!] No bookings found.\n" RESET);
        return;
    }
    line();
    printf(CYAN "BOOKING RECORDS\n" RESET);
    smallLine();
    printf("%-8s %-8s %-20s %-8s %-6s %-12s\n","BookID","PNR","Passenger","Train","Seat","Gender");
    smallLine();
    while(fscanf(fp,"%d %d %d %49s %d %c %d %14s %14s",
        &t.booking_id,&t.pnr,&t.train_id,t.passenger_name,
        &t.age,&t.gender,&t.seat_number,t.status,t.emergency_contact)==9){
        printf("%-8d %-8d %-20s %-8d %-6d ",
            t.booking_id,t.pnr,t.passenger_name,t.train_id,t.seat_number);
        if(t.gender=='M') printf(BLUE "M" RESET);
        else if(t.gender=='F') printf(PINK "F" RESET);
        else if(t.gender=='C') printf(YELLOW "C" RESET);
        printf(" - ");
        if(strcmp(t.status,"CONFIRMED")==0) printf(GREEN "%s\n" RESET,t.status);
        else printf(RED "%s\n" RESET,t.status);
        count++;
    }
    fclose(fp);
    smallLine();
    printf("Total Bookings: %d\n",count);
    smallLine();
}

void showTicketByID(){
    int id,found=0;
    struct Ticket t;
    struct Train *tr;
    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL){
        printf(RED "[!] No bookings found.\n" RESET);
        return;
    }
    printf("Enter Booking ID: ");
    while(scanf("%d",&id) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Enter Booking ID: ");
    }
    getchar();
    loader("Searching ticket");
    while(fscanf(fp,"%d %d %d %49s %d %c %d %14s %14s",
        &t.booking_id,&t.pnr,&t.train_id,t.passenger_name,
        &t.age,&t.gender,&t.seat_number,t.status,t.emergency_contact)==9){
        if(t.booking_id==id){
            tr=findTrain(t.train_id);
            if(tr==NULL){
                printf(RED "[!] Train data not found.\n" RESET);
                fclose(fp);
                return;
            }
            printTicket(t,tr);
            found=1;
            break;
        }
    }
    fclose(fp);
    if(!found) printf(RED "[!] Booking ID not found.\n" RESET);
}

void cancelTicket(){
    int cancel_id,found=0,i,count=0;
    char confirm;
    struct Ticket all[500];
    printf("Enter Booking ID: ");
    while(scanf("%d",&cancel_id) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Enter Booking ID: ");
    }
    getchar();
    printf("Confirm cancellation? (Y/N): ");
    while(scanf("%c",&confirm) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Confirm cancellation? (Y/N): ");
    }
    getchar();
    if(confirm!='Y' && confirm!='y'){
        printf("Cancellation stopped.\n");
        return;
    }
    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL){
        printf(RED "[!] No bookings found.\n" RESET);
        return;
    }
    while(fscanf(fp,"%d %d %d %49s %d %c %d %14s %14s",
        &all[count].booking_id,&all[count].pnr,&all[count].train_id,
        all[count].passenger_name,&all[count].age,&all[count].gender,
        &all[count].seat_number,all[count].status,all[count].emergency_contact)==9){
        if(all[count].booking_id==cancel_id &&
           strcmp(all[count].status,"CONFIRMED")==0){
            strcpy(all[count].status,"CANCELLED");
            found=1;
        }
        count++;
        if(count >= 500) break;
    }
    fclose(fp);
    loader("Cancelling ticket");
    fp=fopen(TICKET_FILE,"w");
    for(i=0;i<count;i++){
        fprintf(fp,"%d %d %d %s %d %c %d %s %s\n",
            all[i].booking_id,all[i].pnr,all[i].train_id,
            all[i].passenger_name,all[i].age,all[i].gender,
            all[i].seat_number,all[i].status,all[i].emergency_contact);
    }
    fflush(fp);
    fclose(fp);
    if(found) printf(GREEN "[+] Ticket cancelled successfully.\n" RESET);
    else printf(RED "[!] Booking ID not found or already cancelled.\n" RESET);
}

void searchTrain(){
    int choice,i,found=0,trainChoice,nextChoice;
    char from[30],to[30],key[50];
    printf("\n1. Search by route\n");
    printf("2. Search by train keyword\n");
    printf("Enter choice: ");
    while(scanf("%d",&choice) != 1 || (choice != 1 && choice != 2)){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input. Enter 1 or 2.\n" RESET);
        printf("Enter choice: ");
    }
    getchar();
    loader("Searching trains");
    line();
    printf(CYAN "SEARCH RESULT\n" RESET);
    smallLine();
    if(choice==1){
        printf("From: ");
        fgets(from,sizeof(from),stdin);
        from[strcspn(from,"\n")]=0;
        printf("To: ");
        fgets(to,sizeof(to),stdin);
        to[strcspn(to,"\n")]=0;
        for(i=0;i<trainCount;i++){
            if(strcmp(trains[i].from,from)==0 &&
               strcmp(trains[i].to,to)==0){
                printf("Train ID : %d\n",trains[i].id);
                printf("Train    : %s\n",trains[i].name);
                printf("Fare     : %.0f\n",trains[i].fare);
                printf("Platform : %d\n",trains[i].platform);
                smallLine();
                found=1;
            }
        }
    }else if(choice==2){
        printf("Enter keyword: ");
        fgets(key,sizeof(key),stdin);
        key[strcspn(key,"\n")]=0;
        for(i=0;i<trainCount;i++){
            if(strstr(trains[i].name,key)!=NULL){
                printf("Train ID : %d\n",trains[i].id);
                printf("Train    : %s\n",trains[i].name);
                printf("Route    : %s to %s\n",trains[i].from,trains[i].to);
                printf("Fare     : %.0f\n",trains[i].fare);
                printf("Platform : %d\n",trains[i].platform);
                smallLine();
                found=1;
            }
        }
    }else{
        printf(RED "[!] Invalid choice.\n" RESET);
        return;
    }
    if(!found){
        printf(RED "[!] No train found.\n" RESET);
        return;
    }
    printf("\n1. Book Ticket\n");
    printf("2. Go Back\n");
    printf("Enter choice: ");
    while(scanf("%d",&nextChoice) != 1 || (nextChoice != 1 && nextChoice != 2)){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input. Enter 1 or 2.\n" RESET);
        printf("Enter choice: ");
    }
    getchar();
    if(nextChoice==2){
        printf("Returning to menu...\n");
        return;
    }
    printf("Enter Train ID to continue booking: ");
    while(scanf("%d",&trainChoice) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Enter Train ID to continue booking: ");
    }
    getchar();
    bookingProcess(trainChoice);
}

void showSeatMap(){
    int tid,i,available=0,booked=0;
    struct Train *tr;
    char coach[5];
    printf("Enter Train ID: ");
    while(scanf("%d",&tid) != 1){
        while(getchar() != '\n');
        printf(RED "[!] Invalid input.\n" RESET);
        printf("Enter Train ID: ");
    }
    getchar();
    tr = findTrain(tid);
    if(tr==NULL){
        printf(RED "[!] Train not found.\n" RESET);
        return;
    }
    loader("Loading seat map");
    loadSeatMap(tid);
    line();
    printf(CYAN "%s\n" RESET,tr->name);
    printf(CYAN "Train ID: %d | Route: %s to %s\n" RESET,tid,tr->from,tr->to);
    smallLine();
    
    for(i=1;i<=MAX_SEATS;i++){
        if(seat_map[i]=='0'){
            printf("[%02d: ] ",i);
            available++;
        }else if(seat_map[i]=='M'){
            printf(BLUE "[%02d:%c]" RESET " ",i,seat_map[i]);
            booked++;
        }else if(seat_map[i]=='F'){
            printf(PINK "[%02d:%c]" RESET " ",i,seat_map[i]);
            booked++;
        }else if(seat_map[i]=='C'){
            printf(YELLOW "[%02d:%c]" RESET " ",i,seat_map[i]);
            booked++;
        }
        if(i%10==0) printf("\n");
    }
    printf("\n");
    printf("Legend: ");
    printf(BLUE "[##:M]" RESET);
    printf(" = Male | ");
    printf(PINK "[##:F]" RESET);
    printf(" = Female | ");
    printf(YELLOW "[##:C]" RESET);
    printf(" = Child | [##: ] = Available\n");
    smallLine();
    printf("Booked Seats    : %d\n",booked);
    printf("Available Seats : %d\n",available);
    smallLine();
}