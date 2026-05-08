#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SEATS 50
#define TICKET_FILE "tickets.txt"

#define CYAN "\033[1;36m"
#define RED "\033[1;31m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

struct Train {
    int id;
    char name[50];
    char from[30];
    char to[30];
    int total_seats;
    float fare;
    char departure[10];
    int platform;
};

struct Ticket {
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

char seat_map[MAX_SEATS + 1];

void line();
void viewTrains();
void bookTicket();
void viewBookings();
void cancelTicket();
void searchTrain();
void showSeatMap();
void loadSeatMap(int train_id);
void getCoachName(int seat, char coach[]);

int generateBookingID();
int generatePNR();
int isDuplicateBooking(char name[], int train_id);
int isWindowSeat(int seat);
int isSandwiched(int seat);
int allocateSeat(char gender, int age, int want_window);
int getDiscountPercent(int age);
int countBookedSeats(int train_id);

float calculateFare(float baseFare, int age);

struct Train* findTrain(int id);

struct Train trains[5] = {
    {101,"Rajdhani Express","Delhi","Mumbai",50,850.0,"06:00",3},
    {102,"Shatabdi Express","Chennai","Bangalore",50,450.0,"07:30",1},
    {103,"Duronto Express","Kolkata","Delhi",50,700.0,"22:00",5},
    {104,"Garib Rath","Mumbai","Pune",50,120.0,"09:15",2},
    {105,"Jan Shatabdi","Delhi","Agra",50,200.0,"08:00",4}
};

int trainCount = 5;

int main() {
    int choice;

    do {
        printf(CYAN);
        printf("\n=============================================\n");
        printf("      Train Seat Allocation System\n");
        printf("=============================================\n");
        printf(RESET);

        printf("1. View All Trains\n");
        printf("2. Search Train\n");
        printf("3. Book Ticket\n");
        printf("4. View Bookings\n");
        printf("5. Cancel Ticket\n");
        printf("6. View Seat Map\n");
        printf("7. Exit\n");
        printf("\nEnter choice: ");
        scanf("%d",&choice);

        switch(choice) {
            case 1: viewTrains(); sleep(1); break;
            case 2: searchTrain(); sleep(1); break;
            case 3: bookTicket(); sleep(1); break;
            case 4: viewBookings(); sleep(1); break;
            case 5: cancelTicket(); sleep(1); break;
            case 6: showSeatMap(); sleep(1); break;
            case 7: printf("\nThank you.\n"); break;
            default: printf(RED "Invalid choice.\n" RESET);
        }
    } while(choice != 7);

    return 0;
}

void line() {
    printf("\n------------------------------------------------------------\n");
}

void loadSeatMap(int train_id) {
    int i;
    struct Ticket t;

    for(i=1;i<=MAX_SEATS;i++)
        seat_map[i]='0';

    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL)
        return;

    while(fscanf(fp,"%d %d %d %s %d %c %d %s %s",
        &t.booking_id,
        &t.pnr,
        &t.train_id,
        t.passenger_name,
        &t.age,
        &t.gender,
        &t.seat_number,
        t.status,
        t.emergency_contact)==9) {

        if(t.train_id==train_id &&
           strcmp(t.status,"CONFIRMED")==0 &&
           t.seat_number>=1 &&
           t.seat_number<=MAX_SEATS) {
            seat_map[t.seat_number]=t.gender;
        }
    }

    fclose(fp);
}

int generateBookingID() {
    int count=1001;
    char line[200];

    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL)
        return count;

    while(fgets(line,sizeof(line),fp))
        count++;

    fclose(fp);
    return count;
}

int generatePNR() {
    int count=100000;
    char line[200];

    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL)
        return count;

    while(fgets(line,sizeof(line),fp))
        count++;

    fclose(fp);
    return count;
}

