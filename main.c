#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

#define MAX_SEATS 50
#define MAX_PASSENGERS 10
#define TICKET_FILE "tickets.txt"

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
int generateBookingID();
int isWindowSeat(int seat);
int isSandwiched(int seat);
int allocateSeat(char gender, int age, int want_window);
struct Train* findTrain(int id);
void getCoachName(int seat, char coach[]);
void displayTicket(struct Ticket t, struct Train tr);
float calculateFare(float baseFare, int age);
int getDiscountPercent(int age);

struct Train trains[5] = {
    {101,"Rajdhani Express","Delhi","Mumbai",50,850.0,"06:00",3},
    {102,"Shatabdi Express","Chennai","Bangalore",50,450.0,"07:30",5},
    {103,"Duronto Express","Kolkata","Delhi",50,700.0,"22:00",2},
    {104,"Garib Rath","Mumbai","Pune",50,120.0,"09:15",7},
    {105,"Jan Shatabdi","Delhi","Agra",50,200.0,"08:00",1}
};

int trainCount = 5;

int main() {
    int choice;

    do {
        printf("\n%s==============================%s\n", CYAN, RESET);
        printf("%s   Train Seat Allocation System   %s\n", CYAN, RESET);
        printf("%s==============================%s\n", CYAN, RESET);
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
            case 1: viewTrains(); break;
            case 2: searchTrain(); break;
            case 3: bookTicket(); break;
            case 4: viewBookings(); break;
            case 5: cancelTicket(); break;
            case 6: showSeatMap(); break;
            case 7: printf("\nThank you.\n"); break;
            default: printf(RED "Invalid choice.\n" RESET);
        }
    } while(choice != 7);

    return 0;
}

void line() {
    printf("\n--------------------------------------------------\n");
}

void getCoachName(int seat, char coach[]) {
    if(seat >= 1 && seat <= 25) strcpy(coach, "S1");
    else if(seat >= 26 && seat <= 50) strcpy(coach, "S2");
    else strcpy(coach, "NA");
}

void displayTicket(struct Ticket t, struct Train tr) {
    char coach[5];
    getCoachName(t.seat_number, coach);
    printf("\n------------------------------\n");
    printf("        TRAIN TICKET\n");
    printf("------------------------------\n");
    printf("Booking ID : %d\n", t.booking_id);
    printf("Passenger  : %s\n", t.passenger_name);
    printf("Age        : %d\n", t.age);
    printf("Gender     : %c\n", t.gender);
    printf("Train      : %s\n", tr.name);
    printf("Route      : %s -> %s\n", tr.from, tr.to);
    printf("Coach      : %s\n", coach);
    printf("Seat       : %s-%d\n", coach, t.seat_number);
    printf("Platform   : %d\n", tr.platform);
    printf("Fare       : %.0f\n", tr.fare);
    printf("Status     : %s%s%s\n", strcmp(t.status, "CONFIRMED") == 0 ? GREEN : RED, t.status, RESET);
    printf("------------------------------\n");
}

void loadSeatMap(int train_id) {
    int i;
    for(i=1;i<=MAX_SEATS;i++) seat_map[i]='0';

    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) return;

    struct Ticket t;
    while(fscanf(fp,"%d %d %s %d %c %d %s %s",
        &t.booking_id,&t.train_id,t.passenger_name,
        &t.age,&t.gender,&t.seat_number,
        t.status,t.emergency_contact)==8) {

        if(t.train_id==train_id &&
           strcmp(t.status,"CONFIRMED")==0 &&
           t.seat_number>=1 && t.seat_number<=MAX_SEATS) {
            seat_map[t.seat_number]=t.gender;
        }
    }
    fclose(fp);
}

int generateBookingID() {
    int count=1001;
    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) return count;
    char line[200];
    while(fgets(line,sizeof(line),fp)) count++;
    fclose(fp);
    return count;
}

int isWindowSeat(int seat) {
    return (seat%3==1);
}

int isSandwiched(int seat) {
    if(seat<=1 || seat>=MAX_SEATS) return 0;

    char left = seat_map[seat-1];
    char right = seat_map[seat+1];

    if(left!='0' && right!='0')
        return 1;

    return 0;
}

