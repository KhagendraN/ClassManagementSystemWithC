// Author: Khagendra Neupane

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_SUBJECTS 10
#define MAX_TEACHERS 3
#define MAX_STUDENTS 100
#define MAX_CLASSES 10
#define PASSWORD "khagendra123"

struct Class {
    char name[50];
    char subject_names[MAX_SUBJECTS][50];
    char teacher_names[MAX_SUBJECTS][MAX_TEACHERS][50];
    char student_names[MAX_STUDENTS][50];
    int student_count;
};

struct Class classes[MAX_CLASSES];
int class_count = 0;

int create_directory(const char* path) {
    if (mkdir(path, 0777) == -1) {
        if (errno != EEXIST) {
            printf("Error creating directory: %s\n", path);
            return 0;
        }
    }
    return 1;
}

void save_classes() {
    FILE *file = fopen("classes.dat", "wb");
    if (file) {
        fwrite(&class_count, sizeof(class_count), 1, file);
        fwrite(classes, sizeof(struct Class), class_count, file);
        fclose(file);
    }
}

void load_classes() {
    FILE *file = fopen("classes.dat", "rb");
    if (file) {
        fread(&class_count, sizeof(class_count), 1, file);
        fread(classes, sizeof(struct Class), class_count, file);
        fclose(file);
    }
}

int class_exists(const char *class_name) {
    for (int i = 0; i < class_count; i++) {
        if (strcmp(classes[i].name, class_name) == 0) {
            return 1;
        }
    }
    return 0;
}

void create_class() {
    if (class_count >= MAX_CLASSES) {
        printf("Maximum classes reached.\n");
        return;
    }

    char class_name[50];
    printf("Enter name of class: ");
    scanf(" %49[^\n]", class_name);

    if (class_exists(class_name)) {
        printf("Class already exists.\n");
        return;
    }

    strcpy(classes[class_count].name, class_name);

    if (!create_directory(class_name)) {
        return;
    }

    printf("Enter number of subjects: ");
    int num_subjects;
    scanf("%d", &num_subjects);

    for (int i = 0; i < num_subjects; i++) {
        printf("Enter name of subject %d: ", i + 1);
        scanf(" %49[^\n]", classes[class_count].subject_names[i]);

        char subject_dir[200];
        snprintf(subject_dir, sizeof(subject_dir), "%s/%s", class_name, classes[class_count].subject_names[i]);

        if (!create_directory(subject_dir)) {
            return;
        }

        printf("Enter number of teachers: ");
        int num_teachers;
        scanf("%d", &num_teachers);

        for (int j = 0; j < num_teachers; j++) {
            printf("Enter name of teacher %d for subject %s: ", j + 1, classes[class_count].subject_names[i]);
            scanf(" %49[^\n]", classes[class_count].teacher_names[i][j]);

            char filename[200];
            snprintf(filename, sizeof(filename), "%s/%s.txt", subject_dir, classes[class_count].teacher_names[i][j]);

            FILE *file = fopen(filename, "w");
            if (file == NULL) {
                printf("Error creating file for teacher %s\n", classes[class_count].teacher_names[i][j]);
                return;
            }
            fprintf(file, "Students   Remarks\n");
            fclose(file);
        }
    }

    printf("Enter number of students: ");
    int num_students;
    scanf("%d", &num_students);

    for (int i = 0; i < num_students; i++) {
        snprintf(classes[class_count].student_names[i], 50, "Student %d", i + 1);
    }

    classes[class_count].student_count = num_students;

    for (int i = 0; i < num_subjects; i++) {
        char subject_dir[200];
        snprintf(subject_dir, sizeof(subject_dir), "%s/%s", class_name, classes[class_count].subject_names[i]);

        for (int j = 0; j < MAX_TEACHERS; j++) {
            if (strlen(classes[class_count].teacher_names[i][j]) == 0) continue;

            char filename[200];
            snprintf(filename, sizeof(filename), "%s/%s.txt", subject_dir, classes[class_count].teacher_names[i][j]);

            FILE *file = fopen(filename, "a");
            if (file == NULL) {
                printf("Error opening file for %s\n", classes[class_count].teacher_names[i][j]);
                return;
            }

            for (int k = 0; k < num_students; k++) {
                fprintf(file, "%s   (0, 0)\n", classes[class_count].student_names[k]);
            }

            fclose(file);
        }
    }

    class_count++;
    save_classes();
    printf("Class created successfully.\n");
}

