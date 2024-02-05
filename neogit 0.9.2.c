#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <windows.h>

#define MAX_FILENAME_LENGTH 1000
#define MAX_COMMIT_MESSAGE_LENGTH 2000                              
#define MAX_LINE_LENGTH 1000
#define MAX_MESSAGE_LENGTH 1000  
#define BUFFER_SIZE 1024
#define MAX_BUFFER_SIZE 1024   
#define m(a, b) strcmp( a, b);

#define debug(x) printf("--------%s--------\n", x);

int change_color(char color){
    HANDLE hConsole;    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (color == 'r')        SetConsoleTextAttribute(hConsole, 4);
    else if (color == 'b')        SetConsoleTextAttribute(hConsole, 3);
    else if (color == 'y')        SetConsoleTextAttribute(hConsole, 6);
    else if (color == 'k')        SetConsoleTextAttribute(hConsole, 8);
    else if (color == 'p')        SetConsoleTextAttribute(hConsole, 5);
    else if (color == 'g')        SetConsoleTextAttribute(hConsole, 2);
    else if (color == 'R')        SetConsoleTextAttribute(hConsole, 64);
    else if (color == 'B')
        SetConsoleTextAttribute(hConsole, 48);    else if (color == 'Y')
        SetConsoleTextAttribute(hConsole, 96);    else if (color == 'G')
        SetConsoleTextAttribute(hConsole, 32);    else if (color == 'K')
        SetConsoleTextAttribute(hConsole, 128);    else if (color == 'P')
        SetConsoleTextAttribute(hConsole, 208);    else if (color == 'W')
        SetConsoleTextAttribute(hConsole, 240);    return 0;
}

int reset_color(){
    HANDLE hConsole;    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);    return 0;
}

int white(const char *filename) { 
    FILE *file = fopen(filename, "r"); 
    if (file == NULL) { 
        perror("Error opening file"); 
        return -1; 
    } 
    int result = 0; 
    fseek(file, 0, SEEK_END); 
    long fileSize = ftell(file); 
    fseek(file, 0, SEEK_SET); 
    if (fileSize > 0) { 
        char *buffer = (char *)malloc(MAX_BUFFER_SIZE * sizeof(char)); 
        if (buffer == NULL) { 
            fclose(file); 
            perror("Memory allocation error"); 
            return -1; 
        } 
        fseek(file, -MAX_BUFFER_SIZE, SEEK_END); 
        size_t bytesRead = fread(buffer, 1, MAX_BUFFER_SIZE, file); 
        if (bytesRead > 0) { 
            for (size_t i = bytesRead - 1; i >= 0; --i) { 
                if (buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n' || buffer[i] == '\r') { 
                    result = 1; 
                } 
                else { break; } 
            } 
        } 
        free(buffer); 
    } 
    fclose(file); 
    return result; 
} 
 
void removewhite(const char *filename){ 
    FILE *file = fopen(filename, "r+"); 
    if (file == NULL) { 
        perror("Error opening file"); 
        return; 
    } 
    fseek(file, 0, SEEK_END); 
    long fileSize = ftell(file); 
    fseek(file, 0, SEEK_SET); 
    if (fileSize > 0) { 
        char *buffer = (char *)malloc(MAX_BUFFER_SIZE * sizeof(char)); 
        if (buffer == NULL) { 
            fclose(file); 
            perror("Memory allocation error"); 
            return; 
        } 
        fseek(file, -MAX_BUFFER_SIZE, SEEK_END); 
        size_t bytesRead = fread(buffer, 1, MAX_BUFFER_SIZE, file); 
        if (bytesRead > 0) { 
            size_t index; 
            for (index = bytesRead - 1; index >= 0; --index) { 
                if (buffer[index] != ' ' && buffer[index] != '\t' && buffer[index] != '\n' && buffer[index] != '\r') { 
                    break; 
                } 
            } 
            ftruncate(fileno(file), ftell(file) - (bytesRead - index - 1)); 
        } 
        free(buffer); 
    } 
    fclose(file); 
}

int parseTime(const char *timeString, struct tm *tmResult) { 
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; 
    char month[4]; 
    int day, year, hour, minute, second; 
 
    if (sscanf(timeString, "%*s %3s %d %d:%d:%d %d", month, &day, &hour, &minute, &second, &year) != 6) { 
        fprintf(stderr, "Error parsing time string\n"); 
        return 0; 
    } 
    int monthIndex; 
    for (monthIndex = 0; monthIndex < 12; ++monthIndex) { 
        if (strcmp(month, months[monthIndex]) == 0) { 
            break; 
        } 
    } 
 
    if (monthIndex == 12) { 
        fprintf(stderr, "Invalid month in time string\n"); 
        return 0; 
    } 
 
    memset(tmResult, 0, sizeof(struct tm)); 
    tmResult->tm_sec = second; 
    tmResult->tm_min = minute; 
    tmResult->tm_hour = hour; 
    tmResult->tm_mday = day; 
    tmResult->tm_mon = monthIndex; 
    tmResult->tm_year = year - 1900; 
 
    return 1; 
} 

double calculateTimeDifference(const char *timeString1, const char *timeString2) { 
    struct tm tm1, tm2; 
    time_t time1, time2; 
 
    if (!parseTime(timeString1, &tm1)) { 
        fprintf(stderr, "Error parsing time string 1\n"); 
        return -1.0; 
    } 
    time1 = mktime(&tm1); 
 
    if (!parseTime(timeString2, &tm2)) { 
        fprintf(stderr, "Error parsing time string 2\n"); 
        return -1.0; 
    } 
    time2 = mktime(&tm2); 

    return difftime(time2, time1); 
} 
 
int compareFiles(const char *file1, const char *file2) {  
    FILE *f1 = fopen(file1, "rb");  
    FILE *f2 = fopen(file2, "rb");  
  
    if (f1 == NULL){
        return -1;
    }
    if (f2 == NULL) {    
        return -2;  
    }  
  
    int result = 1; // Assume files are identical  
  
    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];  
    size_t bytesRead1, bytesRead2;  
  
    do {  
        bytesRead1 = fread(buffer1, 1, sizeof(buffer1), f1);  
        bytesRead2 = fread(buffer2, 1, sizeof(buffer2), f2);  
  
        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {  
            result = 0; // Files are different  
            break;  
        }  
  
    } while (bytesRead1 > 0 && bytesRead2 > 0);  
  
    fclose(f1);  
    fclose(f2);  
    return result;  
}  

int number_out ( char * word ){
    int sum = 0, t = 1;
    for (int i = strlen( word) - 1; i >= 0 ; i--){
        sum += t * ( word [i] - '0');
        t = t * 10;
    } return sum;
}

void find_name(char * name){
    char cwd[1024];
    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening current directory");
        }
        while ((entry = readdir(dir)) != NULL) {                         ///////////////////////// CHANGE DI_DIR ///////////////////////
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, name) == 0){
                exists = true;
                break;
            }
        } 
        closedir(dir);
        // update current working directory
        getcwd(tmp_cwd, sizeof(tmp_cwd));
        // change cwd to parent
        if (strlen(tmp_cwd) > 4 && !exists) {
            chdir("..") != 0 ;
        }
    } while (strlen(tmp_cwd) > 4 && !exists);
}

int is_wild(char *wild) { 
    for (int i = 0; i < strlen(wild); i++) { 
        if (*(wild + i) == '*') { 
            return 1;
        } 
    } 
    return 0; 
}

int number_of_lines( char * add ){
    FILE * ptr = fopen( add, "r");
    int i = 0 ;
    char line[1000];
    while ( fgets ( line, sizeof(line), ptr) != NULL ) {
        i++;
    } fclose( ptr ) ; return i;
}

void change_word ( char * word, char a, char b){
    for ( int i = 0 ; i < strlen(word) ; i++){
        if (word[i] == a) {word[i] = b;
        }
    } 
}

int fins( char * first, char * second , char a[1000]){
    char * words1[100];
    char * words2[100];
    char * token = strtok(first, "^");
    int number1 = 0;
     while ( token != NULL){
        words1[number1] = token;
        number1++;
        token = strtok(NULL ,"^");
    }
    char * token2 = strtok(second, "^");
    int number2 = 0;
     while ( token2 != NULL){
        words2[number2] = token2;
        number2++;
        token2 = strtok(NULL , "^");
    }
    if ( number1 <= number2){
        for ( int i = 0 ; i < number1 ; i++){
            if ( strcmp ( words1[i], words2[i])) return 0 ;
        }return number2 - number1 + 1;
    } return 0;
}