int allocateSeat(char gender,int age,int want_window) {
    int seat;

    if(want_window==1) {
        for(seat=1;seat<=MAX_SEATS;seat++) {
            if(seat_map[seat]=='0' && isWindowSeat(seat))
                return seat;
        }
    }

    if(want_window==0) {
        for(seat=1;seat<=MAX_SEATS;seat++) {
            if(seat_map[seat]=='0' && !isWindowSeat(seat))
                return seat;
        }
    }

    if(age<12) {
        for(seat=1;seat<=10;seat++)
            if(seat_map[seat]=='0') return seat;
    }

    if(gender=='F' || gender=='C') {
        for(seat=1;seat<=MAX_SEATS;seat++)
            if(seat_map[seat]=='0') return seat;
    }

    if(gender=='M') {
        for(seat=1;seat<=MAX_SEATS;seat++)
            if(seat_map[seat]=='0' && !isSandwiched(seat)) return seat;

        for(seat=1;seat<=MAX_SEATS;seat++)
            if(seat_map[seat]=='0') return seat;
    }

    return -1;
}

void viewTrains() {
    int i;
    line();
    printf(CYAN);
    printf("ID   Name                 From       To         Fare   Dep   Plat\n");
    printf(RESET);
    for(i=0;i<trainCount;i++) {
        printf("%-4d %-20s %-10s %-10s %-6.0f %-5s %-4d\n",
            trains[i].id,trains[i].name,
            trains[i].from,trains[i].to,
            trains[i].fare,trains[i].departure,trains[i].platform);
    }
    line();
    printf("Processing...\n");
    sleep(1);
}

struct Train* findTrain(int id) {
    int i;
    for(i=0;i<trainCount;i++)
        if(trains[i].id==id) return &trains[i];
    return NULL;
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
    int discount = getDiscountPercent(age);
    return baseFare - (baseFare * discount / 100);
}


void bookTicket() {
    int train_id;
    struct Ticket t;
    int want_window;
    float finalFare;
    int discount;

    viewTrains();

    printf("\nEnter Train ID: ");
    scanf("%d",&train_id);

    struct Train *tr = findTrain(train_id);

    if(tr == NULL) {
        printf("Train not found.\n");
        return;
    }

    t.train_id = train_id;
    t.booking_id = generateBookingID();
    strcpy(t.status,"CONFIRMED");

    printf("Passenger Name: ");
    scanf("%s",t.passenger_name);

    printf("Age: ");
    scanf("%d",&t.age);

    if(t.age < 12) {
        t.gender = 'C';
        printf("Emergency Contact: ");
        scanf("%s",t.emergency_contact);
    } else {
        printf("Gender (M/F): ");
        scanf(" %c",&t.gender);
        strcpy(t.emergency_contact,"N/A");
    }

    printf("Window seat required (1/0): ");
    scanf("%d",&want_window);

    loadSeatMap(train_id);

    t.seat_number = allocateSeat(t.gender,t.age,want_window);

    if(t.seat_number == -1) {
        printf("No seats available.\n");
        return;
    }

    FILE *fp = fopen(TICKET_FILE,"a");

    if(fp == NULL) {
        printf("File error.\n");
        return;
    }

    fprintf(fp,"%d %d %s %d %c %d %s %s\n",
        t.booking_id,
        t.train_id,
        t.passenger_name,
        t.age,
        t.gender,
        t.seat_number,
        t.status,
        t.emergency_contact);

    fclose(fp);

    finalFare = calculateFare(tr->fare, t.age);
    discount = getDiscountPercent(t.age);

    printf("\nTicket Confirmed!\n");

    printf("\n====================================\n");
    printf("           TRAIN TICKET\n");
    printf("====================================\n");
    printf("Booking ID : %d\n",t.booking_id);
    printf("Passenger  : %s\n",t.passenger_name);
    printf("Age        : %d\n",t.age);
    printf("Gender     : %c\n",t.gender);
    printf("Train ID   : %d\n",t.train_id);
    printf("Seat No    : %d\n",t.seat_number);
    printf("Base Fare  : %.0f\n",tr->fare);
    printf("Discount   : %d%%\n",discount);
    printf("Final Fare : %.0f\n",finalFare);
    printf("Status     : %s\n",t.status);
    printf("====================================\n");

    printf("Processing....");
}

