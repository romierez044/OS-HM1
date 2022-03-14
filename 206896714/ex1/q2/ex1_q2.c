#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int read_file_and_update_merged_file(char *file_name, FILE *merged_file);
void report_input_file(const char* file_name, int num_stud);
void report_data_summary(int num_stud, double avg);
void update_student_in_merged_file(char *student_line, FILE *merged_file);
void insert_grades_in_position(char *student_line, long position, FILE *merged_file);
void sort_grades_in_file(FILE *file, int *students_num, int *grades_sum, int *grades_num);

char merged_file_name[] = "merged.txt";
char tmp_file_name[] = "tmp_file.txt";

int cmpfunc (const void * a, const void * b) {
    return ( *(int*)b - *(int*)a );
}

int main(int args, char **argv) {
    int students_num = 0, grades_sum = 0, grades_num = 0;
    FILE *merged_file = fopen(merged_file_name, "r+");
    if (!merged_file)
    {
        fprintf(stderr, "Error opening file '%s'\n", merged_file_name);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < args; ++i) {
        int pid = fork();
        if (pid < 0) {
            perror("Cannot fork()") ;
            exit(EXIT_FAILURE) ;
        }
        if (pid == 0) { // child process
            return read_file_and_update_merged_file(argv[i], merged_file);
        }
        else
            wait(NULL);
    }
    sort_grades_in_file(merged_file, &students_num, &grades_sum, &grades_num);
    float avg = (grades_num != 0 ) ?  (grades_sum / (float)grades_num) : 0.0;
    report_data_summary(students_num, avg);
    return EXIT_SUCCESS;
}

int read_file_and_update_merged_file(char *file_name, FILE *merged_file) {
    char * line = NULL;
    size_t line_len = 0;
    int line_count = 0;
    FILE *input_file = fopen(file_name, "r");
    if (!input_file)
    {
        fprintf(stderr, "Error opening file '%s'\n", file_name);
        return EXIT_FAILURE;
    }

    while (getline(&line, &line_len, input_file) >= 0 && strcmp(line, "\n") != 0)
    {
        line_count++;
        update_student_in_merged_file(line, merged_file);
        fflush(merged_file);
    }
    report_input_file(file_name, line_count);
    free(line);
    fclose(input_file);
    return EXIT_SUCCESS;
}

void update_student_in_merged_file(char *student_line, FILE *merged_file) {
    if (student_line[strlen(student_line)-1] == '\n' ) student_line[strlen(student_line)-1]='\0';
    int student_found = 0;
    char *merged_file_token;
    char line_copy[160];
    strcpy(line_copy, student_line);
    /* getting student name from input file */
    char *input_file_token = strtok(line_copy, " ");

    char * line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, merged_file) >=0 && student_found == 0)
    {
        /* getting student name from merged file */
        merged_file_token = strtok(line, " ");
        /* compare students name */
        if (strcmp(merged_file_token, input_file_token) == 0) { /* student already in the merged file */
            insert_grades_in_position(student_line, ftell(merged_file), merged_file);
            fflush(merged_file);
            student_found = 1;
        }
    }
    free(line);

    /* student isn't exist in the merged file */
    if (student_found == 0){
        fseek(merged_file, 0, SEEK_END);
        fprintf(merged_file,"%s\n", student_line);
        fflush(merged_file);
    }

    /* back to the start of the merged file */
    fseek(merged_file, 0, SEEK_SET);
}

void insert_grades_in_position(char *student_line, long position, FILE *merged_file) {
    fseek(merged_file, 0, SEEK_END);
    long fin = ftell(merged_file);

    fseek(merged_file, position, SEEK_SET);
    long lSize = fin - position;
    char *buffer = (char*) malloc(sizeof(char) * lSize);
    fread(buffer, 1, lSize, merged_file);
    buffer[lSize] = 0;
    fseek(merged_file, position - 1, SEEK_SET);

    char *input_file_token;
    char seps[] = " ";
    char line_copy[160];
    strcpy(line_copy, student_line);
    /* get first grade */
    strtok(line_copy, seps);
    input_file_token = strtok(NULL, seps );
    while(input_file_token != NULL ) {
        fprintf(merged_file, " %s", input_file_token);
        fflush(merged_file);
        /* get next grade */
        input_file_token = strtok(NULL, seps );
    }
    fprintf(merged_file, "\n%s", buffer);
    fflush(merged_file);
    free(buffer);
}

void sort_grades_in_file(FILE *file, int *students_num, int *grades_sum, int *grades_num) {
    FILE *tmp_file = fopen(tmp_file_name, "w");
    fseek(file, 0, SEEK_SET);

    // prepare sorted grades in a tmp file
    char *token;
    size_t line_len = 0;
    char * line = NULL;
    int arr[10], i;
    while (getline(&line, &line_len, file) >= 0)
    {
        i = 0;
        (*students_num)++;
        token = strtok(line, " ");
        fprintf(tmp_file, "%s ", token);
        token = strtok(NULL, " ");
        while (token != NULL && strcmp(token, "\n") != 0) {
            if (token[strlen(token)-1] == '\n' ) token[strlen(token)-1]='\0';
            arr[i++] = atoi(token);
            *grades_sum = *grades_sum + atoi(token);
            token = strtok (NULL, " ");
        }
        *grades_num = *grades_num + i;
        qsort(arr, i, 4,cmpfunc);
        for (int j = 0; j < i; ++j) {
            fprintf(tmp_file, "%d ", arr[j]);
        }
        fprintf(tmp_file, "\n");
    }
    free(line);
    fclose(file);
    fclose(tmp_file);

    // transfer tmp file to merged file
    char ch;
    tmp_file = fopen(tmp_file_name, "r");
    FILE *merged_file = fopen(merged_file_name, "w");
    while((ch = fgetc(tmp_file) ) != EOF )
        fputc(ch, merged_file);
    fclose(tmp_file);
    fclose(merged_file);
    remove(tmp_file_name);
}

void report_data_summary(int num_stud, double avg)
{
    fprintf(stderr, "process: %d data summary - number of students:"
                    " %d grade average: %.2f\n",
            getpid(), num_stud, avg);
}

void report_input_file(const char* file_name, int num_stud)
{
    fprintf(stderr, "process: %d file: %s number of students: %d\n",
            getpid(), file_name, num_stud);
}