int is_file(char *path) { 
    struct stat file_stat; 
    if (stat(path, &file_stat) == 0) { 
        if (S_ISREG(file_stat.st_mode)) { 
            return 1; 
        } 
        else if (S_ISDIR(file_stat.st_mode)) { 
            return 0; 
        } 
        else { 
            return -1; 
        } 
    } 
    else { 
        return -1; 
    } 
}

char * out_line(int number, char * add, char * line2){
    FILE * ptr;
    ptr = fopen(add, "r");
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, ptr);
    for ( int i = 0 ; i < number ; i++){
        fgets(line, MAX_LINE_LENGTH, ptr);
    } line[strlen(line) - 1] = '\0';
    strcpy(line2, line);
    fclose(ptr);
    return line2;
}

int todo_in_code(FILE *file) { 
    char line[256]; 
    int insideBlockComment = 0 , insideSingleLineComment = 0; 
    while (fgets(line, sizeof(line), file) != NULL) { 
        char *todoInComment = strstr(line, "TODO"); 
        if (strstr(line, "/*") != NULL) { 
            insideBlockComment = 1; 
        } 
        if (insideSingleLineComment == 0) { 
            char *singleLineCommentStart = strstr(line, "//"); 
            if (singleLineCommentStart != NULL) { 
                char *todoInSingleLineComment = strstr(singleLineCommentStart, "TODO"); 
                if (todoInSingleLineComment != NULL) { 
                    return 1; 
                } 
            } 
        } 
        if ((insideBlockComment || insideSingleLineComment) && todoInComment != NULL) { 
            return 1; 
        } 
        if (strstr(line, "*/") != NULL) { 
            insideBlockComment = 0; 
        } 
    } 
    return 0; 
}

void add_to_file( char * entry, char * add){
    FILE *ptr = fopen(add, "a");
    fprintf(ptr, "%s\n", entry);
    fclose(ptr);
}

int word_in_file ( char * add , char * word , int flag) {

    for ( int i = 0 ; i < number_of_lines ( add ) ; i++){
        char line[1000]; out_line ( i , add , line );
        if ( strstr ( line , word) != NULL ){
            if ( flag == 1) {
                printf ( "%s\n" , line );
            } if ( flag == 2){
                printf ( "%d %s\n", i + 1 , line );
            } return 1 ;
        }
    }
    return 0; 
}

int comperror(const char *filename, int isCpp) { 
    const char *compiler = isCpp ? "g++" : "gcc"; 
    char command[256]; 
    snprintf(command, sizeof(command), "%s -o NUL -Wall -Wextra -Werror %s > NUL 2>&1", compiler, filename); 
    int result = system(command); 
    return result == 0 ? 1 : 0; 
}
int checker (char * hook_id , char * path , int flag ){
    reset_color();
    char * format = strrchr ( path , '.') ;
    format = format + 1;
    if ( strcmp ( hook_id , "file-size-check") == 0){
        FILE * file = fopen ( path , "r" );
        fseek(file, 0, SEEK_END); 
        long size = ftell(file);
        if ( size > 5 * 1024 * 1024 ){
            if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); }
            return 0;
        }
        if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); }
        return 1;
    } 
    else if ( strcmp(hook_id,"character-limit")==0 && ( strcmp( format , "c")==0 || strcmp( format , "cpp")==0 || strcmp( format , "txt")==0)){
        FILE * file = fopen ( path , "r" );
        fseek(file, 0, SEEK_END); 
        long size = ftell(file);
        if ( size > 20000 ){
            if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); }
            return 0;
        }
        if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); }
        return 1;
    }
    else if ( strcmp( hook_id , "format-check" ) == 0 ) {
        if ( ( strcmp( format , "c")==0 || strcmp( format , "cpp")==0 || strcmp( format , "txt")==0 || strcmp( format , "mp4")==0 
        || strcmp( format , "mp3")==0 || strcmp( format , "wav")==0 || format == NULL)){
            if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); } return 1;
        }
        else { 
            if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); } return 0;
        }
    } 
    else if ( strcmp ( hook_id , "todo-check") == 0 ) {
        if ( strcmp( format , "cpp")==0 || strcmp( format , "c")==0 ){
            if ( todo_in_code ( fopen(path, "r") )){
                if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); } return 0;
            } else {
                if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); } return 1;
            }
        } else if ( strcmp ( format , "txt") == 0){
            if ( word_in_file (path , "TODO" , 0)){
                if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); } return 0;
            } else {
                if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); } return 1;
            }
        }
    }
    else if ( strcmp ( hook_id , "static-error-check" ) == 0){
        if ( strcmp ( format , "cpp") == 0 || strcmp ( format , "c") == 0){
            if ( comperror ( path , strcmp ( format , "c")) == 0) {
                if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); } return 0;
            } else {
                if ( flag ) {printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); } return 1;
            }
        }

    }
    else if ( strcmp ( hook_id , "eof-blank-space") == 0 && ( strcmp( format , "c")==0 || strcmp( format , "cpp")==0 || strcmp( format , "txt")==0) ){
        if ( white ( path ) ) {
                if ( flag ) {
                    printf ( "%s--------------------------------------------------------", hook_id); change_color('r'); printf ("FAILED\n"); 
                    if ( flag == 2) removewhite ( path );} return 0;
        } else {
                if ( flag ) {
                    printf ( "%s--------------------------------------------------------", hook_id); change_color('g'); printf ("PASSED\n"); 
                    if ( flag == 2) removewhite ( path );} return 1;
        } 


    }
    if ( flag ) { printf ( "%s--------------------------------------------------------", hook_id); change_color('y'); printf ( "SKIPPED\n"); }
    return -1;
}

void tree(const char *dirPath,  char * address) { 
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        char path[1024]; 
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name); 
        if (entry->d_type == DT_DIR) { 
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp ( entry->d_name, ".neogit") != 0) { 
                tree(path, address); 
            } 
        } else {
            add_to_file(path, address);
            }
        } 
    closedir(dir); 
}

void tree_dir_include(const char *dirPath,  char * address) { 
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        char path[1024]; 
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name); 
        if (entry->d_type == DT_DIR) { 
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp ( entry->d_name, ".neogit") != 0) { 
                tree_dir_include(path, address); 
            } 
        } 
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp ( entry->d_name, ".neogit")) add_to_file ( path , address );
    } 
    closedir(dir); 
}

void add_tree(const char *dirPath,  char * address, char * argv) { 
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        char path[1024]; 
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name); 
        if (entry->d_type == DT_DIR) { 
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp ( entry->d_name, ".neogit") != 0) { 
                tree(path, address); 
            } 
        } else {
            char command[1000]; 
            snprintf ( command, sizeof(command), "neogit %s %s", argv, path);
            system(command);
            }
        } 
    closedir(dir); 
}

char * get_last_branch_commit(char * branch_name){
    char cwd[1000]; getcwd(cwd, sizeof(cwd));
    find_name(".neogit");
    FILE * ptr = fopen(".neogit/branch", "r");
    char line[1000]; int i = 0;
    while ( fgets( line, sizeof(line), ptr) != NULL){ line[strlen(line) - 1] = '\0';
        if ( strcmp ( line, branch_name ) == 0){
            chdir(cwd);
            fclose(ptr);
            return ( out_line ( i + 1 , ".neogit/branch", line));
        } i++;
    }
} 

void delete_line(const char *filename, int lineToDelete) { 
    FILE *originalFile = fopen(filename, "r"); 
    FILE *tempFile = fopen("temp.txt", "w"); 
 
    if (originalFile == NULL || tempFile == NULL) { 
        perror("Error opening files"); 
        exit(EXIT_FAILURE); 
    } 
 
    char buffer[1024]; 
    int currentLine = 1; 
 
    while (fgets(buffer, sizeof(buffer), originalFile) != NULL) { 
        if (currentLine != lineToDelete) { 
            fputs(buffer, tempFile); 
        } 
        currentLine++; 
    } 
 
    fclose(originalFile); 
    fclose(tempFile); 
 
    // Remove the original file 
    if (remove(filename) != 0) { 
        perror("Error removing original file"); 
        exit(EXIT_FAILURE); 
    } 
 
    // Rename temp file to original file 
    if (rename("temp.txt", filename) != 0) { 
        perror("Error renaming file"); 
        exit(EXIT_FAILURE); 
    } 
} 