void viewBookings() {
    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) {
        printf(RED "No bookings found.\n" RESET);
        return;
    }

    struct Ticket t;
    printf(CYAN);
    line();
    printf("BookID | Passenger | Train | Seat | Coach | Status\n");
    line();
    printf(RESET);
    while(fscanf(fp,"%d %d %s %d %c %d %s %s",
        &t.booking_id,&t.train_id,t.passenger_name,
        &t.age,&t.gender,&t.seat_number,
        t.status,t.emergency_contact)==8) {

        char coach[5];
        getCoachName(t.seat_number, coach);
        printf("%-6d | %-9s | %-5d | %-4d | %-5s | %s%s%s\n",
            t.booking_id,t.passenger_name,
            t.train_id,t.seat_number,coach,
            strcmp(t.status,"CONFIRMED")==0 ? GREEN : RED,
            t.status, RESET);
    }
    fclose(fp);
    line();
    printf("Processing...\n");
    sleep(1);
}

void cancelTicket() {
    int cancel_id,found=0,i,count=0;
    struct Ticket all[200];

    printf("Enter Booking ID: ");
    scanf("%d",&cancel_id);

    FILE *fp=fopen(TICKET_FILE,"r");
    if(fp==NULL) return;

    while(fscanf(fp,"%d %d %s %d %c %d %s %s",
        &all[count].booking_id,&all[count].train_id,
        all[count].passenger_name,&all[count].age,
        &all[count].gender,&all[count].seat_number,
        all[count].status,all[count].emergency_contact)==8) {

        if(all[count].booking_id==cancel_id) {
            strcpy(all[count].status,"CANCELLED");
            found=1;
        }
        count++;
    }
    fclose(fp);

    fp=fopen(TICKET_FILE,"w");
    for(i=0;i<count;i++) {
        fprintf(fp,"%d %d %s %d %c %d %s %s\n",
            all[i].booking_id,all[i].train_id,
            all[i].passenger_name,all[i].age,
            all[i].gender,all[i].seat_number,
            all[i].status,all[i].emergency_contact);
    }
    fclose(fp);

    if(found) printf("Ticket Cancelled.\n");
    else printf("Booking ID not found.\n");
    printf("Processing.....");
}

void searchTrain() {
    char from[30],to[30],keyword[50];
    int i,found=0,choice;

    printf(CYAN "\n--- Search Options ---\n" RESET);
    printf("1. Search by Route (From-To)\n");
    printf("2. Search by Train Name (Keyword)\n");
    printf("Enter choice: ");
    scanf("%d",&choice);

    if(choice==1) {
        printf("From: ");
        scanf("%s",from);
        printf("To: ");
        scanf("%s",to);
        printf("Trains found:\n");
        for(i=0;i<trainCount;i++) {
            if(strcmp(trains[i].from,from)==0 &&
               strcmp(trains[i].to,to)==0) {
                printf("ID: %d | %s | Fare: %.0f | Platform: %d\n",
                    trains[i].id,trains[i].name,
                    trains[i].fare,trains[i].platform);
                found=1;
            }
        }
        if(!found) printf(RED "No trains found for this route.\n" RESET);
    } else if(choice==2) {
        printf("Enter train name keyword: ");
        scanf("%s",keyword);
        printf("Trains found:\n");
        for(i=0;i<trainCount;i++) {
            if(strstr(trains[i].name, keyword)!=NULL) {
                printf("ID: %d | %s | %s -> %s | Fare: %.0f\n",
                    trains[i].id,trains[i].name,
                    trains[i].from,trains[i].to,trains[i].fare);
                found=1;
            }
        }
        if(!found) printf(RED "No trains found with keyword '%s'.\n" RESET, keyword);
    } else {
        printf(RED "Invalid choice.\n" RESET);
    }
    printf("Processing...\n");
    sleep(1);
}

void showSeatMap() {
    int tid,i;
    int available=0, booked=0;

    printf("Enter Train ID: ");
    scanf("%d",&tid);

    loadSeatMap(tid);

    printf("\nSeat Map\n\n");

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

    printf("\nAliases: M = Male, F = Female, C = Child, Empty = Available\n");
    printf("Booked Seats    : %d\n",booked);
    printf("Available Seats : %d\n",available);

    printf("\nProcessing.....\n");
}