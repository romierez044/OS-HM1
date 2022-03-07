#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int readFileAndUpdateMergedFile(char *fileName, FILE *mergedFile);
void report_input_file(const char* file_name, int num_stud);
void report_data_summary(int num_stud, double avg);
void updateGradesInMergedFile(char *student_line, FILE *mergedFile);
void insert_grades_in_position(char *student_line, long position, FILE *mergedFile);

char mergedFileName[] = "merged.txt";
long size;
long position;

int main(int args, char **argv) {
    FILE *mergedFile = fopen(mergedFileName, "r+");
    setlinebuf(mergedFile);
    size = ftell(mergedFile); printf("after open. %d\n",size);
    for (int i = 1; i < args; ++i) {
        readFileAndUpdateMergedFile(argv[i], mergedFile);
    }

//    sortGradesInFile()
//    report_data_summary()
    fclose(mergedFile);
    return EXIT_SUCCESS;
}


int readFileAndUpdateMergedFile(char *fileName, FILE *mergedFile) {
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;
    FILE *fp = fopen(fileName, "r");
    if (!fp)
    {
        fprintf(stderr, "Error opening file '%s'\n", fileName);
        return EXIT_FAILURE;
    }

    line_size = getline(&line_buf, &line_buf_size, fp);
    while (line_size >= 0)
    {
        line_count++;
        printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_count,
               line_size, line_buf_size, line_buf);
        updateGradesInMergedFile(line_buf, mergedFile);
        line_size = getline(&line_buf, &line_buf_size, fp);
    }
//    report_input_file(fileName, line_count);
    free(line_buf);
    line_buf = NULL;
    fclose(fp);
    return EXIT_SUCCESS;
}

void updateGradesInMergedFile(char *student_line, FILE *mergedFile) {
    size=ftell(mergedFile);
    printf ("updateGradesInMergedFile Size of myfile.txt: %ld bytes.\n",size);
    int student_found = 0;
    char seps[] = " ";
    char *merged_file_token;
    char *input_file_token;
    /* getting student name from input file */
    char line_copy[80];
    strcpy(line_copy, student_line);
    input_file_token = strtok(line_copy, seps);

    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;
    size = ftell(mergedFile); printf("before getline. %d\n",size);
    line_size = getline(&line_buf, &line_buf_size, mergedFile);
    size = ftell(mergedFile); printf("after getline.%d\n", size);

    while (line_size >= 0 && student_found == 0)
    {
        /* getting student name from merged file */
        merged_file_token = strtok(line_buf, seps);
        /* compare students name */
        if (strcmp(merged_file_token, input_file_token) == 0) { /* student already in the merged file */
            position = ftell(mergedFile);
            insert_grades_in_position(student_line, position, mergedFile);
            student_found = 1;
        }
        line_size = getline(&line_buf, &line_buf_size, mergedFile);
    }
    free(line_buf);
    line_buf = NULL;

    /* student isn't exist in the merged file */
    if (student_found == 0){
        fseek(mergedFile, 0, SEEK_END);
        fprintf(mergedFile, "%s", student_line);
    }

    /* back to the start of the merged file */
    fseek(mergedFile, 0, SEEK_SET);
}

void insert_grades_in_position(char *student_line, long position, FILE *mergedFile) {
    fseek(mergedFile, 0L, SEEK_END);
    long Fin = ftell(mergedFile);
    fseek(mergedFile, position, SEEK_SET);
    long lSize = Fin - position;
    char *buffer = (char*) malloc(sizeof(char) * lSize);
    fread(buffer, 1, lSize, mergedFile);
    fseek(mergedFile, position - 1, SEEK_SET);

    char *input_file_token;
    char seps[] = " ";
    /* get first grade */
    strtok(student_line, seps);
    input_file_token = strtok(NULL, seps );
    while(input_file_token != NULL ) {
        fprintf(mergedFile, " %s\n", input_file_token);
        fflush(mergedFile);
        input_file_token = strtok(NULL, seps );
    }
    fputs(buffer, mergedFile);
    fflush(mergedFile);
}

void report_input_file(const char* file_name, int num_stud)
{
    fprintf(stderr, "process: %d file: %s number of students: %d\n",
            getpid(), file_name, num_stud);
}

void report_data_summary(int num_stud, double avg)
{
    fprintf(stderr, "process: %d data summary - number of students:"
                    " %d grade average: %.2f\n",
            getpid(), num_stud, avg);
}