int show_status(char* path, char * commit_address){ // path alan ro migire ba commit moghayese mikone

    char new_path[1000]; strcpy( new_path , path); char new_commit_address[1000];
    change_word( new_path , '\\', '^');
    strcpy( new_path, new_path + 2);

    snprintf(new_commit_address, sizeof(new_commit_address), "%s\\%s", commit_address, new_path);

    return compareFiles(path, new_commit_address);
}

void tree_status(const char *dirPath,  char * address) { 
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        char path[1024]; 
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name); 
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit")) { 
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) { 
                tree_status(path, address); 
            } 
        } else if ( entry->d_type != DT_DIR){
            int x  = show_status( path , address);
            char cwd[1000];getcwd(cwd, sizeof(cwd));
            find_name(".neogit");
            char flag = '-'; char line[1000];
            char second_flag = 'U';
            for ( int i = 0 ; i < number_of_lines (".neogit/staging") ; i++){
                 out_line(i, ".neogit/staging", line);
                if ( strcmp (line, path) == 0){
                    flag = '+'; break;
                }
            }
            if ( x == -1) second_flag = 'D';
            if ( x == -2) second_flag = 'A';
            if ( x == 0 ) second_flag = 'M';
            if ( second_flag != 'U')
            printf("%s %c%c\n",entry->d_name, flag, second_flag);
            }
        } 
    closedir(dir); 
}

int copy_file(int number,  char * add, char * new_add, char * entry){
    FILE * ptr = fopen(add, "r"); if (ptr == NULL) return 1;
    FILE * new_file = fopen(new_add, "w");
    int i = 0 ;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, ptr) != NULL){
        if ( i == number){
            fprintf(new_file, "%s\n", entry);
        } else{
            fprintf(new_file, "%s", line);
        } i++;
    } 
    fclose(new_file);
    fclose(ptr);
    remove(add);
    rename(new_add, add);
}

int remove_directory( char * add ){
    char command[1000];
    snprintf(command, sizeof(command), "rmdir /s /q %s", add);
    system(command);
}

int number_of_files ( char * add ){
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(add))) { 
        perror("opendir"); 
        return -1; 
    } 
    int i = 0 ;
    while ((entry = readdir(dir)) != NULL) { 
        if ( entry->d_type == DT_REG) i++;
    } return i;
}

void print_command(int argc, char * const argv[]);

int run_init(int argc, char * const argv[]);

int create_configs(char *username, char *email, int halat);

char * get_last_commit_id(char * id){
    char cwd[1000]; getcwd(cwd, sizeof(cwd));
    find_name(".neogit");
    out_line(2,".neogit/config", id);
    chdir(cwd);
    return ( id );
}

void out_log(char * add){
    char line[1000];
    out_line(5, "details", line);
    printf("time : %s\n", line);
    out_line(6, "details", line);
    printf("message : %s\n", line);
    out_line(3, "details", line);
    printf("user-name : %s\n", line);
    out_line(0, "details", line);
    printf("id : %s\n", line);
    out_line(1, "details", line);
    printf("branch : %s\n", line);
    chdir("..");
    int n = number_of_files(add);
    printf("number of files : %d\n---------------\n", n - 3);
}

int run_add(int argc, char * const argv[]);

int add_to_staging(char *filepath);

int search_for_line ( char * entry , char * add){
    char line[1000];
    for (int i = 0 ; i < number_of_lines( add ) ; i++){
        out_line(i, add, line);
        if ( strcmp (entry , line) == 0) return i;
    } return -1;
}

int run_reset(int argc, char * const argv[]);

int remove_from_staging(char *filepath);

int tree_maker( char * dirPath, char * destionation){
    char neo_add[1000]; getcwd ( neo_add, sizeof ( neo_add ));
    int lenth = strlen ( destionation );
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return 1; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        if ( strcmp ( entry->d_name , ".") && strcmp ( entry->d_name , "..") ){

            if ( strcmp ( entry->d_name , "details") && strcmp ( entry->d_name , "deleting") && strcmp ( entry->d_name , "staging")){
        
                char name[1000]; strcpy ( name , entry->d_name );
                char correct_path [1000] = "C:";
                strcat( correct_path, entry->d_name); 
                change_word ( correct_path , '^', '\\');
                strcpy ( correct_path , correct_path + lenth + 1 );
                
                char * last_tmp = ( char * ) malloc ( sizeof ( char ) * 1000);
                char * tmp_correct_path = ( char * ) malloc ( sizeof ( char ) * 1000);

                last_tmp = strrchr ( correct_path , '\\');
                tmp_correct_path ; strcpy ( tmp_correct_path , correct_path );

                char * token = strtok ( tmp_correct_path , "\\");
                char last_tok [1000];
                
                while ( token != last_tmp && token != NULL){
                    strcpy ( last_tok , token );
                    mkdir ( token );
                    chdir ( token );
                    token = strtok ( NULL , "\\");
                    debug( token );
                } 
                chdir ("..");
                rmdir ( last_tok );
                char command[1000];
                printf("-----------------\n");
                snprintf ( command , sizeof ( command ), "copy /Y %s\\%c%s%c %s\\%s", dirPath , 34,  name, 34 , neo_add, correct_path);
                printf("----------------\n");
                chdir ( destionation );
                system ( command );

                system ( "cd" );


            }
        }






        } 
    closedir(dir);
    return 1;
}

void remove_tree(const char *dirPath,  char * address) { 
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        char cwd[1000]; getcwd(cwd, sizeof(cwd));
        char path[1024]; 
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name); 
        if (entry->d_type == DT_DIR) { 
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp ( entry->d_name, ".neogit") && search_for_line( path, address ) == -1) { 
                remove_tree(path, address); 
            } else if ( search_for_line( path, address ) != -1) {
                char command[1000] = "rmdir /s /q ";
                strcat ( command , path );
                system ( command );
            }
        } else {
            if ( search_for_line ( path , address) != -1) { 
                chdir ( dirPath );
                remove(entry->d_name);
                chdir ( cwd );
                }
            }
        } 
    closedir(dir); 
}

int run_commit(int argc, char * const argv[]);

int inc_last_commit_ID();

bool check_file_directory_exists(char *filepath);

int commit_staged_file(int commit_ID, char *filepath);

int track_file(char *filepath);

int last_useful_line ( char * add ){
    FILE * ptr = fopen ( add, "r");
    int i = -1; int j = 0;
    char line[1000];
    while ( fgets ( line, sizeof (line), ptr) != NULL){ line[strlen( line ) - 1] = '\0';
        if ( strcmp ( line , "\n") && strcmp ( line , "*")){
            i = j;
        } j++;
    } fclose ( ptr );
    return i;
}

bool is_tracked(char *filepath);

int create_commit_file(int commit_ID, char *message);

int find_file_last_commit(char* filepath);

void tree_for_commit(const char *dirPath,  char * address) { 
    DIR *dir; 
    struct dirent *entry; 
    if (!(dir = opendir(dirPath))) { 
        perror("opendir"); 
        return; 
    } 
    while ((entry = readdir(dir)) != NULL) { 
        char path[1024]; 
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name); 
        if (entry->d_type == DT_DIR) { 
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) { 
                tree(path, address); 
            } 
        } else if ( strcmp(entry->d_name, "details") != 0 && strcmp(entry->d_name, "deleting") != 0 && strcmp(entry->d_name, "staging") != 0 ) {
            char name[1000] = "C:";
            char name2[1000]; strcpy ( name2, entry->d_name);
            change_word(name2, '^', '\\');
            strcat(name, name2);
            if ( search_for_line ( name , address ) == -1 ) {  add_to_file(name, address);}
            }
        } 
    closedir(dir); 
}

int run_checkout(int argc, char *const argv[]);

int find_file_last_change_before_commit(char *filepath, int commit_ID);

int checkout_file(char *filepath, int commit_ID);

