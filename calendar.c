#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
  short hour;
  short minute;
} Time;

typedef struct
{
  Time startTime;
  Time endTime;
  char *subject;
  char *location;
} Appointment;

typedef struct
{
  short day;
  short month;
  Appointment *appts[8];
  short apptCount;
} Day;

Day* addDate(Day *days, int month, int day, int count, int *size);
void cleanUp(Day *days, int count);
int findDate(Day *days, int count, int month, int day);
int getChoice();
void getDate(int *month, int *day);
void print(const Day &day);
void print(const Time &time);
void print(const Appointment *appt);
Appointment* readAppointment();
void readTime(Time *time);
void searchDate(Day *days, int count);
void searchSubject(Day *days, int count);




Day* addDate(Day *days, int month, int day, int count, int *size)
{
  int i;

  if(count + 1 >= *size)
  {
    Day *temp =  (Day*) malloc(sizeof(Day) * *size * 2);

    for(int i = 0; i < *size; i++)
      temp[i] = days[i];

    delete [] days;
    days = temp;
    *size *= 2;
  } // if need to resize

  for(i = 0; i < count; i++)
    if(days[i].month > month || (days[i].month == month && days[i].day > day))
      break;

  for(int j = count - 1; j >= i; --j)
    days[j + 1] = days[j];  // roll entries toward end

  days[i].month = month;
  days[i].day = day;
  days[i].apptCount = 0;
  return days;
} // addDate()


void cleanUp(Day *days, int count)
{
  for(int i = 0; i < count; i++)
    for(int j = 0; j < days[i].apptCount; j++)
    {
      free(days[i].appts[j]->subject);
      free(days[i].appts[j]->location);
    } // for j

  free(days);
} // cleanUp()


int findDate(Day *days, int count, int month, int day)
{
  for(int i = 0; i < count; i++)
    if(days[i].month == month && days[i].day == day)
      return i;

  return count; // not found
} // findDate()


int getChoice()
{
  int choice;
  char s[80];

  do
  {
    printf("Calendar Menu\n0. Done.\n1. Search for date.\n");
    printf("2. Search for subject.\n\nYour choice >> ");
    scanf("%d", &choice);

    if(choice < 0 || choice > 2)
      printf("\nChoice must be between 0 and 2.\nPlease try again.\n\n");
  } while(choice < 0 || choice > 2);

  fgets(s, 80, stdin);
  return choice; // eat end of line
} // getChoice()


void getDate(int *month, int *day)
{
  char s[80], s2[80], *ptr;

  do
  {
    *month = *day = 0;
    printf("\nPlease enter the month and day (mm/dd) >> ");
    fgets(s, 80, stdin);
    s[strlen(s) - 1] = '\0';  // chop off \n
    strcpy(s2, s);
    ptr = strtok(s2,"/");

    if(ptr)
    {
      *month = atoi(ptr);
      ptr = strtok(NULL, ",");
      if(ptr)
        *day = atoi(ptr);
    }

    if(*month < 1 || *month > 12 || *day < 1 || *day > 31)
      printf("%s is not a valid date.\nPlease try again.\n\n", s);
  } while(*month < 1 || *month > 12 || *day < 1 || *day > 31);

} //getDate()


void print(const Day &day)
{
  printf("%2d/%-2d ", day.month, day.day);
} // print()


void print(const Time &time)
{
  printf("%02d:%02d ", time.hour, time.minute);
} // print()


void print(const Appointment *appt)
{
  print(appt->startTime);
  print(appt->endTime);
  printf("%-15s %s\n", appt->subject, appt->location);
} // print()


Appointment* readAppointment()
{
  char *ptr;

  Appointment *apptPtr =  (Appointment*) malloc(sizeof(Appointment));
  ptr = strtok(NULL, ",");
  apptPtr->subject = (char*) malloc(strlen(ptr) + 1);
  strcpy(apptPtr->subject, ptr);
  readTime(&apptPtr->startTime);
  readTime(&apptPtr->endTime);
  ptr = strtok(NULL, ",");
  apptPtr->location = (char*) malloc(strlen(ptr) + 1);
  strcpy(apptPtr->location, ptr);
  return apptPtr;
} // readAppointment()


Day* readFile(int *count)
{
  char s[256];
  int month, day, pos, size = 30;
  FILE *fp = fopen("appts.csv", "r");
  *count = 0;
  Day *days = (Day*) malloc(sizeof(Day) * size);
  fgets(s, 256, fp); // eat titles.

  while(fgets(s, 256, fp))
  {
    s[strlen(s) - 1] = '\0';  // eliminate \n
    month = atoi(strtok(s,"/"));
    day = atoi(strtok(NULL,"/"));
    strtok(NULL, ","); // read through 2003
    pos = findDate(days, *count, month, day);

    if(pos == *count)
    {
      days = addDate(days, month, day, (*count)++, &size);
      pos = findDate(days, *count, month, day);
    } // if date not found

    days[pos].appts[ days[pos].apptCount++] = readAppointment();
  } // while more to read

  fclose(fp);
  return days;
} // readFile()



void readTime(Time *time)
{
  time->hour = atoi(strtok(NULL, ":"));
  time->minute = atoi(strtok(NULL, ":"));
  char *ptr = strtok(NULL, ",");

  if(strchr(ptr, 'P') && time->hour != 12)
    time->hour += 12;
  else
    if(strchr(ptr, 'A') && time->hour == 12)
      time->hour = 0;
} // readTime()


void searchDate(Day *days, int count)
{
  int month, day;
  bool found = false;

  getDate(&month, &day);

  for(int i = 0; i < count; i++)
    if(days[i].month == month && days[i].day == day)
    {
      printf("Start End   Subject         Location\n");
      for(int j = 0; j < days[i].apptCount; j++)
      {
        print(days[i].appts[j]);
        found = true;
      } // for each day
      break;
    }  // if date matches

  if(!found)
    printf("There are no appointments for that date.\n");

  printf("\n");
} // searchDate()


void searchSubject(Day *days, int count)
{
  char s[80];
  bool found = false;

  printf("\nPlease enter the subject >> ");
  fgets(s, 80, stdin);
  s[strlen(s) - 1] = '\0'; // overwrite \n

  for(int i = 0; i < count; i++)
    for(int j = 0; j < days[i].apptCount; j++)
      if(strcmp(days[i].appts[j]->subject, s) == 0)
      {
        if(!found)
          printf("Date  Start End   Subject         Location\n");

        print(days[i]);
        print(days[i].appts[j]);
        found = true;
      }

  if(!found)
    printf("%s was not found as a subject in the calendar.\n", s);

  printf("\n");
} // searchSubject()


int main()
{
  Day *days;
  int choice, count;

  days = readFile(&count);
  choice = getChoice();

  while(choice != 0)
  {
    if(choice == 1)
      searchDate(days, count);
    else // choice == 2
      searchSubject(days, count);

    choice = getChoice();
  } // while choice not Done

  cleanUp(days, count);
  return 0;
} // main()