void take_attendance(int selected_class, int selected_subject, int selected_teacher) {
    char date[50];
    printf("Enter date (e.g., jan2): ");
    scanf(" %49[^\n]", date);

    char filename[200];
    snprintf(filename, sizeof(filename), "%s/%s/%s.txt",
             classes[selected_class - 1].name,
             classes[selected_class - 1].subject_names[selected_subject - 1],
             classes[selected_class - 1].teacher_names[selected_subject - 1][selected_teacher - 1]);

    FILE *file = fopen(filename, "r");
    int file_exists = (file != NULL);

    char buffer[1024];
    char new_header[1024] = "Students   Remarks";
    int has_date = 0;

    FILE *temp = fopen("temp.txt", "w");
    if (!file_exists) {
        fprintf(temp, "Students   %s   Remarks\n", date);
        for (int i = 0; i < classes[selected_class - 1].student_count; i++) {
            fprintf(temp, "Student %d   -\n", i + 1);  // Just add the student names with no attendance recorded yet
        }
    } else {
        fgets(buffer, sizeof(buffer), file);
        strcpy(new_header, buffer);

        if (strstr(buffer, date) != NULL) {
            has_date = 1;
        } else {
            char *pos = strstr(new_header, "Remarks");
            if (pos != NULL) {
                int index = pos - new_header;
                new_header[index - 1] = '\0';
                strcat(new_header, "   ");
                strcat(new_header, date);
                strcat(new_header, "   Remarks");
            }
        }
        fprintf(temp, "%s\n", new_header);

        int line_no = 0;
        while (fgets(buffer, sizeof(buffer), file)) {
            line_no++;
            if (line_no <= classes[selected_class - 1].student_count) {
                buffer[strcspn(buffer, "\n")] = 0;  // Remove the newline at the end of the string
                char attendance[2];
                printf("Enter attendance for %s (p for present, a for absent): ", buffer);
                scanf(" %1s", attendance);  // Just store 'p' or 'a' for attendance

                // If the date is not already in the header, append it
                if (!has_date) {
                    fprintf(temp, "%s   %s\n", buffer, attendance);  // Just store attendance (p or a) without counts
                } else {
                    fprintf(temp, "%s\n", buffer);  // If date is already present, we just append the student name
                }
            }
        }
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);

    printf("Attendance recorded successfully.\n");
}

void view_attendance(int selected_class, int selected_subject, int selected_teacher) {
    char filename[200];
    snprintf(filename, sizeof(filename), "%s/%s/%s.txt",
             classes[selected_class - 1].name,
             classes[selected_class - 1].subject_names[selected_subject - 1],
             classes[selected_class - 1].teacher_names[selected_subject - 1][selected_teacher - 1]);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for viewing attendance.\n");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    fclose(file);
}