int run_init(int argc, char * const argv[]) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
                exists = true;
        } 
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;

        // change cwd to parent
        if (strlen(tmp_cwd) > 4 && !exists) {
            if (chdir("..") != 0) return 1;
        }

    } while (strlen(tmp_cwd) > 4 && !exists);
    // return to the initial cwd
    if (chdir(cwd) != 0) return 1;
        
    if (!exists) {
        char add[1000] = "C:/Users/asus/Desktop/Project/global.txt";
        add_to_file(cwd, add);
        if (mkdir(".neogit") != 0) return 1;
        FILE *file;
        // create commits folder
        if (mkdir(".neogit/commits") != 0) return 1;
        if (mkdir(".neogit/stage_area") != 0) return 1;
        // create files folder
        if (mkdir(".neogit/files") != 0) return 1;

        file = fopen(".neogit/staging", "w");
        fclose(file);

        file = fopen(".neogit/tracks", "w");
        fclose(file);
        create_configs(" ", " ", 0);
        file = fopen(".neogit/alias", "w");
        fclose(file);
        file = fopen(".neogit/relstaging", "w");
        fclose(file);
        file = fopen(".neogit/branch", "w");
        fprintf(file, "master\n"); fprintf(file, "0\n");
        fclose(file);
        file = fopen(".neogit/deleting", "w");
        fclose(file);
        file  = fopen (".neogit/tags", "w");
        fclose (file);
        file = fopen (".neogit/hooks",  "w");
        fprintf (file , "%s\n-\n" , "file-size-check");


        fclose(file);

        perror ("neogit created succsefully");
    } else {
        perror("neogit repository has already initialized");
    }
    return 0;
}

int run_precom ( int argc , char * const argv[], int flag){
    if ( argc == 3 && strcmp ( argv[2] , "-u") == 0){
        flag = 2; argc = 2;
    }
    char cwd[1000]; getcwd ( cwd, sizeof ( cwd ) );
    find_name ( ".neogit" );
    char line [1000];
    int commit_check = 1;
    if ( argc == 4 && strcmp ( argv[2], "hooks" ) == 0 && strcmp ( argv[3] , "list" ) == 0 ){
        for ( int i = 0 ; i < number_of_lines ( ".neogit/hooks") ; i = i + 2){
            printf("%s\n", out_line ( i , ".neogit/hooks", line));
        }
    } else if ( argc == 4 && strcmp ( argv[3], "hooks" ) == 0 && strcmp ( argv[2] , "applied" ) == 0 ){
        for ( int i = 1 ; i < number_of_lines ( ".neogit/hooks") ; i = i + 2){
            if ( strcmp ( out_line ( i , ".neogit/hooks", line) , "-"))
            printf("%s\n", out_line ( i-1 , ".neogit/hooks", line));
        }
    } else if ( argc == 5 && strcmp( argv[3], "hook" ) == 0 && ( strcmp ( argv[2] , "add" ) == 0 || strcmp ( argv[2] , "remove") == 0)){
        char pos[1000] = "-";
        if ( strcmp ( argv[2] , "add" ) == 0 )  strcpy ( pos , "+" );
            
        int x = search_for_line ( argv[4], ".neogit/hooks" );
        if ( x != -1 )copy_file ( x + 1 , ".neogit/hooks" , ".neogit/hooks2" , pos );
        else { printf ("hook not found");}

    } else if ( argc == 2){
        DIR * dir = opendir ( ".neogit\\stage_area" );
        struct dirent * entry ;
        while ( (entry = readdir ( dir )) != NULL){
            char path [1024];
            if ( strcmp (entry->d_name , ".") && strcmp (entry->d_name , "..") ){
                char * name = strrchr ( entry->d_name , '^');
                name = name + 1 ;
                change_color ( 'b' );
                if ( flag ) printf("%s\n", name);
                snprintf ( path , sizeof ( path ) , ".neogit\\stage_area\\%s", entry->d_name );
                char hook_id[1000];
                char hook_check[1000];
                for ( int i = 0 ; i < number_of_lines ( ".neogit/hooks") ; i = i + 2){
                    out_line ( i , ".neogit/hooks" , hook_id ) ;
                    out_line ( i + 1 , ".neogit/hooks" , hook_check );
                    if ( strcmp ( hook_check , "-") ){
                        commit_check = commit_check * checker ( hook_id , path , flag);
                    }

                }

            }
        } reset_color();
    } else { 
        printf ( "something went wrong !");
    }
    chdir ( cwd );
    return commit_check ; 
}

int create_configs(char *username, char *email, int halat) {
    char entry[1000];
    strcpy(entry, email);
    if ( halat == 0 ) strcpy(entry, username);
    FILE *file = fopen(".neogit/config", "r");
    if (file == NULL){
        fclose(file); file = fopen(".neogit/config", "w");
        fprintf(file, "%s\n", username);           // username
        fprintf(file, "%s\n", email);              // email 
        fprintf(file, "%d\n", 0);                  // last_id
        fprintf(file, "%d\n", 0);                  // alan_id
        fprintf(file, "%s\n", "master");          // shakhe-branch
        fclose(file);
    } else{ fclose(file);
        copy_file(halat, ".neogit/config", ".neogit/confayakon", entry);
    }

    return 0;
}

int run_add(int argc, char *const argv[]) { 
    int x = 2;
    if (strcmp( argv[2] , "-f" ) == 0){
        x = 3;}  
    // TODO: handle command in non-root directories 
    if (argc < 3) { 
        perror("please specify a file"); 
        return 1; } 
    else if ( strcmp(argv[2], "-undo") || strcmp(argv[1], "reset")) { 
        for ( x ; x < argc ; x++){
            char cwd[1000]; 
            getcwd(cwd, sizeof(cwd)); 
            int flag2 = !(argv[x][1] == ':' && argv[x][0] == 'C');
            char tmpcwd[1000] = "";
            strcpy(tmpcwd, cwd); 
            if ( flag2 ){ 
                strcat(cwd, "\\"); 
                strcat(cwd, argv[x]); 
            }
            else {strcpy(cwd, argv[x]);}
            char *tmp; 
            int flag = 0;
            if ( is_file(cwd) != -1){
                char command[1000];
                if ( is_file ( cwd ) == 1 ){ tmp  = strrchr( cwd, '\\'); *tmp = '\0'; chdir ( cwd );
                    getcwd(cwd, sizeof(cwd));
                    snprintf(cwd, sizeof(cwd), "%s\\%s",cwd, tmp + 1);
                    find_name(".neogit");
                    add_to_file(cwd, ".neogit/relstaging");
                    add_to_file(cwd, ".neogit/tracks");
                    char line[1000];
                } else { chdir(cwd);
                    getcwd(cwd, sizeof(cwd));
                    find_name(".neogit");
                    tree(cwd, ".neogit/relstaging");
                    add_to_file ( cwd , ".neogit/tracks");
                    tree_dir_include(cwd, ".neogit/tracks");
                }
                FILE * ptr_rel = fopen( ".neogit/relstaging", "r");
                char entry[1000];
                while ( fgets( entry, sizeof( entry), ptr_rel) != NULL){ entry[strlen(entry) - 1] = '\0';
                    char new_name[1000]; strcpy(new_name,entry);change_word(new_name, '\\', '^');char*tmp = strrchr( entry, '\\'); strcpy( new_name, new_name+ 2);
                    chdir(".neogit\\stage_area");
                    remove(new_name);
                    if ( strcmp ( argv[1], "add") == 0){
                        snprintf(command, sizeof(command), "xcopy /Y %s", entry);
                        system(command);
                        rename(tmp + 1, new_name);
                    }
                    find_name(".neogit");
                    FILE * ptr = fopen( ".neogit/staging", "r");
                    char line[1000]; int lines[100];
                    int i = 0;
                    while ( fgets( line, sizeof(line), ptr) != NULL) { line[strlen(line) - 1] = '\0';
                        if ( strcmp ( line, entry) == 0){
                            break;
                        }i++;
                    } fclose(ptr);
                    delete_line(".neogit/staging",i + 1);



                    if ( strcmp ( argv[1], "add" ) == 0 ) {add_to_file(entry, ".neogit/staging"); }
                    if ( strcmp ( argv[1], "add" ) == 0 ) {
                        int x = search_for_line(entry , ".neogit/deleting");
                        while ( x != -1 ){
                            delete_line ( ".neogit/deleting", x + 1);
                            x = search_for_line(entry , ".neogit/deleting");
                        }
                    }
            
                } fclose(ptr_rel);
                remove(".neogit/relstaging");
                FILE * del = fopen(".neogit/relstaging", "w");
                fclose(del);

                chdir(tmpcwd);
            } else {
                char new_cwd[1000]; strcpy( new_cwd, cwd);
                change_word(new_cwd, '\\', '^');
                strcpy ( new_cwd, new_cwd + 2);

                find_name(".neogit");

                char branch_name[1000]; out_line(4, ".neogit/config", branch_name);
                char * id = get_last_branch_commit(branch_name);
                struct dirent *entry;
                char address[1000]; snprintf( address, sizeof(address), ".neogit\\commits\\%s", id);
                DIR *dir = opendir(address);
                int flag = 0;
                if (dir != NULL) {
                    while ((entry = readdir(dir)) != NULL) {
                        char name[1000]; strcpy( name , entry->d_name);
                        char tmp[1000]; strcpy( tmp , new_cwd);
                        if ( strcmp( entry->d_name, ".") && strcmp ( entry->d_name, "..") && fins(tmp, entry->d_name, "^")){
                            flag = 1;
                            change_word(name, '^', '\\');
                            char to[1000] = "C:"; strcat(to, name);
                            if (search_for_line(to, ".neogit/staging") != -1) delete_line(".neogit/staging", search_for_line(to, ".neogit/staging") + 1);
         /*mishe nabashe */ if (search_for_line(to, ".neogit/deleting") != -1) delete_line(".neogit/deleting", search_for_line(to, ".neogit/deleting") + 1); 
                            if (strcmp (argv[1], "reset")) add_to_file(to, ".neogit/staging");
                            if (strcmp (argv[1], "reset")) add_to_file(to, ".neogit/deleting");
                        }
                    } 
                    closedir(dir);





                }
                if (flag == 0){
                    char string [100];
                    strcpy( string , argv[x]); strcat ( string, " ");
                    perror(string);
                } else {
                    add_to_file("*", ".neogit/deleting");
                }
                chdir(tmpcwd);
            }
        }if ( strcmp ( argv[1], "add" ) == 0 ){ add_to_file("*", ".neogit/staging");}
    } else if (argc == 3 ){ 
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        find_name( ".neogit" );
        int n = number_of_lines ( ".neogit/staging");
        char line[1000];
        int i = n - 1;
        for ( i ; i >= 0 ; i--){
            out_line( i, ".neogit/staging", line);
            if (strcmp ( line , "*") != 0) break;
        }
        int flag = 0;
        for ( int j = i ; j >= 0 ; j--){
            char command[1000] = "neogit reset ";
            out_line( j, ".neogit/staging", line);
            if (strcmp ( line , "*") == 0) break;
            flag = flag + search_for_line (line, ".neogit/deleting") + 1;
            strcat(command, line);
            system(command);
        } 
        if ( flag ) {
            n = number_of_lines ( ".neogit/deleting");
            i = n - 1;
            for ( i ; i >= 0 ; i--){
                out_line( i, ".neogit/deleting", line);
                if (strcmp ( line , "*") != 0) break;
            }
            for ( int j = i ; j >= 0 ; j--){
                out_line( j, ".neogit/deleting", line);
                if (strcmp ( line , "*") == 0) break;
                delete_line(".neogit/deleting", j + 1);
            }
        }



        chdir(cwd);
    }  else{
        perror("sdfsdf");
    }  
}

