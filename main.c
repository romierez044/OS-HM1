#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int readFileAndUpdateMergedFile(char *fileName, FILE *mergedFile);
void report_input_file(const char* file_name, int num_stud);
void report_data_summary(int num_stud, double avg);
void updateGradesInMergedFile(char *student_line, FILE *mergedFile);

char mergedFileName[] = "merged.txt";

int main(int args, char **argv) {
    FILE *mergedFile = fopen(mergedFileName, "w");
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
    report_input_file(fileName, line_count);
    free(line_buf);
    line_buf = NULL;
    fclose(fp);
    return EXIT_SUCCESS;
}

void updateGradesInMergedFile(char *student_line, FILE *mergedFile) {
    int student_found = 0;
    char seps[] = " ";
    char *token_merged_file;
    char *token_input_file;
    /* getting student name from input file */
    char line_copy[80];
    strcpy(line_copy, student_line);
    token_input_file = strtok(line_copy, seps);

    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;

    line_size = getline(&line_buf, &line_buf_size, mergedFile);
    while (line_size >= 0 && student_found == 0)
    {
        /* getting student name from merged file */
        token_merged_file = strtok(line_buf, seps);
        /* compare students name */
        if (strcmp(token_merged_file, token_input_file)==0) {
            // TODO - adding the grapdes in the end of the line
            /* student already in the merged file */
            student_found = 1;
        }
        line_size = getline(&line_buf, &line_buf_size, mergedFile);
    }
    free(line_buf);
    line_buf = NULL;

    /* student isn't exist in the merged file */
    fseek(mergedFile, 0, SEEK_END);
    fprintf(mergedFile, "%s", student_line);
    /* back to the start of the merged file */
    fseek(mergedFile, 0, SEEK_SET);
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


//    char seps[] = " ";
//    char *token;
//    token = strtok( student_line, seps );
//    while( token != NULL ) {
//
//        token = strtok(NULL, seps ); //get next token
//    }