void manage_class() {
    if (class_count == 0) {
        printf("No classes have been created.\n");
        return;
    }

    printf("Available classes:\n");
    for (int i = 0; i < class_count; i++) {
        printf("%d. %s\n", i + 1, classes[i].name);
    }

    printf("Select a class: ");
    int selected_class;
    scanf("%d", &selected_class);

    if (selected_class < 1 || selected_class > class_count) {
        printf("Invalid class selection.\n");
        return;
    }

    char entered_password[50];
    printf("Enter password to manage class: ");
    scanf(" %49[^\n]", entered_password);

    if (strcmp(entered_password, PASSWORD) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    printf("Select subject:\n");
    for (int i = 0; i < MAX_SUBJECTS; i++) {
        if (strlen(classes[selected_class - 1].subject_names[i]) != 0) {
            printf("%d. %s\n", i + 1, classes[selected_class - 1].subject_names[i]);
        }
    }

    int selected_subject;
    scanf("%d", &selected_subject);

    if (selected_subject < 1 || selected_subject > MAX_SUBJECTS || strlen(classes[selected_class - 1].subject_names[selected_subject - 1]) == 0) {
        printf("Invalid subject selection.\n");
        return;
    }

    printf("Select teacher:\n");
    for (int i = 0; i < MAX_TEACHERS; i++) {
        if (strlen(classes[selected_class - 1].teacher_names[selected_subject - 1][i]) != 0) {
            printf("%d. %s\n", i + 1, classes[selected_class - 1].teacher_names[selected_subject - 1][i]);
        }
    }

    int selected_teacher;
    scanf("%d", &selected_teacher);

    if (selected_teacher < 1 || selected_teacher > MAX_TEACHERS || strlen(classes[selected_class - 1].teacher_names[selected_subject - 1][selected_teacher - 1]) == 0) {
        printf("Invalid teacher selection.\n");
        return;
    }

    printf("Manage Attendance:\n1. Take Attendance\n2. View Attendance Status\nEnter choice: ");
    int attendance_choice;
    scanf("%d", &attendance_choice);

    switch (attendance_choice) {
        case 1: take_attendance(selected_class, selected_subject, selected_teacher); break;
        case 2: view_attendance(selected_class, selected_subject, selected_teacher); break;
        default: printf("Invalid choice.\n");
    }
}

int main() {
    load_classes();
    printf("\033[1;31m"); // Set text color to red
    printf(R"(
╔═══╗╔╗─╔╗────────╔╗────────────╔═╗╔═╗───────────────────────╔╗─╔═══╗──────╔╗
║╔═╗╠╝╚╦╝╚╗───────║║────────────║║╚╝║║──────────────────────╔╝╚╗║╔═╗║─────╔╝╚╗
║║─║╠╗╔╩╗╔╬══╦═╗╔═╝╠══╦═╗╔══╦══╗║╔╗╔╗╠══╦═╗╔══╦══╦══╦╗╔╦══╦═╬╗╔╝║╚══╦╗─╔╦═╩╗╔╬══╦╗╔╗
║╚═╝║║║─║║║║═╣╔╗╣╔╗║╔╗║╔╗╣╔═╣║═╣║║║║║║╔╗║╔╗╣╔╗║╔╗║║═╣╚╝║║═╣╔╗╣║─╚══╗║║─║║══╣║║║═╣╚╝║
║╔═╗║║╚╗║╚╣║═╣║║║╚╝║╔╗║║║║╚═╣║═╣║║║║║║╔╗║║║║╔╗║╚╝║║═╣║║║║═╣║║║╚╗║╚═╝║╚═╝╠══║╚╣║═╣║║║
╚╝─╚╝╚═╝╚═╩══╩╝╚╩══╩╝╚╩╝╚╩══╩══╝╚╝╚╝╚╩╝╚╩╝╚╩╝╚╩═╗╠══╩╩╩╩══╩╝╚╩═╝╚═══╩═╗╔╩══╩═╩══╩╩╩╝
──────────────────────────────────────────────╔═╝║──────────────────╔═╝║
──────────────────────────────────────────────╚══╝──────────────────╚══╝)");
    printf("\033[0m"); // Reset text color

    int choice;
    while (1) {
        printf("\nMenu:\n1. Create Class\n2. Manage Class\n3. Exit\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: create_class(); break;
            case 2: manage_class(); break;
            case 3: save_classes(); return 0;
            default: printf("Invalid choice.\n");
        }
    }
}