int run_commit(int argc, char * const argv[]) {
    char cwd[1000]; getcwd( cwd, sizeof(cwd));
    find_name(".neogit");
    if ( argc == 2){
        perror("incorrect form");
    } else if ( strcmp ( argv[2], "-m") != 0){
        perror("incorrect input");
    } else if ( argc > 4){
        perror("you should write your message with "" ");
    } else if ( argc == 3) {
        perror ("enter your commit message");
    } else if ( strlen ( argv[3]) > 72){
        perror("message out of size");
    } else if (number_of_files(".neogit\\stage_area") == 0 && last_useful_line(".neogit/deleting") == -1 ){   //seems that problem have solved
        perror("you have not add anything");
    } else {
        if ( run_precom ( 2, argv , 0) == 0) { printf ("you already have FAILED files in stage");  return 0 ; }
        char current_id[1000];
        char id[1000];  get_last_commit_id(id); char branch_name[1000];
        char user_name[1000]; char user_email[1000];
        out_line(0, ".neogit/config", user_name); 
        out_line(1, ".neogit/config", user_email);
        out_line(3, ".neogit/config", current_id);
        out_line(4, ".neogit/config", branch_name);
        char last_branch_commit [1000]; 
        strcpy ( last_branch_commit, get_last_branch_commit(branch_name)) ;

        if ( strcmp ( last_branch_commit, current_id ) ) {perror ("you are not at head to commit !"); return 0;} 

        time_t date;
        time( &date );

        chdir(".neogit/commits"); 
        char next_id[1000];
        sprintf(next_id, "%d", number_out(id) + 1);
        char command[1000] = "xcopy ";
        snprintf(command, sizeof(command), "%s/E /Y %s %s" ,command ,last_branch_commit, next_id);

        mkdir(next_id);

        // managing files

        if ( strcmp ( id, "0")){
            printf("%s\n", command);
            system(command);
        }
        char command2[1000] = "xcopy ";
        char command3[1000] = "xcopy ";
        char command4[1000] = "xcopy ";
        snprintf(command2, sizeof(command2), "%s/E /Y ..\\stage_area %s", command2, next_id);
        snprintf(command3, sizeof(command3), "%s/Y ..\\staging %s", command3, next_id);
        snprintf(command4, sizeof(command4), "%s/Y ..\\deleting %s", command4, next_id);
        system(command2);
        system(command3);
        system(command4);

        chdir(next_id);

        // deleting;

        DIR * dir;
        dir = opendir(".");
        struct dirent * entry;
        char tmp[1000];
        while ( (entry = readdir(dir)) != NULL){
            char name[1000] = "C:";
            strcpy ( tmp , entry->d_name);
            change_word ( tmp , '^', '\\');
            strcat ( name , tmp );
            if ( search_for_line ( name , "deleting") != -1 ){
                remove( entry->d_name );
            }
        }
        debug(last_branch_commit);
        // building details
        FILE * ptr = fopen("details", "w");
        fprintf(ptr, "%s\n", next_id);                      // 0 id
        fprintf(ptr, "%s\n", branch_name);                  // 1 branch 
        fprintf(ptr, "%s\n", last_branch_commit);           // 2 head
        fprintf(ptr, "%s\n", user_name);                    // 3 user_name
        fprintf(ptr, "%s\n", user_email);                   // 4 user_email
        fprintf(ptr, "%s",   ctime(&date));                 // 5 date
        fprintf(ptr, "%s\n", argv[3]);                      // 6 commit_message
        fclose(ptr);

        // correcting other files ( stages , config , branch )

        find_name(".neogit");
        copy_file(2, ".neogit/config", ".neogit/spars24d", next_id);
        copy_file(3, ".neogit/config", ".neogit/spars24d", next_id);

        ptr = fopen( ".neogit/branch", "r");
        char line[1000]; int i = 0;
        while ( fgets( line , sizeof(line), ptr) != NULL ){ line[strlen( line ) - 1] = '\0';
            if ( strcmp ( line, branch_name ) == 0){
                break;
            }i++;
        }fclose(ptr);
        copy_file(i + 1, ".neogit/branch", ".neogit/saprsdf235", next_id);

        remove_directory(".neogit\\stage_area"); mkdir(".neogit\\stage_area");
        remove("staging"); ptr = fopen(".neogit/staging", "w"); fclose(ptr);
        remove("deleting"); ptr = fopen ( ".neogit/deleting", "w"); fclose(ptr);
    } chdir(cwd);
}
// returns new commit_ID

void space_breaker( char * word ){ //////////////  bug 

    char tmp_word[1000] = "";
    char * token = strtok(word, " ");

    while ( token != NULL){
        strcat ( tmp_word , token);
        token = strtok(NULL ," ");
    }
    strcpy ( word , tmp_word );
}