int isDuplicateBooking(char name[], int train_id) {
    FILE *fp=fopen(TICKET_FILE,"r");
    struct Ticket t;

    if(fp==NULL)
        return 0;

    while(fscanf(fp,"%d %d %d %s %d %c %d %s %s",
        &t.booking_id,
        &t.pnr,
        &t.train_id,
        t.passenger_name,
        &t.age,
        &t.gender,
        &t.seat_number,
        t.status,
        t.emergency_contact)==9) {

        if(strcmp(t.passenger_name,name)==0 &&
           t.train_id==train_id &&
           strcmp(t.status,"CONFIRMED")==0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int getDiscountPercent(int age) {
    if(age < 12)
        return 50;
    else if(age > 60)
        return 20;
    else
        return 0;
}

float calculateFare(float baseFare, int age) {
    int discount=getDiscountPercent(age);
    return baseFare - (baseFare * discount / 100);
}

void getCoachName(int seat, char coach[]) {
    if(seat <= 25)
        strcpy(coach,"S1");
    else
        strcpy(coach,"S2");
}

int isWindowSeat(int seat) {
    return (seat%3==1);
}

int isSandwiched(int seat) {
    char left,right;

    if(seat<=1 || seat>=MAX_SEATS)
        return 0;

    left=seat_map[seat-1];
    right=seat_map[seat+1];

    if(left!='0' && right!='0')
        return 1;

    return 0;
}

int allocateSeat(char gender,int age,int want_window) {
    int seat;

    if(age<12) {
        for(seat=1;seat<=10;seat++) {
            if(seat_map[seat]=='0')
                return seat;
        }
    }

    if(age>60) {
        for(seat=1;seat<=MAX_SEATS;seat++) {
            if(seat_map[seat]=='0')
                return seat;
        }
    }

    if(want_window==1) {
        for(seat=1;seat<=MAX_SEATS;seat++) {
            if(seat_map[seat]=='0' && isWindowSeat(seat))
                return seat;
        }
    }

    if(gender=='M') {
        for(seat=1;seat<=MAX_SEATS;seat++) {
            if(seat_map[seat]=='0' && !isSandwiched(seat))
                return seat;
        }
    }

    for(seat=1;seat<=MAX_SEATS;seat++) {
        if(seat_map[seat]=='0')
            return seat;
    }

    return -1;
}

int countBookedSeats(int train_id) {
    int booked=0;
    int i;

    loadSeatMap(train_id);

    for(i=1;i<=MAX_SEATS;i++) {
        if(seat_map[i]!='0')
            booked++;
    }

    return booked;
}

void viewTrains() {
    int i,booked,available;

    line();
    printf(CYAN "ID   Name                 From       To         Fare   Dep    Plat  Avail\n" RESET);

    for(i=0;i<trainCount;i++) {
        booked=countBookedSeats(trains[i].id);
        available=trains[i].total_seats-booked;

        printf("%-4d %-20s %-10s %-10s %-6.0f %-6s %-5d %-5d\n",
            trains[i].id,
            trains[i].name,
            trains[i].from,
            trains[i].to,
            trains[i].fare,
            trains[i].departure,
            trains[i].platform,
            available);
    }

    line();
}

struct Train* findTrain(int id) {
    int i;

    for(i=0;i<trainCount;i++) {
        if(trains[i].id==id)
            return &trains[i];
    }

    return NULL;
}

void printTicket(struct Ticket t, struct Train *tr) {
    char coach[5];
    float finalFare;
    int discount;

    getCoachName(t.seat_number,coach);

    finalFare=calculateFare(tr->fare,t.age);
    discount=getDiscountPercent(t.age);

    printf(CYAN "\n====================================\n" RESET);
    printf(WHITE "           TRAIN TICKET\n" RESET);
    printf(CYAN "====================================\n" RESET);
    printf("Booking ID : %d\n",t.booking_id);
    printf("PNR        : %d\n",t.pnr);
    printf("Passenger  : %s\n",t.passenger_name);
    printf("Age        : %d\n",t.age);
    printf("Gender     : %c\n",t.gender);
    printf("Train      : %s\n",tr->name);
    printf("Route      : %s to %s\n",tr->from,tr->to);
    printf("Coach      : %s\n",coach);
    printf("Seat       : %s-%d\n",coach,t.seat_number);
    printf("Platform   : %d\n",tr->platform);
    printf("Base Fare  : %.0f\n",tr->fare);
    printf("Discount   : %d%%\n",discount);
    printf("Final Fare : %.0f\n",finalFare);
    printf("Status     : %s\n",t.status);
    printf(CYAN "====================================\n" RESET);
}

void bookTicket() {
    int train_id;
    int passengers;
    int i;
    int want_window;
    struct Ticket t;

    viewTrains();

    printf("\nEnter Train ID: ");
    scanf("%d",&train_id);

    struct Train *tr=findTrain(train_id);

    if(tr==NULL) {
        printf(RED "Train not found.\n" RESET);
        return;
    }

    printf("Enter number of passengers: ");
    scanf("%d",&passengers);

    if(passengers<=0) {
        printf(RED "Invalid number of passengers.\n" RESET);
        return;
    }

    for(i=1;i<=passengers;i++) {
        printf("\nPassenger %d Details\n",i);

        t.train_id=train_id;
        t.booking_id=generateBookingID();
        t.pnr=generatePNR();
        strcpy(t.status,"CONFIRMED");

        printf("Passenger Name: ");
        scanf("%s",t.passenger_name);

        if(isDuplicateBooking(t.passenger_name,train_id)) {
            printf(RED "Duplicate booking found for this passenger.\n" RESET);
            continue;
        }

        printf("Age: ");
        scanf("%d",&t.age);

        if(t.age<12) {
            t.gender='C';
        } else {
            printf("Gender (M/F): ");
            scanf(" %c",&t.gender);
        }

        printf("Emergency Contact: ");
        scanf("%s",t.emergency_contact);

        printf("Window seat required (1/0): ");
        scanf("%d",&want_window);

        loadSeatMap(train_id);

        t.seat_number=allocateSeat(t.gender,t.age,want_window);

        if(t.seat_number==-1) {
            printf(RED "No seats available.\n" RESET);
            continue;
        }

        FILE *fp=fopen(TICKET_FILE,"a");

        if(fp==NULL) {
            printf(RED "File error.\n" RESET);
            return;
        }

        fprintf(fp,"%d %d %d %s %d %c %d %s %s\n",
            t.booking_id,
            t.pnr,
            t.train_id,
            t.passenger_name,
            t.age,
            t.gender,
            t.seat_number,
            t.status,
            t.emergency_contact);

        fclose(fp);

        printf("\nTicket Confirmed!\n");
        printTicket(t,tr);
    }

    printf("\nProcessing completed.\n");
}

void viewBookings() {
    FILE *fp=fopen(TICKET_FILE,"r");
    struct Ticket t;

    if(fp==NULL) {
        printf("No bookings found.\n");
        return;
    }

    line();
    printf(CYAN "Bookings\n" RESET);

    while(fscanf(fp,"%d %d %d %s %d %c %d %s %s",
        &t.booking_id,
        &t.pnr,
        &t.train_id,
        t.passenger_name,
        &t.age,
        &t.gender,
        &t.seat_number,
        t.status,
        t.emergency_contact)==9) {

        printf("%d | PNR:%d | %s | Train:%d | Seat:%d | %s\n",
            t.booking_id,
            t.pnr,
            t.passenger_name,
            t.train_id,
            t.seat_number,
            t.status);
    }

    fclose(fp);
    line();
}

void cancelTicket() {
    int cancel_id,found=0,i,count=0;
    char confirm;
    struct Ticket all[200];

    printf("Enter Booking ID: ");
    scanf("%d",&cancel_id);

    printf("Confirm cancellation? (Y/N): ");
    scanf(" %c",&confirm);

    if(confirm!='Y' && confirm!='y') {
        printf("Cancellation stopped.\n");
        return;
    }

    FILE *fp=fopen(TICKET_FILE,"r");

    if(fp==NULL) {
        printf("No bookings found.\n");
        return;
    }

    while(fscanf(fp,"%d %d %d %s %d %c %d %s %s",
        &all[count].booking_id,
        &all[count].pnr,
        &all[count].train_id,
        all[count].passenger_name,
        &all[count].age,
        &all[count].gender,
        &all[count].seat_number,
        all[count].status,
        all[count].emergency_contact)==9) {

        if(all[count].booking_id==cancel_id &&
           strcmp(all[count].status,"CONFIRMED")==0) {
            strcpy(all[count].status,"CANCELLED");
            found=1;
        }

        count++;
    }

    fclose(fp);

    fp=fopen(TICKET_FILE,"w");

    for(i=0;i<count;i++) {
        fprintf(fp,"%d %d %d %s %d %c %d %s %s\n",
            all[i].booking_id,
            all[i].pnr,
            all[i].train_id,
            all[i].passenger_name,
            all[i].age,
            all[i].gender,
            all[i].seat_number,
            all[i].status,
            all[i].emergency_contact);
    }

    fclose(fp);

    if(found)
        printf(RED "Ticket Cancelled.\n" RESET);
    else
        printf(RED "Booking ID not found or already cancelled.\n" RESET);
}

void searchTrain() {

    int choice;
    char from[30],to[30],key[50];
    int i,found=0;
    int trainChoice;
    int nextChoice;

    printf("\n1. Search by route\n");
    printf("2. Search by train keyword\n");

    printf("Enter choice: ");
    scanf("%d",&choice);

    if(choice==1) {

        printf("From: ");
        scanf("%s",from);

        printf("To: ");
        scanf("%s",to);

        printf("\nMatching Trains\n");
        line();

        for(i=0;i<trainCount;i++) {

            if(strcmp(trains[i].from,from)==0 &&
               strcmp(trains[i].to,to)==0) {

                printf("Train ID : %d\n",trains[i].id);
                printf("Train    : %s\n",trains[i].name);
                printf("Fare     : %.0f\n",trains[i].fare);
                printf("Platform : %d\n",trains[i].platform);

                line();
                found=1;
            }
        }

    }

    else if(choice==2) {

        printf("Enter keyword: ");
        scanf("%s",key);

        printf("\nMatching Trains\n");
        line();

        for(i=0;i<trainCount;i++) {

            if(strstr(trains[i].name,key)!=NULL) {

                printf("Train ID : %d\n",trains[i].id);
                printf("Train    : %s\n",trains[i].name);

                printf("Route    : %s to %s\n",
                    trains[i].from,
                    trains[i].to);

                printf("Fare     : %.0f\n",
                    trains[i].fare);

                printf("Platform : %d\n",
                    trains[i].platform);

                line();

                found=1;
            }
        }

    }

    else {
        printf(RED "Invalid choice.\n" RESET);
        return;
    }

    if(!found) {
        printf(RED "No train found.\n" RESET);
        return;
    }

    printf("\n1. Book Ticket\n");
    printf("2. Go Back\n");

    printf("Enter choice: ");
    scanf("%d",&nextChoice);

    if(nextChoice==2) {
        printf("Returning to menu...\n");
        return;
    }

    if(nextChoice!=1) {
        printf(RED "Invalid choice.\n" RESET);
        return;
    }

    printf("\nEnter Train ID to continue booking: ");
    scanf("%d",&trainChoice);

    if(findTrain(trainChoice)==NULL) {
        printf(RED "Invalid Train ID.\n" RESET);
        return;
    }

    printf("\nRedirecting to booking...\n");
    sleep(1);

    int passengers;
    int p;
    int want_window;

    struct Ticket t;

    struct Train *tr=findTrain(trainChoice);

    printf("Enter number of passengers: ");
    scanf("%d",&passengers);

    if(passengers<=0) {
        printf(RED "Invalid number of passengers.\n" RESET);
        return;
    }

    for(p=1;p<=passengers;p++) {
        printf("\nPassenger %d Details\n",p);
        t.train_id=trainChoice;

        t.booking_id=generateBookingID();

        t.pnr=generatePNR();

        strcpy(t.status,"CONFIRMED");

        printf("Passenger Name: ");
        scanf("%s",t.passenger_name);

        if(isDuplicateBooking(
            t.passenger_name,
            trainChoice)) {

            printf(RED
                "Duplicate booking found.\n"
                RESET);

            continue;
        }

        printf("Age: ");
        scanf("%d",&t.age);

        if(t.age<12) {

            t.gender='C';

        } else {
            printf("Gender (M/F): ");
            scanf(" %c",&t.gender);
        }
        printf("Emergency Contact: ");
        scanf("%s",t.emergency_contact);
        printf("Window seat required (1/0): ");
        scanf("%d",&want_window);

        loadSeatMap(trainChoice);

        t.seat_number=allocateSeat(
            t.gender,
            t.age,
            want_window
        );

        if(t.seat_number==-1) {

            printf(RED
                "No seats available.\n"
                RESET);

            continue;
        }

        FILE *fp=fopen(TICKET_FILE,"a");

        if(fp==NULL) {

            printf(RED
                "File error.\n"
                RESET);

            return;
        }

        fprintf(fp,
            "%d %d %d %s %d %c %d %s %s\n",

            t.booking_id,
            t.pnr,
            t.train_id,
            t.passenger_name,
            t.age,
            t.gender,
            t.seat_number,
            t.status,
            t.emergency_contact
        );

        fclose(fp);

        printf("\nTicket Confirmed!\n");

        printTicket(t,tr);
    }

    printf("\nProcessing completed.\n");
}


void showSeatMap() {
    int tid,i;
    int available=0,booked=0;

    printf("Enter Train ID: ");
    scanf("%d",&tid);

    if(findTrain(tid)==NULL) {
        printf(RED "Train not found.\n" RESET);
        return;
    }

    loadSeatMap(tid);

    printf(CYAN "\nSeat Map\n\n" RESET);

    for(i=1;i<=MAX_SEATS;i++) {
        if(seat_map[i]=='0') {
            printf("[%02d: ] ",i);
            available++;
        } else {
            printf("[%02d:%c] ",i,seat_map[i]);
            booked++;
        }

        if(i%10==0)
            printf("\n");
    }

    printf("\nLegend: M = Male, F = Female, C = Child, Empty = Available\n");
    printf("Booked Seats    : %d\n",booked);
    printf("Available Seats : %d\n",available);
}