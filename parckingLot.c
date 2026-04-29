#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h> // for kbhit/getch
    #define CLEAR "cls"
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #define CLEAR "clear"
    #define sleep_ms(ms) usleep((ms)*1000)
#endif

int ROWX;
int ROWY;

int START_HOUR = 6;
int END_HOUR = 22;
int currHour;
int profit = 0;
int PRICE = 1;       
int FINE = 100;      

typedef struct {
    int car;
    int status;
    int hour;
} parckingSpace;

parckingSpace **parckLot;

/* ---------------- TERMINAL UTILS ---------------- */
// Hides the cursor for a cleaner TUI feel
void hideCursor() {
    printf("\033[?25l");
}

// Restores the cursor before exiting
void showCursor() {
    printf("\033[?25h");
}

#ifndef _WIN32
// Cross-platform getch() for Linux/macOS to read single keypresses
int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}
#endif

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
void logCarPayment(int car, int payment, const char *type)
{
    FILE *f = fopen("carsHistory.txt", "a");
    if (!f) return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    char date[64];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M", tm_info);

    fprintf(f, "Date: %s | Car: %d | Payment: %d | Type: %s\n",
            date, car, payment, type);

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
    printf("\033[38;5;212mSaved settings deleted. They will be recreated on next boot.\033[0m\n");
}

/* ---------------- HELP ---------------- */
void showHelp()
{
    system(CLEAR);
    showCursor();

    printf("\033[38;5;212m=== PARKING SYSTEM HELP ===\033[0m\n\n");

    printf("Simulation Info:\n");
    printf("- Cars randomly enter or exit\n");
    printf("- Disabled cars use disabled spots\n");
    printf("- \033[32mGreen\033[0m = Free/Disabled\n");
    printf("- \033[31mRed\033[0m   = Occupied\n\n");

    printf("Current Settings:\n");
    printf("- Rows: %d\n", ROWX);
    printf("- Columns: %d\n", ROWY);
    printf("- Parking cost per hour: %d\n", PRICE);
    printf("- Drag fine: %d\n", FINE);
    printf("- Simulation start hour: %d:00\n", START_HOUR);
    printf("- Simulation end hour: %d:00\n\n");

    printf("\033[38;5;241mPress ENTER to return...\033[0m");
    getchar();
    hideCursor();
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

    printf("\033[38;5;241mCar %d could not find a spot.\033[0m\n", car);
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
    logCarPayment(arr[x][y].car, payment, "Exit");
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
                    logCarPayment(arr[i][j].car, profit, "Dragged");
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
/* ------------------ CAR LOG --------------------- */
void showCarHistory()
{
    FILE *f = fopen("carsHistory.txt", "r");
    if (!f) {
        printf("No history found.\n");
        getchar();
        return;
    }

    system(CLEAR);
    showCursor();

    printf("\033[1;38;5;212m=== Cars History ===\033[0m\n\n");

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }

    fclose(f);

    printf("\n\033[38;5;241mPress ENTER to return...\033[0m");
    getchar();

    hideCursor();
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
        printf("\n\033[1;38;5;212m[ SIMULATION RUNNING ]\033[0m\n");
        printf("Current hour: %d:00\n", currHour);
        printf("\033[38;5;241mPress any key to stop and return to menu...\033[0m\n\n");

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
        {
            // clear the pressed key from buffer
            #ifdef _WIN32
                getch();
            #else
                getchar();
            #endif
            break;
        }

        int delay = 1000 + rand() % 3001; 
        sleep_ms(delay);
    }

    grar(parckLot);
    printf("\033[1;32mMoney made: %d\033[0m\n", profit);
    printf("\n\033[38;5;241mPress ENTER to continue...\033[0m\n");
    
    // Clear buffer and wait
    while(keyPressed()) getchar();
    getchar();
}

