#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h> // for kbhit/getch
    #define CLEAR "cls"
    #define sleep_ms(ms) Sleep(ms)   // Sleep takes milliseconds
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #define CLEAR "clear"
    #define sleep_ms(ms) usleep((ms)*1000) // usleep takes microseconds
#endif

int ROWX;
int ROWY;

int START_HOUR = 6;
int END_HOUR = 22;
int currHour;
int profit = 0;
int PRICE = 1;       // price per hour
int FINE = 100;      // fine for dragged car

typedef struct {
    int car;
    int status;
    int hour;
} parckingSpace;

parckingSpace **parckLot;

/* ---------------- MEMORY ---------------- */
void allocateParking()
{
    parckLot = malloc(ROWX * sizeof(parckingSpace*));
    for (int i = 0; i < ROWX; i++)
        parckLot[i] = malloc(ROWY * sizeof(parckingSpace));
}

void freeParking()
{
    for (int i = 0; i < ROWX; i++)
        free(parckLot[i]);
    free(parckLot);
}

/* ---------------- SETTINGS FILE ---------------- */
void saveSettings()
{
    FILE *f = fopen("settings.txt", "w");
    if (!f) return;
    fprintf(f, "%d %d %d %d %d %d", ROWX, ROWY, PRICE, FINE, START_HOUR, END_HOUR);
    fclose(f);
}