int run_log( int argc , char * const argv[]){
    char cwd[1000]; getcwd(cwd, sizeof(cwd));
    find_name(".neogit");
    int to_where = 0 ;
    char id[1000]; get_last_commit_id(id); int n = number_out(id);
    if (argc > 2 && strcmp ( argv[2], "-n") == 0){
        if (argc == 3) {perror("please enter number of logs");
            chdir ( cwd );
            return 0;
        } else {
            int y = number_out(argv[3]);
            to_where = n - y;
            if ( to_where < 1) {perror("The number is more than number of commits"); chdir ( cwd ); return 1;}
        }
    } if (argc == 2 || ((strcmp ( argv[2], "-n") == 0) && argc == 4 )){
        chdir( ".neogit\\commits");
        for ( int i = n ; i > to_where ; i--){
            char commit_name[1000];
            sprintf(commit_name, "%d", i);
            chdir(commit_name);
            char line[1000];
            out_log(commit_name);
        } chdir ( cwd ); return 0;
    } else if (argc >= 4){
        if (strcmp( argv[2], "-branch") == 0 && argc == 4){
            chdir( ".neogit\\commits");
            for ( int i = n ; i > to_where ; i--){
                char commit_name[1000];
                sprintf(commit_name, "%d", i);
                chdir(commit_name);
                char line[1000]; out_line(1, "details", line);
                if ( strcmp ( argv[3],line ) == 0)
                    out_log(commit_name);
                else {chdir("..");}
            } chdir ( cwd ); return 0;   

        } else if (strcmp( argv[2], "-author") == 0 && argc == 4){
            chdir( ".neogit\\commits");
            for ( int i = n ; i > to_where ; i--){
                char commit_name[1000];
                sprintf(commit_name, "%d", i);
                chdir(commit_name);
                char line[1000]; out_line(3, "details", line);
                if ( strcmp ( argv[3],line ) == 0)
                    out_log(commit_name);
                else {chdir("..");}
            } chdir ( cwd ); return 0;   

        } else if (strcmp( argv[2], "-search") == 0){
            chdir( ".neogit\\commits");
            for ( int i = n ; i > to_where ; i--){
                int flag = 1;
                char commit_name[1000];
                sprintf(commit_name, "%d", i);
                chdir(commit_name);
                char line[1000]; out_line(6, "details", line);
                for ( int j = 3 ; j < argc ; j++){
                    if ( strstr(line,argv[j])) {
                        out_log(commit_name);
                        flag = 0;
                        break;
                    }
                } if (flag){ chdir("..");}
            } chdir ( cwd ); return 0; 
        } else if (strcmp( argv[2], "-since") == 0 || strcmp ( argv[2], "-before")){
            int halat = strcmp( argv[2], "-since");
            chdir( ".neogit\\commits");
            for ( int i = n ; i > to_where ; i--){
                int flag = 1;
                char commit_name[1000];
                sprintf(commit_name, "%d", i);
                chdir(commit_name);
                char line[1000]; out_line(5, "details", line); 
                double diffrence = calculateTimeDifference(line, argv[3]);
                if ( halat && diffrence >= 0){ // before
                    out_log(commit_name);
                    flag = 0;
                } else if ( halat == 0 && diffrence <= 0){
                    out_log(commit_name);
                    flag = 0;
                } if (flag){ chdir("..");}
            } chdir ( cwd ); return 0;
        } else {
            printf("to many arguments");
            return 1;
        }
    } else {
        chdir ( cwd );
        perror("Invali command");
        return 1;
    }
}

int run_branch( int argc, char * const argv[]){
    char cwd [1000]; getcwd(cwd, sizeof(cwd));
    if ( argc  < 4 ){
        find_name(".neogit");
        if ( argc == 2 ){
            char line[1000];
            for ( int i = 0 ; i < number_of_lines(".neogit/branch"); i = i + 2){
                out_line(i, ".neogit/branch", line);
                printf("%s\n", line);
            } 
        } else if ( argc == 3 ){
            char id[1000];
            out_line (3, ".neogit/config", id);
            add_to_file(argv[2], ".neogit/branch");
            add_to_file(id, ".neogit/branch");
        } chdir(cwd);
    return 0;}
    else {
        perror("incorrect input");
        return 1;
    }
}

int run_status( int argc, char * const argv[], int print){
    if ( argc == 2 || strcmp ( argv[1], "checkout") == 0){
        char cwd[1000]; getcwd(cwd, sizeof(cwd));

        find_name(".neogit");
        char branch_name[1000];
        out_line(4, ".neogit/config", branch_name);

        char neo_add[1000];
        getcwd(neo_add, sizeof(neo_add));


        char * id = get_last_branch_commit(branch_name);
        out_line ( 3, ".neogit/config", id);


        char directory_address[1000]; 
        chdir( ".neogit\\commits"); chdir(id);
        getcwd( directory_address, sizeof(directory_address));
        find_name(".neogit");
        tree(neo_add, ".neogit/status");
        tree_for_commit ( directory_address , ".neogit/status");

        int flag = 0;
        char line[1000];
        char X = '-'; char Y = 'M';
        for ( int i = 0 ; i < number_of_lines ( ".neogit/status" ) ; i++ ){

            X = '-' ; Y = 'M';
            out_line ( i , ".neogit/status", line);

            if ( show_status ( line , directory_address) == -1)  Y = 'D'; 
            if ( show_status ( line , directory_address) == -2)  Y = 'A';
            if ( show_status ( line , directory_address) != 1 && ( print || search_for_line ( line , ".neogit/tracks") != -1)) {
                flag = 100;
                int x = search_for_line ( line, ".neogit/staging");
                if ( x != -1 ) X = '+';
                char * name = strrchr ( line, '\\' );
                if ( print ) printf ("%s %c%c\n", name + 1 ,X, Y);
            }
        }    remove( ".neogit/status"); FILE * ptr = fopen ( ".neogit/status", "w");
        fclose ( ptr ); 

        chdir(cwd);



        if ( print == 0){
            return flag;
        }
        return 0;

    } else if ( strcmp ( argv[1] , "checkout" ) ){
        perror( " invalid command ");
        return 1;
    }
}

int is_number(char * entry){
    for ( int i = 0 ; i < strlen( entry ) ; i++){
        if ( entry[i] -'0' < 0 || entry[i] - '0' > 9) return 0;
    } return 1;
}

int run_checkout( int argc, char * const argv[]){ ////////////////////////////// careful to commit in head ////////////////////////
    if ( argc == 2 ){
        printf("enter commit-id or branch name");
        return 0;
    } else if ( argc == 3 ){
        int z = run_status ( argc, argv, 0);
        if ( z == 100 && strcmp(argv[2], "HEAD")){
            printf("you have uncommited changes");
            return 0;
        } 

        char cwd[1000]; getcwd ( cwd, sizeof ( cwd ));
        find_name(".neogit");
        char neo_add[1000]; getcwd( neo_add , sizeof ( neo_add));
        char id[1000];
        char branch_name[1000];
        int x = search_for_line ( argv[2] , ".neogit/branch" );

        // finding branch name and commit id
        if ( is_number ( argv[2] ) ) {

            strcpy ( id , argv[2]);
            chdir ( ".neogit\\commits");
            chdir ( id );
            out_line( 1, "details", branch_name);
            find_name( ".neogit" );

        } else if ( strncmp ( argv[2], "HEAD" , 4) == 0) {
            out_line ( 4 , ".neogit/config", branch_name);
            strcpy ( id , get_last_branch_commit ( branch_name ));
            if (strlen ( argv [2]) > 5) {
                char n[1000]; 
                chdir ( ".neogit\\commits");
                strcpy ( n , argv[2] + 5); int m = number_out( n );

                for ( int i = 0 ; i < m ; i ++ ){
                    chdir ( id );
                    out_line ( 2, "details", id);
                    chdir ( ".." );
                }
                find_name (".neogit");
            } else if ( strlen ( argv[2]) == 5 ){
                perror( "enter number" ); return 0;
            }
        
        
        } else {
            strcpy ( branch_name , argv[2] );
            out_line ( x + 1 , ".neogit/branch", id);
        }

        char directory_address[1000] = ".neogit\\commits\\";
        strcat ( directory_address , id );
        // deleting files                   (                                         untracked files remains                                           ) 

        char tracks_address[1000]; strcpy ( tracks_address , neo_add );
        strcat ( tracks_address , "\\.neogit\\tracks");

        remove_tree ( neo_add , tracks_address );
        printf("!");
        // making tree
        debug ( directory_address );
        debug ( neo_add );
        tree_maker (directory_address, neo_add);
        // correcting details

        copy_file (3, ".neogit/config", ".neogit/consdfasdgsdin", id );
        copy_file (4, ".neogit/config", ".neogit/sadfsfsdafsfeg", branch_name);

        chdir( cwd );



        return 0;
        
    }
}

