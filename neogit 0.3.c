#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAME_LENGTH 1000
#define MAX_COMMIT_MESSAGE_LENGTH 2000
#define MAX_LINE_LENGTH 1000
#define MAX_MESSAGE_LENGTH 1000

#define debug(x) printf("%s", x);
int is_file(char *path) 
{ 
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
    return line2;
}
void add_to_file( char * entry, char * add){
    FILE *ptr = fopen(add, "a");
    fprintf(ptr, "%s\n", entry);
    fclose(ptr);
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
void print_command(int argc, char * const argv[]);
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
int run_init(int argc, char * const argv[]);
int create_configs(char *username, char *email, int halat);

int run_add(int argc, char * const argv[]);
int add_to_staging(char *filepath);

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
        if (mkdir(".neogit/stage area") != 0) return 1;
        // create files folder
        if (mkdir(".neogit/files") != 0) return 1;

        file = fopen(".neogit/staging", "w");
        fclose(file);

        file = fopen(".neogit/tracks", "w");
        fclose(file);
        create_configs(" ", " ", 0);
        file = fopen(".neogit/alias", "w");
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
        fprintf(file, "%s\n", username);    // username
        fprintf(file, "%s\n", email);          //email 
        fprintf(file, "%d\n", 0);     //last_id
        fprintf(file, "%d\n", 0);   //alan_id
        fprintf(file, " %s\n", "master");          //shakhe
        fclose(file);
    } else{ fclose(file);
        copy_file(halat, ".neogit/config", ".neogit/confayakon", entry);
    }

    return 0;
}

int run_add(int argc, char *const argv[]) {
    // TODO: handle command in non-root directories 
    if (argc < 3) {
        perror("please specify a file");
        return 1;
    } else if ( argc == 3){
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "\\");
        strcat(cwd, argv[2]);
        char * tmp;
        int flag = 0;
        if ( is_file (cwd) != 0){
            tmp = strrchr(cwd, '\\');
            *tmp = '\0';
            flag = 1;

        } chdir(cwd);
        getcwd(cwd, sizeof(cwd));
        if (flag){strcat(cwd, "1\0"); strcat(cwd , tmp + 1);}
        find_name(".neogit");
        add_to_file(cwd, ".neogit/staging");
        return 0;
    } else { perror("invalid command") ; return 1;}

    // return add_to_staging(argv[2]);
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
    if (argc < 4) {
        perror("please use the correct format");
        return 1;
    }
    
    char message[MAX_MESSAGE_LENGTH];
    strcpy(message, argv[3]);

    int commit_ID = inc_last_commit_ID();
    if (commit_ID == -1) return 1;
    
    FILE *file = fopen(".neogit/staging", "r");
    if (file == NULL) return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }
        
        if (!check_file_directory_exists(line)) {
            char dir_path[MAX_FILENAME_LENGTH];
            strcpy(dir_path, ".neogit/files/");
            strcat(dir_path, line);
            if (mkdir(dir_path) != 0) return 1;
        }
        printf("commit %s\n", line);
        commit_staged_file(commit_ID, line);
        track_file(line);
    }
    fclose(file); 
    
    // free staging
    file = fopen(".neogit/staging", "w");
    if (file == NULL) return 1;
    fclose(file);

    create_commit_file(commit_ID, message);
    fprintf(stdout, "commit successfully with commit ID %d", commit_ID);
    
    return 0;
}

// returns new commit_ID
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

int run_checkout(int argc, char * const argv[]) {
    if (argc < 3) return 1;
    
    int commit_ID = atoi(argv[2]);

    DIR *dir = opendir(".");
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && is_tracked(entry->d_name)) {
            checkout_file(entry->d_name, find_file_last_change_before_commit(entry->d_name, commit_ID));
        }
    }
    closedir(dir);

    return 0;
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
    } else if (strcmp(argv[1], "add") == 0) {
        return run_add(argc, argv);
    } else if (strcmp(argv[1], "reset") == 0) {
        return run_reset(argc, argv);
    } else if (strcmp(argv[1], "commit") == 0) {
        return run_commit(argc, argv);
    } else if (strcmp(argv[1], "checkout") == 0) {
        return run_checkout(argc, argv);
    } else if ( strcmp(argv[1], "config") == 0) {
        return run_config(argc, argv);
    } else if ( argc == 2 ) { 
        return run_alias(argc, argv);
    } else {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    return 0;
}