int loadSettings()
{
    FILE *f = fopen("settings.txt", "r");
    if (!f) return 0;
    if (fscanf(f, "%d %d %d %d %d %d", &ROWX, &ROWY, &PRICE, &FINE, &START_HOUR, &END_HOUR) != 6 ||
        ROWX <= 0 || ROWY <= 0 || PRICE < 0 || FINE < 0 ||
        START_HOUR < 0 || START_HOUR > 23 || END_HOUR <= START_HOUR || END_HOUR > 24)
    {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

void resetSettings()
{
    remove("settings.txt");
    printf("Saved settings deleted.\n");
}

/* ---------------- HELP ---------------- */
void showHelp()
{
    system(CLEAR);

    printf("\033[36m=== PARKING SYSTEM HELP ===\033[0m\n\n");

    printf("\033[33msimulate\033[0m  - Start parking simulation (%d:00 to %d:00)\n", START_HOUR, END_HOUR);
    printf("\033[33msettings\033[0m  - Change rows, columns, price, drag fine, and start/end hours\n");
    printf("\033[33mreset\033[0m     - Delete saved settings file\n");
    printf("\033[33mhelp\033[0m      - Show this help screen\n");
    printf("\033[33mquit\033[0m      - Exit program\n\n");

    printf("Simulation Info:\n");
    printf("- Cars randomly enter or exit\n");
    printf("- Disabled cars use disabled spots\n");
    printf("- Green = Free/Disabled\n");
    printf("- Red   = Occupied\n\n");

    printf("Current Settings:\n");
    printf("- Rows: %d\n", ROWX);
    printf("- Columns: %d\n", ROWY);
    printf("- Parking cost per hour: %d\n", PRICE);
    printf("- Drag fine: %d\n", FINE);
    printf("- Simulation start hour: %d:00\n", START_HOUR);
    printf("- Simulation end hour: %d:00\n\n", END_HOUR);

    printf("Press ENTER to return...");
    getchar();
}

/* ---------------- START ---------------- */
void start(parckingSpace **arr)
{
    int limit = ROWX * ROWY / 100;
    if (limit < 4) limit = 4;

    int count = 0;

    for (int x = 0; x < ROWX; x++) {
        for (int y = 0; y < ROWY; y++) {

            if (count < limit) {
                arr[x][y].status = 'D'; // disabled
                count++;
            } else {
                arr[x][y].status = 'O'; // free
            }

            arr[x][y].car = 0;
            arr[x][y].hour = 0;
        }
    }
}

/* ---------------- ENTER ---------------- */
void enter(parckingSpace **arr, int car, int disabled)
{
    for (int x = 0; x < ROWX; x++) {
        for (int y = 0; y < ROWY; y++) {

            if (disabled && arr[x][y].status == 'D') {
                arr[x][y].status = 'E'; // occupied disabled
                arr[x][y].car = car;
                arr[x][y].hour = currHour;
                printf("Car %d (Disabled) entered at row %d, col %d at hour %d\n",
                       car, x, y, currHour);
                return;
            }

            if (!disabled && arr[x][y].status == 'O') {
                arr[x][y].status = 'X'; // occupied
                arr[x][y].car = car;
                arr[x][y].hour = currHour;
                printf("Car %d entered at row %d, col %d at hour %d\n",
                       car, x, y, currHour);
                return;
            }
        }
    }

    printf("Car %d could not find a spot.\n", car);
}

/* ---------------- EXIT ---------------- */
char *exitPar(parckingSpace **arr, int x, int y)
{
    if (arr[x][y].status == 'O' || arr[x][y].status == 'D') {
        char *res = malloc(1);
        res[0] = '\0';
        return res;
    }

    char *res = malloc(100);
    int payment = (currHour - arr[x][y].hour) * PRICE;
    profit += payment;

    sprintf(res, "Car: %d Payment: %d\n", arr[x][y].car, payment);

    if (arr[x][y].status == 'E')
        arr[x][y].status = 'D';
    else
        arr[x][y].status = 'O';

    arr[x][y].car = 0;
    arr[x][y].hour = 0;

    return res;
}

/* ---------------- SHOW ---------------- */
void show(parckingSpace **arr)
{
    for (int i = 0; i < ROWX; i++) {
        for (int j = 0; j < ROWY; j++) {

            char *color;
            switch (arr[i][j].status) {
                case 'O':
                case 'D':
                    color = "\033[32m"; // green
                    break;
                case 'X':
                case 'E':
                    color = "\033[31m"; // red
                    break;
                default:
                    color = "\033[0m";
            }

            printf("%s%c\033[0m ", color, arr[i][j].status);
        }
        printf("\n");
    }
}

/* ---------------- DRAG ---------------- */
void grar(parckingSpace **arr)
{
    int cars = 0;

    printf("\n");

    for (int i = 0; i < ROWX; i++) {
        for (int j = 0; j < ROWY; j++) {

            if (arr[i][j].status == 'X' || arr[i][j].status == 'E') {
                printf("Row %d Col %d Car %d dragged\n", i, j, arr[i][j].car);
                profit += FINE;
                cars++;
            }
        }
    }

    printf("Total cars dragged: %d\n", cars);
}

/* ---------------- NON-BLOCKING INPUT ---------------- */
int keyPressed()
{
#ifdef _WIN32
    return kbhit();
#else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) return 1;
    return 0;
#endif
}

/* ---------------- SIMULATION ---------------- */
void simulate()
{
    currHour = START_HOUR;
    profit = 0;
    start(parckLot);

    while (currHour < END_HOUR)
    {
        system(CLEAR);
        printf("\nCurrent hour: %d\n", currHour);
        printf("Press any key to stop and return to menu...\n");

        if (rand() % 2 == 0)
            enter(parckLot, 1000000 + rand() % 9000000, rand() % 100 < 25);
        else
        {
            int x = rand() % ROWX;
            int y = rand() % ROWY;
            char *p = exitPar(parckLot, x, y);
            if (p[0] != '\0') printf("%s", p);
            free(p);
        }

        show(parckLot);

        if (rand() % 10 < 3)
            currHour++;

        if (keyPressed())
            break;

        int delay = 1000 + rand() % 3001; 
        sleep_ms(delay);
    }

    grar(parckLot);
    printf("Money made: %d\n", profit);
}

/* ---------------- MAIN ---------------- */
int main(void)
{
    srand(time(NULL));
    char command[20];

    if (!loadSettings())
    {
        printf("Enter initial rows: ");
        scanf("%d", &ROWX);
        printf("Enter initial columns: ");
        scanf("%d", &ROWY);
        printf("Enter price per hour: ");
        scanf("%d", &PRICE);
        printf("Enter drag fine amount: ");
        scanf("%d", &FINE);
        printf("Enter simulation start hour (0-23): ");
        scanf("%d", &START_HOUR);
        printf("Enter simulation end hour (1-24): ");
        scanf("%d", &END_HOUR);

        if (ROWX <= 0) {
    printf("Invalid rows value. Setting to default: 5\n");
    ROWX = 5;
        sleep_ms(2000);

}
if (ROWY <= 0) {
    printf("Invalid columns value. Setting to default: 10\n");
    ROWY = 10;
        sleep_ms(2000);

}
if (PRICE < 0) {
    printf("Invalid price value. Setting to default: 1\n");
    PRICE = 1;
        sleep_ms(2000);

}
if (FINE < 0) {
    printf("Invalid drag fine value. Setting to default: 100\n");
    FINE = 100;
        sleep_ms(2000);

}
if (START_HOUR < 0 || START_HOUR > 23) {
    printf("Invalid start hour. Setting to default: 6\n");
    START_HOUR = 6;
        sleep_ms(2000);

}
if (END_HOUR <= START_HOUR || END_HOUR > 24) {
    printf("Invalid end hour. Setting to default:%d\n", START_HOUR+1);
    END_HOUR = START_HOUR+1;
        sleep_ms(2000);

}


        saveSettings();
    }

    allocateParking();
    getchar(); // clear leftover newline

    while (1)
{
    printf("\nType 'simulate', 'settings', 'reset', 'help', or 'quit': ");
    scanf("%s", command);
    getchar(); // clear newline

    // convert to lowercase for safety (optional)
    for(int i = 0; command[i]; i++) 
        command[i] = tolower(command[i]);

    if (strncmp(command, "h", 1) == 0)             // help or h
        showHelp();
    else if (strncmp(command, "s", 1) == 0)        // simulate or settings
    {
        if (strcmp(command, "settings") == 0 || strcmp(command, "set") == 0)
        {
            freeParking();

            printf("Enter new rows: ");
            scanf("%d", &ROWX);
            printf("Enter new columns: ");
            scanf("%d", &ROWY);
            printf("Enter price per hour: ");
            scanf("%d", &PRICE);
            printf("Enter drag fine amount: ");
            scanf("%d", &FINE);
            printf("Enter simulation start hour (0-23): ");
            scanf("%d", &START_HOUR);
            printf("Enter simulation end hour (1-24): ");
            scanf("%d", &END_HOUR);

            if (ROWX <= 0) ROWX = 5;
            if (ROWY <= 0) ROWY = 10;
            if (PRICE < 0) PRICE = 1;
            if (FINE < 0) FINE = 100;
            if (START_HOUR < 0 || START_HOUR > 23) START_HOUR = 6;
            if (END_HOUR <= START_HOUR || END_HOUR > 24) END_HOUR = 22;

            allocateParking();
            saveSettings();

            printf("Settings updated.\n");
        }
        else                                        // simulate or sim
            simulate();
    }
    else if (strncmp(command, "r", 1) == 0)        // reset or r
        resetSettings();
    else if (strncmp(command, "q", 1) == 0)        // quit or q
    {
        freeParking();
        break;
    }
    else
        printf("Unknown command.\n");
}


    return 0;
}