/* ---------------- MAIN ---------------- */
int main(void)
{
    srand(time(NULL));

    if (!loadSettings())
    {
        system(CLEAR);
        printf("\033[1;38;5;212m=== First Time Setup ===\033[0m\n\n");
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
            printf("\033[38;5;241mInvalid rows. Setting to 5.\033[0m\n");
            ROWX = 5;
            sleep_ms(1000);
        }
        if (ROWY <= 0) {
            printf("\033[38;5;241mInvalid columns. Setting to 10.\033[0m\n");
            ROWY = 10;
            sleep_ms(1000);
        }
        if (PRICE < 0) {
            printf("\033[38;5;241mInvalid price. Setting to 1.\033[0m\n");
            PRICE = 1;
            sleep_ms(1000);
        }
        if (FINE < 0) {
            printf("\033[38;5;241mInvalid fine. Setting to 100.\033[0m\n");
            FINE = 100;
            sleep_ms(1000);
        }
        if (START_HOUR < 0 || START_HOUR > 23) {
            printf("\033[38;5;241mInvalid start hour. Setting to 6.\033[0m\n");
            START_HOUR = 6;
            sleep_ms(1000);
        }
        if (END_HOUR <= START_HOUR || END_HOUR > 24) {
            printf("\033[38;5;241mInvalid end hour. Setting to %d.\033[0m\n", START_HOUR+1);
            END_HOUR = START_HOUR+1;
            sleep_ms(1000);
        }

        saveSettings();
        // Clear leftover newline from scanf
        int c; while ((c = getchar()) != '\n' && c != EOF);
    }

    allocateParking();

    // Charm-style interactive TUI menu
    const char *options[] = {
        "Simulate",
        "Settings",
        "Show Car History",
        "Reset Defaults",
        "Help",
        "Quit"
    };
    int num_options = 6;
    int selected = 0;

    hideCursor();

    while (1)
    {
        system(CLEAR);
        
        // Header
        printf("\n  \033[1;38;5;212mParking Simulation TUI\033[0m\n");
        printf("  \033[38;5;241mUse arrow-up/arrow-down or j/k to navigate & Enter to select\033[0m\n\n");

        // Render Menu Options
        for (int i = 0; i < num_options; i++) {
            if (i == selected) {
                // Charm Pink cursor & text highlight
                printf("  \033[1;38;5;212m> %s\033[0m\n", options[i]);
            } else {
                // Dim unselected text
                printf("    \033[38;5;246m%s\033[0m\n", options[i]);
            }
        }

        // Wait for keypress
        #ifdef _WIN32
            int c = _getch();
        #else
            int c = getch();
        #endif

        // Arrow Key / Navigation Logic
        if (c == 224 || c == 27) { // Special char for arrows (Windows 224, Unix 27/ESC)
            #ifdef _WIN32
                int next = _getch();
                if (next == 72) c = 'k'; // Up
                if (next == 80) c = 'j'; // Down
            #else
                int next1 = getch();
                if (next1 == '[') {
                    int next2 = getch();
                    if (next2 == 'A') c = 'k'; // Up
                    if (next2 == 'B') c = 'j'; // Down
                }
            #endif
        }

        // Apply Selection
        if (c == 'k' || c == 'K' || c == 'w' || c == 'W') {
            selected--;
            if (selected < 0) selected = num_options - 1;
        } 
        else if (c == 'j' || c == 'J' || c == 's' || c == 'S') {
            selected++;
            if (selected >= num_options) selected = 0;
        } 
        else if (c == '\r' || c == '\n') {
            // Execute selected option
         if (selected == 0) {
    simulate();
} 
else if (selected == 1) {
    system(CLEAR);
    showCursor();
    freeParking();

    printf("\033[1;38;5;212m=== Settings ===\033[0m\n\n");

    printf("Rows: ");
    scanf("%d", &ROWX);

    printf("Columns: ");
    scanf("%d", &ROWY);

    printf("Price per hour: ");
    scanf("%d", &PRICE);

    printf("Drag fine: ");
    scanf("%d", &FINE);

    printf("Start hour (0-23): ");
    scanf("%d", &START_HOUR);

    printf("End hour (1-24): ");
    scanf("%d", &END_HOUR);

    allocateParking();
    saveSettings();

    int flush; while ((flush = getchar()) != '\n' && flush != EOF);
    hideCursor();

    printf("\n\033[1;32mSettings updated.\033[0m\n");
    sleep_ms(1000);
} 
else if (selected == 2) {
    showCarHistory();   // FIXED
} 
else if (selected == 3) {
    resetSettings();    // FIXED
    sleep_ms(1500);
} 
else if (selected == 4) {
    showHelp();
} 
else if (selected == 5) {
    freeParking();
    break;
}
        }
    }
    showCursor();
    system(CLEAR);
    return 0;
}