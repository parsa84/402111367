#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_FILENAME_LENGTH 1000
#define MAX_COMMIT_MESSAGE_LENGTH 2000                              
#define MAX_LINE_LENGTH 1000
#define MAX_MESSAGE_LENGTH 1000  
#define BUFFER_SIZE 1024                     
#define debug(x) printf("%s\n", x);

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
int min( int a, int b){
    if ( b < a) return b; return a;
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
    if (stat(path, &file_stat) == 0) 
    { 
        if (S_ISREG(file_stat.st_mode)) 
        { 
            return 1; 
        } 
        else if (S_ISDIR(file_stat.st_mode)) 
        { 
            return 0; 
        } 
        else 
        { 
            return -1; 
        } 
    } 
    else 
    { 
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
void add_to_file( char * entry, char * add){
    FILE *ptr = fopen(add, "a");
    fprintf(ptr, "%s\n", entry);
    fclose(ptr);
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

int run_commit(int argc, char * const argv[]);
int inc_last_commit_ID();
bool check_file_directory_exists(char *filepath);
int commit_staged_file(int commit_ID, char *filepath);
int track_file(char *filepath);
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

void print_command(int argc, char * const argv[]) {
    for (int i = 0; i < argc; i++) {
        fprintf(stdout, "%s ", argv[i]);
    }
    fprintf(stdout, "\n");
}
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
    } else {
        perror("neogit repository has already initialized");
    }
    return 0;
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
                    char line[1000];
                } else{ chdir(cwd);
                    getcwd(cwd, sizeof(cwd));
                    find_name(".neogit");
                    add_tree(cwd, ".neogit/relstaging", argv[1]);}
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
int add_to_staging(char *filepath) {
    FILE *file = fopen(".neogit/staging", "r");
    if (file == NULL) return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }

        if (strcmp(filepath, line) == 0) return 0;
    }
    fclose(file);
    
    file = fopen(".neogit/staging","a");
    if (file == NULL) return 1;

    fprintf(file, "%s\n", filepath);
    fclose(file);

    return 0;
}

int run_reset(int argc, char *const argv[]) {
    // TODO: handle command in non-root directories 
    if (argc < 3) {
        perror("please specify a file");
        return 1;
    }
    
    return remove_from_staging(argv[2]);
}

int remove_from_staging(char *filepath) {
    FILE *file = fopen(".neogit/staging", "r");
    if (file == NULL) return 1;
    
    FILE *tmp_file = fopen(".neogit/tmp_staging", "w");
    if (tmp_file == NULL) return 1;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }

        if (strcmp(filepath, line) != 0) fputs(line, tmp_file);
    }
    fclose(file);
    fclose(tmp_file);
    remove(".neogit/staging");
    rename(".neogit/tmp_staging", ".neogit/staging");
    return 0;
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
    } else if ( strlen ( argv[3]) > 72){
        perror("message out of size");
    } else if (number_of_files(".neogit\\stage_area") == 0 && 0){          ////////  problem / / / / // / 
        perror("you have not add anything");
    } else {
        char id[1000];  get_last_commit_id(id); char branch_name[1000];
        char user_name[1000]; char user_email[1000];
        out_line(0, ".neogit/config", user_name); 
        out_line(1, ".neogit/config", user_email);
        out_line(4, ".neogit/config", branch_name);
        char * last_branch_commit = ( char * ) malloc ( sizeof ( char ) * 1000);
        last_branch_commit = get_last_branch_commit(branch_name);

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
            debug(name);
            if ( search_for_line ( name , "deleting") != -1 ){
                remove( entry->d_name );
            }
        }

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
            get_last_commit_id(id);
            add_to_file(argv[2], ".neogit/branch");
            add_to_file(id, ".neogit/branch");
        } chdir(cwd);
    return 0;}
    else {
        perror("incorrect input");
        return 1;
    }
}
int run_checkout( int argc, char * const argv[]){

}
int run_status( int argc, char * const argv[]){
    if ( argc == 2){
        char cwd[1000]; getcwd(cwd, sizeof(cwd));

        find_name(".neogit");
        char branch_name[1000];
        out_line(4, ".neogit/config", branch_name);

        char neo_add[1000];
        getcwd(neo_add, sizeof(neo_add));


        char * id = get_last_branch_commit(branch_name);

        char directory_address[1000]; 
        chdir( ".neogit\\commits"); chdir(id);
        getcwd( directory_address, sizeof(directory_address));
        find_name(".neogit");
        tree(neo_add, ".neogit/status");
        tree_for_commit ( directory_address , ".neogit/status");

        char line[1000];
        char X = '-'; char Y = 'M';
        for ( int i = 0 ; i < number_of_lines ( ".neogit/status" ) ; i++ ){

            X = '-' ; Y = 'M';
            out_line ( i , ".neogit/status", line);

            if ( show_status ( line , directory_address) == -1)  Y = 'D';
            if ( show_status ( line , directory_address) == -2)  Y = 'A';
            if ( show_status ( line , directory_address) != 1 ) {
                int x = search_for_line ( line, ".neogit/staging");
                if ( x != -1 ) X = '+';
                char * name = strrchr ( line, '\\' );
                printf ("%s %c%c\n", name + 1 ,X, Y);
            }
        }    remove( ".neogit/status"); FILE * ptr = fopen ( ".neogit/status", "w");
        fclose ( ptr ); 

        chdir(cwd);
        return 0;





    } else {
        perror( " invalid command ");
        return 1;
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
    } else if (strcmp(argv[1], "reset") == 0) {
        return run_reset(argc, argv);
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
        return run_status( argc, argv);
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