int run_tag ( int argc , char * const argv[]){

    time_t date;
    time( &date );
    char cwd[1000]; getcwd ( cwd, sizeof ( cwd ));
    find_name ( ".neogit" );

    char user_name[1000]; out_line ( 0 , ".neogit/config" , user_name );
    char user_email[1000]; out_line ( 1 , ".neogit/config" , user_email );
    char tag_name[1000];
    char message[1000] = "NO MESSAGE";
    char commit_id[1000]; out_line ( 3 , ".neogit/config" , commit_id );
    char over_write[10] = "-";

    if ( argc == 2){ 
        FILE * ptr = fopen ( ".neogit/tags" , "r");
        char ** names = ( char ** ) malloc ( sizeof ( char * ) * 1000); char line[1000];
        for ( int i = 0 ; i < number_of_lines(".neogit/tags") ; i = i + 5){
            *(names + i/5) = ( char * ) malloc ( sizeof ( char ) * 1000);
            out_line ( i, ".neogit/tags" , line);
            strcpy ( *( names + i/5) , line );
        }

        for ( int i = 0 ; i < number_of_lines(".neogit/tags") ; i = i + 5){
            for ( int j = i ; j < number_of_lines(".neogit/tags") ; j = j + 5){
                if ( strcmp ( * (names + i/5) , * ( names + j/5) ) > 0){
                    char * tmp = ( char * ) malloc ( sizeof ( char * ) * 1000 );
                    tmp = * ( names + j/5 );
                    * ( names + j/5 ) = * ( names + i/5 );
                    * ( names + i/5 ) = ( tmp );
                }
            }
        }
        for ( int i = 0 ; i < number_of_lines(".neogit/tags") ; i = i + 5){
            printf("%s\n" ,  * ( names + i/5 ) );
        }
        chdir ( cwd ); 
        return 0;
    } else if ( strcmp ( argv[2] , "-a") == 0){
        int flag = 0;
        for ( int i = 2 ; i < argc ; i ++ ){
            if ( strcmp ( argv[i] , "-a" ) == 0 && ( i + 1 < argc)){ flag = 1;
                strcpy ( tag_name , argv[i + 1] );
            } if ( strcmp ( argv[i] , "-m" ) == 0 && ( i + 1 < argc)){ flag = 1;
                strcpy ( message , argv[i + 1] );
            } if ( strcmp ( argv[i] , "-c" ) == 0 && ( i + 1 < argc)){ flag = 1;
                strcpy ( commit_id , argv[i + 1] );
            } if ( strcmp ( argv[i] , "-f" ) == 0 ) { flag = 1 ; strcpy (over_write , "+"); }
        }
        if ( flag == 0 ){
            printf ( "Incorrect input format");
            return 0;
        }
        FILE * ptr = fopen ( ".neogit/tags" , "r") ; char line[1000];
        for ( int i = 0 ; i < number_of_lines ( ".neogit/tags" ) ; i = i + 5){
            if ( strcmp ( out_line ( i , ".neogit/tags" , line ) , tag_name ) == 0){

                if ( strcmp ( over_write , "-" ) == 0 ){
                    printf ( "this tag already exists" ); return 0;
                } fclose ( ptr );
                for ( int j = i + 1 ; j < i + 6 ; j++){
                    delete_line ( ".neogit/tags" , i + 1 );
                } break ;
            }
        }
        ptr = fopen ( ".neogit/tags" , "a");
        fprintf ( ptr, "%s\n", tag_name );                           // tag
        fprintf ( ptr, "%s\n", commit_id );                          // id
        fprintf ( ptr, "%s <%s>\n", user_name , user_email );        // info
        fprintf ( ptr, "%s", ctime(&date) );                       // date
        fprintf ( ptr, "%s\n" , message );                           // message
        fclose ( ptr );
        chdir ( cwd ); 
        return 0 ;
    } else if ( argc == 4 && strcmp ( argv[2], "show") == 0){
        int x = search_for_line ( argv[3] , ".neogit/tags" );
        char line[1000];
        printf("tag %s\n", out_line(x, ".neogit/tags", line));
        printf("commit %s\n", out_line(x + 1, ".neogit/tags", line));
        printf("Author: %s\n", out_line(x + 2, ".neogit/tags", line));
        printf("Date: %s\n", out_line(x + 3, ".neogit/tags", line));
        if ( strcmp ( out_line(x + 4, ".neogit/tags", line) , "NO MESSAGE") )
        printf("Message: %s\n", out_line(x + 4, ".neogit/tags", line));
        chdir ( cwd );
    }
}

int run_diff ( int argc , char * const argv[]){                        /////////////////////////// not compelet ////////////////
    char cwd[1000]; getcwd ( cwd , sizeof ( cwd ) );
    if ( argc < 5 || strcmp ( argv[2] , "-f" ) || ( argc != 5 && argc != 9 )){
        printf("Invalid input"); return 0;
    } else if ( argc == 5){
        char file1_add[1000]; strcpy ( file1_add , argv [3] );
        char file2_add[1000]; strcpy ( file2_add , argv [4] );
    } else {

    }
}

int run_revert ( int argc, char * const argv[]){

    char cwd[1000]; getcwd ( cwd, sizeof ( cwd ));
    find_name(".neogit");
    char neo_add[1000]; getcwd( neo_add , sizeof ( neo_add));

    char tracks_address[1000]; strcpy ( tracks_address , neo_add );
    strcat ( tracks_address , "\\.neogit\\tracks");

    if ( argc <= 4 && strcmp ( argv[2] , "-n") == 0 ) {

        char id[1000]; get_last_commit_id(id);
        if ( argc == 4) strcpy ( id , argv[3] );

        debug ( id );
        char directory_address[1000] = ".neogit\\commits\\";
        strcat ( directory_address , id );

        remove_tree ( neo_add , tracks_address );

        tree_maker (directory_address, neo_add);

    } else {
        char message[1000];
        if ( argc == 5 ){
            if (strcmp ( argv[2] , "-m" ) == 0 ) strcpy ( message , argv[3] );
            else {printf ( "Invalid input"); return 0;}
        }
        if ( argc == 3 || argc == 5){
            char id[1000];
            if ( strncmp ( "HEAD-" ,  argv[argc - 1], 5) == 0 ){
                strcpy ( id , argv[argc - 1] + 5);
            } else {
                strcpy ( id , argv[argc - 1] );
            }
                int n_id  = number_out ( id ) + 1;
                char new_id[1000]; sprintf ( new_id , "%d" , n_id );

                char directory_address[1000] = ".neogit\\commits\\"; strcat ( directory_address , id );
                char new_directory_address[1000] = ".neogit\\commits\\"; strcat ( new_directory_address , get_last_commit_id(new_id) );

                chdir ( directory_address );
                if ( argc == 3 )  out_line ( 6 , "details" , message );
                find_name ( ".neogit" );

                remove_tree ( neo_add , tracks_address );

                tree_maker ( directory_address , neo_add );

                tree(neo_add, ".neogit/status");
                tree_for_commit ( new_directory_address , ".neogit/status");

                for ( int i = 0 ; i < number_of_lines ( ".neogit/status") ; i++) {
                    char command[1000] = "neogit add ";
                    char line [1000]; out_line ( i , ".neogit/status" , line );
                    strcat ( command , line );
                    debug ( command );
                    system ( command );
                }
                char command[1000] = "neogit commit -m ";
                strcat ( command , message );
                system ( command );
                remove( ".neogit/status"); FILE * ptr = fopen ( ".neogit/status", "w");
                fclose ( ptr );

                            

        } else {
            printf ( "Invalid input ");
        }
    } chdir ( cwd );
}

int run_grep ( int argc, char * const argv[] ){
    char cwd[1000]; getcwd ( cwd, sizeof ( cwd ));
    if ( argc >= 6 && strcmp ( argv[2] , "-f") == 0 && strcmp ( argv[4], "-p") == 0 ){
        if ( argc == 6 ){

        }
    }
} 



int inc_last_commit_ID() {
    FILE *file = fopen(".neogit/config", "r");
    if (file == NULL) return -1;
    
    FILE *tmp_file = fopen(".neogit/tmp_config", "w");
    if (tmp_file == NULL) return -1;

    int last_commit_ID;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "last_commit_ID", 14) == 0) {
            sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
            last_commit_ID++;
            fprintf(tmp_file, "last_commit_ID: %d\n", last_commit_ID);

        } else fprintf(tmp_file, "%s", line);
    }
    fclose(file);
    fclose(tmp_file);

    remove(".neogit/config");
    rename(".neogit/tmp_config", ".neogit/config");
    return last_commit_ID;
}

bool check_file_directory_exists(char *filepath) {
    DIR *dir = opendir(".neogit/files");
    struct dirent *entry;
    if (dir == NULL) {
        perror("Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, filepath) == 0) return true;
    }
    closedir(dir);

    return false;
}

int commit_staged_file(int commit_ID, char* filepath) {
    FILE *read_file, *write_file;
    char read_path[MAX_FILENAME_LENGTH];
    strcpy(read_path, filepath);
    char write_path[MAX_FILENAME_LENGTH];
    strcpy(write_path, ".neogit/files/");
    strcat(write_path, filepath);
    strcat(write_path, "/");
    char tmp[10];
    sprintf(tmp, "%d", commit_ID);
    strcat(write_path, tmp);

    read_file = fopen(read_path, "r");
    if (read_file == NULL) return 1;

    write_file = fopen(write_path, "w");
    if (write_file == NULL) return 1;

    char buffer;
    buffer = fgetc(read_file);
    while(buffer != EOF) {
        fputc(buffer, write_file);
        buffer = fgetc(read_file);
    }
    fclose(read_file);
    fclose(write_file);

    return 0;
}

int track_file(char *filepath) {
    if (is_tracked(filepath)) return 0;

    FILE *file = fopen(".neogit/tracks", "a");
    if (file == NULL) return 1;
    fprintf(file, "%s\n", filepath);
    return 0;
}

bool is_tracked(char *filepath) {
    FILE *file = fopen(".neogit/tracks", "r");
    if (file == NULL) return false;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
        
        if (strcmp(line, filepath) == 0) return true;

    }
    fclose(file); 

    return false;
}

int create_commit_file(int commit_ID, char *message) {
    char commit_filepath[MAX_FILENAME_LENGTH];
    strcpy(commit_filepath, ".neogit/commits/");
    char tmp[10];
    sprintf(tmp, "%d", commit_ID);
    strcat(commit_filepath, tmp);

    FILE *file = fopen(commit_filepath, "w");
    if (file == NULL) return 1;

    fprintf(file, "message: %s\n", message);
    fprintf(file, "files:\n");
    
    DIR *dir = opendir(".");
    struct dirent *entry;
    if (dir == NULL) {
        perror("Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && is_tracked(entry->d_name)) {
            int file_last_commit_ID = find_file_last_commit(entry->d_name);
            fprintf(file, "%s %d\n", entry->d_name, file_last_commit_ID);
        }
    }
    closedir(dir); 
    fclose(file);
    return 0;
}

int find_file_last_commit(char* filepath) {
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/files/");
    strcat(filepath_dir, filepath);

    int max = -1;
    
    DIR *dir = opendir(filepath_dir);
    struct dirent *entry;
    if (dir == NULL) return 1;

    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            int tmp = atoi(entry->d_name);
            max = max > tmp ? max: tmp;
        }
    }
    closedir(dir);

    return max;
}

int run_config(int argc, char * argv[]){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    int global = 0;
    int halat = -1;
    char entry[1000]; 
    char username[1000] = " "; char email[1000] = " ";
    if (strcmp ( argv[2], "-global") == 0){ global = 1;}

    if (strcmp(argv[global + 2], "user.name") == 0){ halat = 0;
        strcpy(username, argv[global + 3]);
    }
    else if (strcmp ( argv[global + 2], "user.email") == 0) {halat = 1;
        strcpy(email, argv[global + 3]);

    } else if ( strncmp ( argv[global + 2], "alias", 5) == 0 ){ halat = 2;
        strcpy(argv[global + 2], argv[global + 2] + 6);


        strcpy(entry, argv[global + 3]);
        for ( int i = global + 4 ; i < argc ; i++){
            strcat(entry, " "); 
            strcat(entry, argv[i]);
        }
    }



    if (global){
        char add[1000] = "C:\\Users\\asus\\Desktop\\Project\\global.txt";
        FILE * ptr = fopen(add, "r");
        char line[1000];
        while ( fgets(line, sizeof(line), ptr) != NULL){
            line[strlen(line) - 1] = '\0';
            chdir(line);
            find_name(".neogit");
            if ( halat != 2 ) create_configs(username, email, halat);
            else { add_to_file(argv[global + 2], ".neogit/alias") ; add_to_file(entry, ".neogit/alias") ; }
            chdir(cwd);
        } fclose(ptr);


    } else if ( !global ) {
        find_name(".neogit");
        if (halat != 2 ) create_configs(username, email, halat);
        else {add_to_file(argv[global + 2], ".neogit/alias") ; add_to_file(entry, ".neogit/alias") ;}
        chdir(cwd);
    } 
}

int find_file_last_change_before_commit(char *filepath, int commit_ID) {
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/files/");
    strcat(filepath_dir, filepath);

    int max = -1;
    
    DIR *dir = opendir(filepath_dir);
    struct dirent *entry;
    if (dir == NULL) return 1;

    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            int tmp = atoi(entry->d_name);
            if (tmp > max && tmp <= commit_ID) {
                max = tmp;
            }
        }
    }
    closedir(dir);

    return max;
}

int checkout_file(char *filepath, int commit_ID) {
    char src_file[MAX_FILENAME_LENGTH];
    strcpy(src_file, ".neogit/files/");
    strcat(src_file, filepath);
    char tmp[10];
    sprintf(tmp, "/%d", commit_ID);
    strcat(src_file, tmp);

    FILE *read_file = fopen(src_file, "r");
    if (read_file == NULL) return 1;
    FILE *write_file = fopen(filepath, "w");
    if (write_file == NULL) return 1;
    
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), read_file) != NULL) {
        fprintf(write_file, "%s", line);
    }
    
    fclose(read_file);
    fclose(write_file);

    return 0;
}

int run_alias ( int argc, char * argv[]){
    find_name(".neogit");
    FILE * ptr = fopen(".neogit/alias", "r");
    char line[1000];
    char entry[1000];
    int i = 0 ;
    while ( fgets( line, sizeof(line), ptr) != NULL){ line[strlen(line) - 1] = '\0';
        if ( i%2 == 0){
            if (strcmp ( line , argv[1] ) == 0){
                strcpy( entry, out_line(i + 1, ".neogit/alias", entry));
                if ( entry [0] == '"'){ strcmp(entry, entry + 1); entry[strlen(entry) - 1] ; 
                }
                system(entry);
                return 0;
            }
        }i++;
    }
    printf("invalid command");
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    if (strcmp(argv[1], "init") == 0) {
        return run_init(argc, argv);
    } else if (strcmp(argv[1], "add") == 0 || strcmp ( argv[1], "reset") == 0) { // yeki munde
        return run_add(argc, argv);
    } else if (strcmp(argv[1], "commit") == 0) {
        return run_commit(argc, argv);
    } else if (strcmp(argv[1], "checkout") == 0) {
        return run_checkout(argc, argv);
    } else if ( strcmp(argv[1], "config") == 0) {
        return run_config(argc, argv);
    } else if ( strcmp ( argv[1], "commit") == 0){ // message mande
        return run_commit(argc, argv);
    } else if ( strcmp ( argv[1], "log") == 0){  // since mande
        return run_log(argc, argv);
    } else if ( strcmp ( argv[1], "branch") == 0){
        return run_branch(argc, argv);
    } else if ( strcmp ( argv[1], "checkout") == 0){
        return run_checkout( argc, argv);
    } else if ( strcmp ( argv[1], "status") == 0){
        return run_status( argc, argv, 1);
    } else if ( strcmp ( argv[1], "tag" ) == 0){
        return run_tag ( argc, argv );
    } else if ( strcmp ( argv[1], "diff") == 0){
        return run_diff ( argc, argv );
    } else if ( strcmp ( argv[1], "revert") == 0){
        return run_revert ( argc , argv );
    } else if ( strcmp ( argv[1], "pre-commit") == 0){
        return run_precom ( argc , argv , 1);
    } else if ( strcmp ( argv[1], "grep") == 0){

    }


    else if ( argc == 2 ) {   
        return run_alias(argc, argv);
    }
    else {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    return 0;
}